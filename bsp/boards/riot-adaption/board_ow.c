/**
\brief RIOT adaption definition of the "board" bsp module.

\author Thomas Watteyne <watteyne@eecs.berkeley.edu>, February 2012.
\author Chang Tengfei <tengfei.chang@gmail.com>,  July 2012.
\author Peter Kietzmann <peter.kietzmann@haw-hamburg.de>,  July 2017.
*/

#include <stdio.h>

#include "sctimer.h"
#include "radio.h"
#include "board.h"
#include "thread.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

void board_init_ow(void){
    puts("OpenWSN board_init");
    sctimer_init();
    // Done in RIOT by auto_init?
    radio_init();
}

void board_sleep(void){
}

void board_reset(void){
    puts("OpenWSN board_reset");
}
