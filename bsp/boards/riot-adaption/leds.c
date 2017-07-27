/**
\brief riot-adaption-specific definition of the "leds" bsp module.
*/

#include "leds.h"

//=========================== defines =========================================

//=========================== variables =======================================

//=========================== prototypes ======================================

//=========================== public ==========================================

void leds_init(void) {
    // configure 4 leds as output, presents as error, sync, debug, radio
}

void leds_error_on(void) {

}

void leds_error_off(void) {

}

void leds_error_toggle(void) {

}

uint8_t leds_error_isOn(void) {
    return 0;
}

void leds_error_blink(void) {
    // blink error led for 10 seconds
    // this function is called when there is critical error happens,
    // the board will blink error led and reset later
}

void leds_radio_on(void) {

}

void leds_radio_off(void) {

}

void leds_radio_toggle(void) {

}

uint8_t leds_radio_isOn(void) {
    return 0;
}

void leds_sync_on(void) {

}

void leds_sync_off(void) {

}

void leds_sync_toggle(void) {

}

uint8_t leds_sync_isOn(void) {
    return 0;
}

void leds_debug_on(void) {

}

void leds_debug_off(void) {

}

void leds_debug_toggle(void) {

}

uint8_t leds_debug_isOn(void) {
    return 0;
}

void leds_all_on(void) {

}

void leds_all_off(void) {

}

void leds_all_toggle(void) {

}

void leds_circular_shift(void) {
    // turn on and off each led in sequence
}

void leds_increment(void) {
    // turn on and off the leds as a binary counter
}
