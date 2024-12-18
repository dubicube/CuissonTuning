/*|==========================================================================|*/
/*|  ____        _    _  _              _                                    |*/
/*| |    \  _ _ | |_ |_|| |_  ___  ___ | |_                                  |*/
/*| |  |  || | || . || ||  _|| -_||  _||   |                                 |*/
/*| |____/ |___||___||_||_|  |___||___||_|_|                                 |*/
/*|                                                                          |*/
/*| dubicube@gmail.com                                                       |*/
/*|==========================================================================|*/
/*| File name: LED_ruban.c                                                   |*/
/*|                                                                          |*/
/*| Description: LED strip controller.                                       |*/
/*| Controls a LED strip with some pre-configured sequences.                 |*/
/*| Does not use memory to store the state of each LED.                      |*/
/*| LED state is computed at each refresh request.                           |*/
/*| This allows to control an infinite amount of LEDs without being limited  |*/
/*| by the available RAM on the mcu.                                         |*/
/*|                                                                          |*/
/*|==========================================================================|*/
/*| 24/04/2022 | Creation                                                    |*/
/*|            |                                                             |*/
/*|==========================================================================|*/


#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "PLQ_plaque.h"
#include "LED_ruban.h"
#include "LED_config.h"

// Timing constants for protocol
#define T0H 0.1
#define T0L 0.4
#define T1H 0.50
#define T1L 0
#define TRES 50 // Reset timing

// // Timing constants for protocol
// #define T0H 0
// #define T0L 0.4
// #define T1H 0.4
// #define T1L 0
// #define TRES 50 // Reset timing


// Some internal variables
uint8_t LED_colorr, LED_colorg, LED_colorb;


// Send a byte to the leds
void LED_sendByte(uint8_t b) {
   uint8_t i;
   for (i = 0; i < 8; i++) {
      if (b&0x80) {
         LED_PORT = LED_PORT | LED_MASK;
         _delay_us(T1H);
         LED_PORT = LED_PORT & (~LED_MASK);
         _delay_us(T1L);
      } else {
         LED_PORT = LED_PORT | LED_MASK;
         _delay_us(T0H);
         LED_PORT = LED_PORT & (~LED_MASK);
         _delay_us(T0L);
      }
      b = b<<1;
   }
}
void LED_reset() {
    LED_PORT = LED_PORT & (~LED_MASK);
    _delay_us(TRES);
}

// Store RGB data to leds
void LED_sendRGB(uint8_t r, uint8_t g, uint8_t b) {
    LED_sendByte(g);
    LED_sendByte(r);
    LED_sendByte(b);
}

// Magic functrion to convert a value between 0 and 255
// to a color in a continous rainbow
void LED_getRainbowColor(uint8_t* r, uint8_t* g, uint8_t* b, uint8_t index) {
    uint16_t index16 = (uint16_t)index*6;
    uint8_t phase = index16/256;
    uint16_t phaseStep = index16%256;
    if (phase == 0) {
        *g = phaseStep*MAX_GREEN/256;
    } else if (phase == 1 || phase == 2) {
        *g = MAX_GREEN;
    } else if (phase == 3) {
        *g = (255-phaseStep)*MAX_GREEN/256;
    } else {
        *g = 0;
    }
    if (phase == 2) {
        *r = phaseStep*MAX_RED/256;
    } else if (phase == 3 || phase == 4) {
        *r = MAX_RED;
    } else if (phase == 5) {
        *r = (255-phaseStep)*MAX_RED/256;
    } else {
        *r = 0;
    }
    if (phase == 4) {
        *b = phaseStep*MAX_BLUE/256;
    } else if (phase == 5 || phase == 0) {
        *b = MAX_BLUE;
    } else if (phase == 1) {
        *b = (255-phaseStep)*MAX_BLUE/256;
    } else {
        *b = 0;
    }
}
void LED_getRandomColor(uint8_t* r, uint8_t* g, uint8_t* b) {
    *r = rand()%MAX_RED;
    *g = rand()%MAX_GREEN;
    *b = rand()%MAX_BLUE;
}
void LED_getRandomColor2(uint8_t* r, uint8_t* g, uint8_t* b) {
    do {
        *r = (rand()%2)*MAX_RED;
        *g = (rand()%2)*MAX_GREEN;
        *b = (rand()%2)*MAX_BLUE;
    } while (*r == 0 && *g == 0 && *b == 0);
}
void LED_getRandomColor3(uint8_t* r, uint8_t* g, uint8_t* b) {
    LED_getRainbowColor(r, g, b, (uint8_t)rand());
}

uint8_t cfg_time = 200;
uint8_t cfg_color = 0;
uint8_t cfg_delta = 4;

uint8_t led_mode = 0;

extern Plaque plaques[2];

uint8_t clignoCounter = 0;
uint8_t clignoState = 0;

void LED_tick() {
    uint16_t i;

    clignoCounter++;
    if (clignoCounter > 20) {
        clignoCounter = 0;
        clignoState = 1-clignoState;
    }

    uint8_t frontOn = (plaques[0].power == 0 || plaques[0].isOk || led_mode) ? 1 : clignoState;
    uint8_t backOn  = (plaques[1].power == 0 || plaques[1].isOk || led_mode) ? 1 : clignoState;

    for (i = 0; i < NBR_LEDS; i++) {
        switch (led_mode) {
            case 0:
                if (i < NBR_LEDS/2) {
                    LED_getRainbowColor(&LED_colorr, &LED_colorg, &LED_colorb, cfg_color+i*cfg_delta);
                } else {
                    LED_getRainbowColor(&LED_colorr, &LED_colorg, &LED_colorb, cfg_color+(NBR_LEDS-1-i)*cfg_delta);
                }
                break;
            case 1:
                if (i < NBR_LEDS/4) {
                    LED_getRainbowColor(&LED_colorr, &LED_colorg, &LED_colorb, plaques[0].rainBowIndex);
                } else if (i < 3*(NBR_LEDS/4)) {
                    LED_getRainbowColor(&LED_colorr, &LED_colorg, &LED_colorb, plaques[1].rainBowIndex);
                } else if (i < NBR_LEDS) {
                    LED_getRainbowColor(&LED_colorr, &LED_colorg, &LED_colorb, plaques[0].rainBowIndex);
                }
            default:
                break;
        }
        // Cligno on/off
        if (i < NBR_LEDS/4 || i >= 3*(NBR_LEDS/4)) {
            // Front
            if (!frontOn) {
                LED_colorr = 0;
                LED_colorg = 0;
                LED_colorb = 0;
            }
        } else {
            // Back
            if (!backOn) {
                LED_colorr = 0;
                LED_colorg = 0;
                LED_colorb = 0;
            }
        }
        LED_sendRGB(LED_colorr, LED_colorg, LED_colorb);
    }

    if (cfg_time != 0xFF) {
        cfg_color++;
    }
}

void LED_setFullColor(uint8_t c) {
    uint16_t i;
    LED_getRainbowColor(&LED_colorr, &LED_colorg, &LED_colorb, c);
    for (i = 0; i < NBR_LEDS; i++) {
        LED_sendRGB(LED_colorr, LED_colorg, LED_colorb);
    }
}

void LED_clear() {
    uint16_t i;
    for (i = 0; i < NBR_LEDS; i++) {
        LED_sendRGB(0, 0, 0);
    }
}

void LED_init() {
    LED_DDR = LED_DDR | LED_MASK;
    LED_clear();
}
