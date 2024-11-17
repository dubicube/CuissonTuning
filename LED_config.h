#ifndef LED_CONFIG_H
#define LED_CONFIG_H

#define CONFIG0


#ifdef CONFIG0

// Define which pin is used to control the leds
#define LED_DDR DDRB
#define LED_PORT PORTB
#define LED_MASK 0x02

// Number of leds to control
#define NBR_LEDS (66*2)

#define NBR_EDGES 2
const uint8_t LED_EDGES[NBR_EDGES] = {66, 66};


// Maximum LED brightness
// Allows to limit the power consumption
#define MAX_RED 64
#define MAX_GREEN 128
#define MAX_BLUE 128

#endif

//////////////////////////////////////////////////////

#ifdef CONFIG1

// Define which pin is used to control the leds
#define LED_DDR DDRD
#define LED_PORT PORTD
#define LED_MASK 0x01

// Number of leds to control
#define NBR_LEDS (66*2)

#define NBR_EDGES 2
const uint8_t LED_EDGES[NBR_EDGES] = {66, 66};


// Maximum LED brightness
// Allows to limit the power consumption
#define MAX_COLOR 64

#endif


#endif
