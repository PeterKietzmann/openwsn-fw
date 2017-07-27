/**
\brief riot-adaption-specific definition of the "uart" bsp module.


\author Stefan Mehner <stefan.mehner@b-tu.de>, July 2017
\author Peter Kietzmann  <peter.kietzmann@haw-hamburg.de>, July 2017


This code is mostly copied from openwsn-fw/bsp/boards/iot-lab_M3/uart.c
*/

#include "uart_ow.h"
#include "uart_stdio.h"
#include "periph/uart.h"

#define ENABLE_DEBUG                (1)

#include "debug.h"

//=========================== defines =========================================

//=========================== variables =======================================

typedef struct {
   uart_tx_cbt txCb;
   uart_rx_cbt rxCb;
} uart_vars_t;

uart_vars_t uart_vars;

uint8_t received_byte = 0;

//=========================== prototypes ======================================

//=========================== public ==========================================

void uart_init_ow(void) {
    DEBUG("uart_init_ow\n");

    uint32_t baudrate = 115200;

    /* the main part of the initialization is done in the RIOT uart_init()*/
    uart_init(UART_DEV(1), baudrate, NULL, NULL);

    DEBUG("Done initializing uart\n");
}

void uart_setCallbacks(uart_tx_cbt txCb, uart_rx_cbt rxCb) {
    DEBUG("uart_setCallbacks\n");
    uart_vars.txCb = txCb;
    uart_vars.rxCb = rxCb;
}

void uart_enableInterrupts(void){
  DEBUG("uart_enableInterrupts\n");
}

void uart_disableInterrupts(void){
  DEBUG("uart_disableInterrupts\n");
}

void uart_clearRxInterrupts(void){
    DEBUG("uart_clearRxInterrupts\n");
}

void uart_clearTxInterrupts(void){
    // clear tx interrupt flag
    DEBUG("uart_clearTxInterrupts\n");
}

void uart_writeByte(uint8_t byteToWrite){
    // write one byte to UART TX BUFFER
    DEBUG("uart_writeByte \n");
    uart_write(UART_DEV(1), &byteToWrite, 1);
}

uint8_t uart_readByte(void){
  DEBUG("uart_readByte\n");
    // read one byte to UART RX BUFFER
  return 0;
}
