/*|==========================================================================|*/
/*|  ____        _    _  _              _                                    |*/
/*| |    \  _ _ | |_ |_|| |_  ___  ___ | |_                                  |*/
/*| |  |  || | || . || ||  _|| -_||  _||   |                                 |*/
/*| |____/ |___||___||_||_|  |___||___||_|_|                                 |*/
/*|                                                                          |*/
/*| dubicube@gmail.com                                                       |*/
/*|==========================================================================|*/
/*|                                                                          |*/
/*|==========================================================================|*/
/*| 17/11/2024 | Creation                                                    |*/
/*|            |                                                             |*/
/*|==========================================================================|*/


// Msg every 20ms
// Bitrate: 19200 bit/s

// Typical messages (looping)
// RX: LENGTH COMMAND_BYTE RESPONSE_BYTE_0 RESPONSE_BYTE_1 RESPONSE_BYTE_2 ...
// RX: 01 2B
// RX: 01 EC
// RX: 0A 3C FF 00 00 00 00 00 00 00 00
// RX: 01 7D
// RX: 04 80 01 00 FE
// RX: 08 1F 00 00 00 00 00 00 FF
// RX: 08 20 06 01 00 00 00 00 F8
// RX: 08 61 00 00 00 00 00 00 FF
// RX: 08 E2 00 00 00 00 00 00 FF
// RX: 0A E9 00 40 0F 00 00 28 18 41 2F
// RX: 0A 6A 06 00 10 00 00 1C 0E B0 0F

// First byte is sent by one side, other bytes are the response sent by the other side
// The last byte of the response is very very likely to be a CRC,
// but I have no idea of the polynom that is used.

// Typical: 11 message, period 20ms => total loop = 220ms

// Sometimes, other messages appear
// Example :
// RX: 04 C1 0A 00 F5
// (appear sometimes after the "RX: 04 80" message)


// Estimated duration for LED update : < 5ms
// => Enough time to update leds between received messages, if synchronized correctly,
// without perturbing LED protocol timings with interrupts.

// With Timer 0 prescaler at 256, the overflow timeout (to detect end of message), is ~4ms.
// Worst case: 16ms idle time between 2 long messages => with the timeout + LED protocol, still enough time.

// The main loop is synchronized with the received messages,
// to synchronously unload messages in the main loop, and to update leds when there is no transmission.

#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "TRX_uart.h"
#include "LED_ruban.h"
#include "PLQ_plaque.h"


#define COM_DDR DDRB
#define COM_PIN PINB
#define COM_MASK 0x01


#define LEDPWR_DDR DDRB
#define LEDPWR_PORT PORTB
#define LEDPWR_MASK 0x20


extern uint8_t led_mode;


uint8_t protocolStep = 0;
uint8_t bitCounter = 0;
uint8_t rxReg = 0x00;
uint8_t byteIndex = 0;
uint8_t buffer[16];


uint8_t poweredOnFlag = 0;
Plaque plaques[2];

// Triggered when receiving message.
// Synchronizes the main loop.
#define SYNC_VOID 0x01
#define SYNC_MSG 0x02
volatile uint8_t synchroLoop = 0;


uint8_t p;

ISR (PCINT0_vect) {
    volatile uint8_t d = TCNT0;
    p = (COM_PIN & COM_MASK) ? 1 : 0;

    switch (protocolStep) {
        case 0:
            TCNT0 = 0;
            if (p != 0 && d > 44) {
                // Reset
                protocolStep = 1;
                bitCounter = 0;
                byteIndex = 0;
            }
            break;

        // Synchro
        case 1:
            TCNT0 = 0;
            if (p != 0 && d > 44) {
                // Reset
                protocolStep = 1;
                bitCounter = 0;
                byteIndex = 0;
            } else {
                bitCounter++;
                if (bitCounter == 10) {
                    protocolStep++;
                }
            }
            break;

        // Start bit
        case 2:
            TCNT0 = 0;
            if (p != 0 && d > 44) {
                // Reset
                protocolStep = 1;
                bitCounter = 0;
                byteIndex = 0;
            } else if (p == 0) {
                protocolStep++;
                OCR0A = 51; // Wait for half bit period, to sample in middle of bits
                TCCR0B = 0x02; // Prescaler: 8
                TCCR0A = 0x02; // CTC
                TIMSK0 |= (1<<OCIE0A); // Enable compare A interrupt
                // PCMSK0 &= ~(1<<PCINT0); // Disable pin change interrupt
            }
            break;

        // RX cmd byte
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;

        default:
            TCNT0 = 0;
            if (p != 0 && d > 44) {
                // Reset
                protocolStep = 1;
                bitCounter = 0;
                byteIndex = 0;
            }
            break;
    }
}

