/*|==========================================================================|*/
/*|  ____        _    _  _              _                                    |*/
/*| |    \  _ _ | |_ |_|| |_  ___  ___ | |_                                  |*/
/*| |  |  || | || . || ||  _|| -_||  _||   |                                 |*/
/*| |____/ |___||___||_||_|  |___||___||_|_|                                 |*/
/*|                                                                          |*/
/*| dubicube@gmail.com                                                       |*/
/*|==========================================================================|*/
/*| File name: TRX_uart.h                                                    |*/
/*|                                                                          |*/
/*| Description: UART driver for an AtMega328.                               |*/
/*| Can implement RX interrupt. If using it, make sure to globally enable    |*/
/*| interrupts, using sei() function.                                        |*/
/*|                                                                          |*/
/*|==========================================================================|*/
/*| 24/04/2022 | Creation                                                    |*/
/*|            |                                                             |*/
/*|==========================================================================|*/


#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <util/delay.h>
#include <avr/io.h>



#ifndef UBRR0

#define STX_BAUDRATE 9600
#define STX_US_PER_BIT (1000000/STX_BAUDRATE)

#define STX_PORT PORTB
#define STX_DDR  DDRB
#define STX_MASK 0x02


#define SRX_BAUDRATE 9600
#define SRX_US_PER_BIT (1000000/STX_BAUDRATE)

#define SRX_PORT PORTB
#define SRX_DDR  DDRB
#define SRX_PIN  PINB
#define SRX_MASK 0x01



#define TIMER_FREQ (F_CPU/64)
// Prescaler at 64 (16MHz clock, 9600 baud)
#define TCCR1_CFG 0x07
#define BAUDRATE 9600
#define CLK_PER_BIT (TIMER_FREQ/BAUDRATE)

#endif



#define TRX_NEWLINE {TRX_putc(0x0d); TRX_putc(0x0a);}

// Simple speed
#define TRX_SSPEED 0
// Double speed
#define TRX_DSPEED 1

#ifdef UBRR0
void TRX_init(uint32_t baudrate, uint8_t doubleSpeed);
#endif
#ifndef UBRR0
void STX_init();
void SRX_init();
#endif
void TRX_putc(const char);
void TRX_print(char*);
void TRX_printHex8(const uint8_t);
void TRX_printHex16(const uint16_t);
void TRX_printHex32(const uint32_t v);
void TRX_printDec(const uint32_t v);

unsigned char TRX_rxAvailable();
char TRX_getChar();
unsigned char TRX_getHex8();

void TRX_enableRXInterrupt(void(*rxHandler)(void));

#endif
