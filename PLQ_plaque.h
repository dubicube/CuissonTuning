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

#ifndef PLQ_PLAQUE
#define PLQ_PLAQUE

#include <stdint.h>

struct Plaque {
    uint8_t sideSelected;
    uint8_t power;
    uint8_t isOk;
    uint8_t rainBowIndex;
};
typedef struct Plaque Plaque;


void PLQ_init(Plaque* p);
void PLQ_receivePowerMsg(Plaque* p);
void PLQ_receiveTempMsg(Plaque* p);
void PLQ_tick(Plaque* p);

#endif