/*|==========================================================================|*/
/*|  ____        _    _  _              _                                    |*/
/*| |    \  _ _ | |_ |_|| |_  ___  ___ | |_                                  |*/
/*| |  |  || | || . || ||  _|| -_||  _||   |                                 |*/
/*| |____/ |___||___||_||_|  |___||___||_|_|                                 |*/
/*|                                                                          |*/
/*| dubicube@gmail.com                                                       |*/
/*|==========================================================================|*/
/*| File name: TRX_uart.c                                                    |*/
/*|                                                                          |*/
/*| Description: UART driver for an AtMega328.                               |*/
/*| Can implement RX interrupt. If using it, make sure to globally enable    |*/
/*| interrupts, using sei() function.                                        |*/
/*|                                                                          |*/
/*|==========================================================================|*/
/*| 24/04/2022 | Creation                                                    |*/
/*|            |                                                             |*/
/*|==========================================================================|*/


#include "TRX_uart.h"
#include <avr/interrupt.h>
#include <avr/io.h>



const uint8_t hexaConvert[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

#ifdef UBRR0

void TRX_init(uint32_t baudrate, uint8_t doubleSpeed) {
    uint16_t ubrr;
    if (doubleSpeed) {
        ubrr = (F_CPU/8)/baudrate - 1;
    } else {
        ubrr = (F_CPU/16)/baudrate - 1;
    }
    // Setup baudrate
    UBRR0 = ubrr;
    // Set simple/double speed
    UCSR0A = (doubleSpeed&1)<<U2X0;
    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    // Async 1 stop bit 8bit char no parity bits
    UCSR0C = 6;
}

void TRX_putc(const char c) {
    // While TX is not ready
    while (!(UCSR0A & (1 << UDRE0)));
    // Send byte through TX channel
    UDR0 = c;
}

void (*TRX_rxHandler)(void);
void TRX_enableRXInterrupt(void(*rxHandler)(void)) {
    UCSR0B |= (1 << RXCIE0);
    TRX_rxHandler = rxHandler;
}
ISR (USART_RX_vect) {
    (*TRX_rxHandler)();
}

unsigned char TRX_rxAvailable(){
    return (UCSR0A>>RXC0)&1;
}
char TRX_getChar(){
    while(!TRX_rxAvailable());
    return UDR0;
}

#endif

#ifndef UBRR0

void STX_init() {
	STX_DDR |= STX_MASK;
	STX_PORT |= STX_MASK;
}

void TRX_putc(char c) {
	uint8_t i;
	STX_PORT &= ~STX_MASK;
	_delay_us(STX_US_PER_BIT);
	for (i = 0; i < 8; i++) {
		if (c & 0x01) {
			STX_PORT |= STX_MASK;
		} else {
			STX_PORT &= ~STX_MASK;
		}
		_delay_us(STX_US_PER_BIT);
		c >>= 1;
	}
	STX_PORT |= STX_MASK;
	_delay_us(STX_US_PER_BIT);
}

// RX with Timer 1

volatile uint8_t SRX_step = 0;

#define SRX_FIFO_SIZE 8
volatile uint8_t SRX_fifo[SRX_FIFO_SIZE];
volatile uint8_t SRX_wp = 0x00;
volatile uint8_t SRX_rp = 0x00;

void SRX_init() {
    TCCR1 = 0x00;
    TIMSK |= (1<<TOIE1);

    SRX_DDR &= ~SRX_MASK;
    SRX_PORT &= ~SRX_MASK;

    PCMSK |= SRX_MASK;
    GIMSK |= (1<<PCIE);
}

ISR (PCINT0_vect) {
    uint8_t d, v;
    if (SRX_step == 0) {
        TCNT1 = 0x00;
        TCCR1 = TCCR1_CFG;
        SRX_step = 1;
    } else {
        d = TCNT1;
        TCNT1 = 0x00;
        v = (SRX_PIN & SRX_MASK) ? 0 : 1;
        d = d + (CLK_PER_BIT/2);
        d = d/CLK_PER_BIT;
        SRX_step+=d;
        SRX_fifo[SRX_wp] = SRX_fifo[SRX_wp] >> d;
        if (v) {
            SRX_fifo[SRX_wp] = SRX_fifo[SRX_wp] | (0xFF<<(8-d));
        }
        if (SRX_step >= 10) {
            TCCR1 = 0x00;
            SRX_step = 0;
            SRX_wp = (SRX_wp+1)%SRX_FIFO_SIZE;
        }
    }
}

ISR (TIMER1_OVF_vect) {
    TCCR1 = 0x00;
    SRX_step = 0;
}

unsigned char TRX_rxAvailable(){
    return SRX_wp != SRX_rp;
}
char TRX_getChar(){
    char c = SRX_fifo[SRX_rp];
    SRX_rp = (SRX_rp+1)%SRX_FIFO_SIZE;
    return c;
}

#endif

void TRX_print(char* str) {
    while(*str) {
        TRX_putc(*str);
        str++;
    }
}

void TRX_printHex8(const uint8_t v) {
    TRX_putc(hexaConvert[v>>4]);
    TRX_putc(hexaConvert[v&0x0F]);
}

void TRX_printHex16(const uint16_t v) {
    TRX_printHex8(v>>8);
    TRX_printHex8(v);
}
void TRX_printHex32(const uint32_t v) {
    TRX_printHex8(v>>24);
    TRX_printHex8(v>>16);
    TRX_printHex8(v>>8);
    TRX_printHex8(v);
}
void TRX_printDec(const uint32_t v) {
    unsigned char decimalBuffer[16];
    unsigned char i = 0;
    uint32_t w = v;
    while(w) {
        decimalBuffer[i] = '0'+(w%10);
        w/=10;
        i++;
    }
    unsigned char j;
    for(j = i-1; j < i; j--) {
        TRX_putc(decimalBuffer[j]);
    }
}

unsigned char TRX_getHex8() {
    while(!TRX_rxAvailable());
    unsigned char c1 = TRX_getChar()-'0';
    if(c1 > 9)c1-=('A'-'0')-10;
    while(!TRX_rxAvailable());
    unsigned char c2 = TRX_getChar()-'0';
    if(c2 > 9)c2-=('A'-'0')-10;
    return (c1<<4)|c2;
}