ISR (TIMER0_COMPA_vect) {
    if (protocolStep == 3) {
        OCR0A = 103; // Wait full bit period
        protocolStep++;
        bitCounter = 0;
    } else if (protocolStep == 4) {
        // Sample the 8 data bits
        p = (COM_PIN & COM_MASK) ? 1 : 0;
        // Seems to be MSB first
        rxReg = (rxReg>>1) | (p<<7);
        bitCounter++;
        if (bitCounter == 8) {
            buffer[byteIndex] = rxReg;
            byteIndex++;
            protocolStep++;
        }
    } else if (protocolStep == 5) {
        TCCR0A = 0x00; // Normal
        TCCR0B = 0x04; // Prescaler: 256
        TIMSK0 &= ~(1<<OCIE0A); // Disable compare A interrupt
        // PCMSK0 |= (1<<PCINT0); // Enable pin change interrupt
        protocolStep = 2;
        TCNT0 = 0;
    }
}

ISR (TIMER0_OVF_vect) {
    if (protocolStep == 2) {
        // Successfully received a complete message
        synchroLoop = SYNC_MSG;
    } else {
        // Timeout: no message received
        synchroLoop = SYNC_VOID;
    }
    protocolStep = 0;
}

int main() {

    PCMSK0 |= (1<<PCINT0);
    PCICR |= (1<<PCIE0);

    // TCCR1A = 0x00;
    // // Clock prescaler : 64
    // TCCR1B = 0x03;
    // TCCR1C = 0x00;


    TCCR0A = 0x00; // Normal
    TCCR0B = 0x04; // Prescaler: 256
    // Enable overflow interrupt
    TIMSK0 = 0x01;


    TRX_init(115200, TRX_DSPEED);
    LED_init();

    PLQ_init(&plaques[0]);
    PLQ_init(&plaques[1]);


    sei();

    COM_DDR &= ~COM_MASK;
    LEDPWR_DDR |= LEDPWR_MASK;

    uint8_t ledOn = 0;
    LEDPWR_PORT |= LEDPWR_MASK;

    while (1) {

        // Wait for synchronization
        do {
            synchroLoop = 0;
            while(!synchroLoop);
        } while(synchroLoop != SYNC_MSG);

        TRX_print("RX: ");
        TRX_printHex8(byteIndex);
        for (uint8_t i = 0; i < byteIndex; i++) {
            TRX_print(" ");
            TRX_printHex8(buffer[i]);
        }
        TRX_NEWLINE;

        // Decode received message
        switch (buffer[0]) {
            case 0x80:
                if (byteIndex == 4) {
                    poweredOnFlag = (buffer[1] & 0x01) ? 1 : 0;
                }
                break;
            case 0x20:
                if (byteIndex == 8) {
                    PLQ_receivePowerMsg(&plaques[0]);
                }
                break;
            case 0x1F:
                if (byteIndex == 8) {
                    PLQ_receivePowerMsg(&plaques[1]);
                }
                break;
            case 0x6A:
                if (byteIndex == 10) {
                    PLQ_receiveTempMsg(&plaques[0]);
                }
                break;
            case 0xE9:
                if (byteIndex == 10) {
                    PLQ_receiveTempMsg(&plaques[1]);
                }
                break;
            default:
                break;
        }


        if (ledOn == 0 && poweredOnFlag) {
            TRX_print("ON\r\n");
            ledOn = 0x01;
            LEDPWR_PORT &= ~LEDPWR_MASK;
        }

        if (ledOn && poweredOnFlag == 0) {
            TRX_print("OFF\r\n");
            ledOn = 0x00;
            LEDPWR_PORT |= LEDPWR_MASK;
            LED_clear();
        }


        led_mode = 0;
        PLQ_tick(&plaques[0]);
        PLQ_tick(&plaques[1]);

        if (ledOn) {
            cli();
            LED_tick();
            sei();
        }
    }
    return 0;
}
