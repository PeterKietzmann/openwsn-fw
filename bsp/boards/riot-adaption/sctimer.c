/**
\brief riot-adaption-specific definition of the "sctimer" bsp module.
*/

#include "board_info.h"
#include "sctimer.h"

#include "periph/timer.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

// TODO this is much too hardware dependant
// ========================== define ==========================================
#define TIMERLOOP_THRESHOLD          0xffffff     // 511 seconds @ 32768Hz clock
#define OVERFLOW_THRESHOLD           0x7fffffff   // as openmotestm32 uses 16kHz, the upper timer overflows when timer research to 0x7fffffff
#define MINIMUM_COMPAREVALE_ADVANCE  10

// ========================== variable ========================================

typedef struct {
    sctimer_cbt sctimer_cb;
    bool        convert;
    bool        convertUnlock;
} sctimer_vars_t;

sctimer_vars_t sctimer_vars;

static void sctimer_isr_internal(void *arg, int chan);

// ========================== protocol =========================================

/**
\brief Initialization sctimer.
*/
void sctimer_init(void){
    DEBUG("sctimer_init\n");
    memset(&sctimer_vars, 0, sizeof(sctimer_vars_t));
    // TODO introduce macro. Question: Should we use RTC here?
    if (timer_init(TIMER_DEV(1), 16000, sctimer_isr_internal, NULL) < 0) {
        DEBUG("sctimer_init: ERROR on initialization\n");
    }
}

void sctimer_set_callback(sctimer_cbt cb){
    DEBUG("sctimer_set_callback\n");
    sctimer_vars.sctimer_cb = cb;
}

/**
\brief set compare interrupt
*/
void sctimer_setCompare(uint32_t val){
    DEBUG("sctimer_setCompare\n");
    // enable the compare interrupt
    /*
    if (current Timer counter - val < TIMERLOOP_THRESHOLD){
        // the timer is already late, schedule the ISR right now manually.
        triggerTimerInterrupt();
    } else {
        if (val - current Timer counter < MINIMUM_COMPAREVALE_ADVANCE){}
            // there is hardware limitation to schedule the timer within TIMERTHRESHOLD ticks
            // schedule ISR right now manually
            triggerTimerInterrupt();
        } else {
            // schedule the timer at val
            setCompareValue(val);
        }
    }
    */
    // make sure convert flag conly toggle once within one overflow period
    if (val > OVERFLOW_THRESHOLD && sctimer_vars.convertUnlock){
        // toggle convert
        if (sctimer_vars.convert){
            sctimer_vars.convert   = true;
        } else {
            sctimer_vars.convert   = false;
        }
        sctimer_vars.convertUnlock = false;
    }
    
    // un lock the changes of convert flag
    if (val > TIMERLOOP_THRESHOLD && val < 2*TIMERLOOP_THRESHOLD ){
        sctimer_vars.convertUnlock = true;
    }
    
    // update value to be compared according to timer condition
    if (val <= OVERFLOW_THRESHOLD){
        if (sctimer_vars.convert){
            val  = val >>1;
            val |= 0x80000000;
        } else {
            val  = val >>1;
        }
    } else {
        if (sctimer_vars.convert){
            val  = val >>1;
        } else {
            val  = val >>1;
            val |= 0x80000000;
        }
    }

    DISABLE_INTERRUPTS();
    if (timer_read(TIMER_DEV(1)) - val < TIMERLOOP_THRESHOLD){
        // the timer is already late, schedule the ISR right now manually 
        timer_set(TIMER_DEV(1), 0, 0);
    } else {
        if (val-timer_read(TIMER_DEV(1))<MINIMUM_COMPAREVALE_ADVANCE){
            // schedule ISR right now manually
            timer_set(TIMER_DEV(1), 0, 0);
        } else {
            // schedule the timer at val
            timer_set(TIMER_DEV(1), 0, (unsigned int)val);
        }
    }

    ENABLE_INTERRUPTS();
}

/**
\brief Return the current value of the timer's counter.

 \returns The current value of the timer's counter.
*/
uint32_t sctimer_readCounter(void){
    DEBUG("sctimer_readCounter:%i\n", (int)timer_read(TIMER_DEV(1)));
    // TODO overflow handling? xtimer?
    return (uint32_t)timer_read(TIMER_DEV(1));
}

void sctimer_enable(void){
    DEBUG("sctimer_enable\n");
    timer_start(TIMER_DEV(1));
}

void sctimer_disable(void){
    DEBUG("sctimer_disable\n");
    timer_stop(TIMER_DEV(1));
}

// ========================== private =========================================

void sctimer_isr_internal(void *arg, int chan){
    (void)arg;
    (void)chan;
    DEBUG("sctimer_isr_internal\n");
    // debugpins_isr_set(); // for debugging

    if (sctimer_vars.sctimer_cb!=NULL) {
        sctimer_vars.sctimer_cb();
    }

    // debugpins_isr_clr(); // for debugging
}

//=========================== interrupt handlers ==============================

/*kick_scheduler_t sctimer_isr() {
    if (sctimer_vars.sctimer_cb!=NULL) {
        
        RCC_Wakeup();
        // call the callback
        sctimer_vars.sctimer_cb();
        // kick the OS
        return KICK_SCHEDULER;
    }
    return DO_NOT_KICK_SCHEDULER;
}*/
