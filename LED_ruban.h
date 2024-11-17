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


#ifndef LED_RUBAN_H
#define LED_RUBAN_H

#include <stdint.h>

void LED_init();

void LED_tick();
void LED_setFullColor(uint8_t c);
void LED_getRainbowColor(uint8_t* r, uint8_t* g, uint8_t* b, uint8_t index);
void LED_clear();

void LED_setAlim(uint8_t alim);
void LED_setColorStep(uint8_t c);

void LED_clearScreen();
void LED_setPixel(uint8_t posx, uint8_t posy, uint8_t red, uint8_t green, uint8_t blue);
void LED_setNumber(uint8_t posx, uint8_t posy, uint8_t v, uint8_t cr, uint8_t cg, uint8_t cb);
void LED_setNumber2(uint8_t posx, uint8_t posy, uint8_t v, uint8_t cr, uint8_t cg, uint8_t cb);
void LED_setNumber4(uint8_t posx, uint8_t posy, uint16_t v, uint8_t cr, uint8_t cg, uint8_t cb);
void LED_refreshScreen();

#endif
