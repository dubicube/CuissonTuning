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

#endif
