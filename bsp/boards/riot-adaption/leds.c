/**
 * @brief Provides an adaption of OpenWSN LED handling to RIOTs handling of LEDs. 
 *
 * The adaption aims to support every (suited) RIOT board. However, this comes with the 
 * limiation of not providing leds_{type}_isOn function calls since these differ from 
 * board to board. Also, this adaption assumes that there are four LEDs, while some boards
 * such as the iotlab-m3 have only three LEDs.
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Michael Frey <michael.frey@msasafety.com> 
 */

#include "leds.h"

/** includes specific to riot */
#include "board.h"
#include "periph/gpio.h"

#include <stdint.h>

#ifndef LED0_PIN
#define LED0_ON
#define LED0_OFF
#define LED0_TOGGLE
#endif 

#ifndef LED1_PIN
#define LED1_ON
#define LED1_OFF
#define LED1_TOGGLE
#endif 

#ifndef LED2_PIN
#define LED2_ON
#define LED2_OFF
#define LED2_TOGGLE
#endif 

#ifndef LED3_PIN
#define LED3_ON
#define LED3_OFF
#define LED3_TOGGLE
#endif 

/**
 * Provides a simple delay by means of iterating over a 32-bit variable
 */
static void leds_delay(void);

/**
 * Configure four leds as output which provide error, sync, debug, and
 * radio status information.
 */
void leds_init(void) {
    /** led color: red (ERROR) */
    #ifdef LED0_PIN
        gpio_init(LED0_PIN, GPIO_OUT);
        gpio_set(LED0_PIN);
    #endif

    /** led color: orange (RADIO) */
    #ifdef LED1_PIN
        gpio_init(LED1_PIN, GPIO_OUT);
        gpio_set(LED1_PIN);
    #endif

    /** led color: green (SYNC) */
    #ifdef LED2_PIN
        gpio_init(LED2_PIN, GPIO_OUT);
        gpio_set(LED2_PIN);
    #endif

    /** led color: yellow */
    #ifdef LED3_PIN
        gpio_init(LED3_PIN, GPIO_OUT);
        gpio_set(LED3_PIN);
    #endif
}

void leds_error_on(void) {
   	LED0_ON;
}

void leds_error_off(void) {
   	LED0_OFF;
}

void leds_error_toggle(void) {
   	LED0_TOGGLE;
}

uint8_t leds_error_isOn(void) {
	/** not implemented */
    return 0;
}

/**
 * Let the error led blink for "10 seconds". 
 */
void leds_error_blink(void) {    
	for (uint8_t i = 0; i < 16; i++) {
        leds_error_toggle();
        leds_delay();
    }
}

void leds_radio_on(void) {
   	LED1_ON;
}

void leds_radio_off(void) {
   	LED1_OFF;
}

void leds_radio_toggle(void) {
   	LED1_TOGGLE;
}

uint8_t leds_radio_isOn(void) {
	/** not implemented */
    return 0;
}

void leds_sync_on(void) {
	LED2_ON;
}

void leds_sync_off(void) {
	LED2_OFF;
}

void leds_sync_toggle(void) {
	LED2_TOGGLE;
}

uint8_t leds_sync_isOn(void) {
	/** not implemented */
    return 0;
}

void leds_debug_on(void) {
	LED3_ON;
}

void leds_debug_off(void) {
	LED3_OFF;
}

void leds_debug_toggle(void) {
	LED3_TOGGLE;
}

uint8_t leds_debug_isOn(void) {
	/** not implemented */
    return 0;
}

void leds_all_on(void) {
    leds_error_on();
    leds_radio_on();
    leds_sync_on();
    leds_debug_on();
}

void leds_all_off(void) {
    leds_error_off();
    leds_radio_off();
    leds_sync_off();
    leds_debug_off();
}

void leds_all_toggle(void) {
    leds_error_toggle();
    leds_radio_toggle();
    leds_sync_toggle();
    leds_debug_toggle();
}

void leds_circular_shift(void) {
    leds_error_on();
    leds_delay();
    leds_error_off();
    leds_delay();
    leds_radio_on();
    leds_delay();
    leds_radio_off();
    leds_delay();
    leds_sync_on();
    leds_delay();
    leds_sync_off();
    leds_delay();
    leds_debug_on();
    leds_delay();
    leds_debug_off();
}

/**
 * Turns on and off the leds as a binary counter.
 */
void leds_increment(void) {
	/** not implemented */
	return;
}


static void leds_delay(void) {
    for(uint32_t i = 0; i < 0x7FFF8; i++);
}
