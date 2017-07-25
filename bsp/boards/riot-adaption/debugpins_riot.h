/**
 * @brief Provides an adaption of OpenWSN debug pin handling to RIOTs handling of GPIOs. 
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw.de>
 * @author      Michael Frey <michael.frey@msasafety.com> 
 */
#ifndef __DEBUGPINS_RIOT_H
#define __DEBUGPINS_RIOT_H

/** RIOT specific includes */
#include "periph/gpio.h"

/**
 * Holds a configuration of debug pins for debugging OpenWSN
 */
typedef struct debugpins_config {
	gpio_t frame;       /**< debug pin for frames */
	gpio_t slot;        /**< debug pin for slots  */
	gpio_t fsm;         /**< debug pin for fsm */
	gpio_t task;        /**< debug pin for tasks */
	gpio_t isr;         /**< debug pin for interrupt service routines */
	gpio_t radio;       /**< debug pin for the radio */
} debugpins_config_t;

void debugpins_riot_init(debugpins_config_t *user_config);

#endif /* __DEBUGPINS_RIOT_H */
