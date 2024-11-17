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

#include "PLQ_plaque.h"

extern uint8_t buffer[16];

extern uint8_t led_mode;

void PLQ_init(Plaque* p) {
    p->sideSelected = 0;
    p->power = 0;
    p->isOk = 1;
}

void PLQ_receivePowerMsg(Plaque* p) {
    uint8_t v = buffer[2]&0x0F;
    if (v != p->power) {
        p->sideSelected = 90;
    }
    p->power = v;
}

void PLQ_receiveTempMsg(Plaque* p) {
    uint8_t v = buffer[2]&0x40;
    p->isOk = v ? 1 : 0;
}

void PLQ_tick(Plaque* p) {
    if (p->sideSelected) {
        led_mode = 1;
        p->sideSelected--;
    }
    p->rainBowIndex = p->power*25;
}