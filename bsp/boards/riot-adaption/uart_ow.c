/**
\brief riot-adaption-specific definition of the "uart" bsp module.

*/

#include "uart_ow.h"
// #include "board_ow.h"
#include "uart_stdio.h"
#include "periph/uart.h"

//=========================== defines =========================================

//=========================== variables =======================================

typedef struct {
   uart_tx_cbt txCb;
   uart_rx_cbt rxCb;
} uart_vars_t;

uart_vars_t uart_vars;

//=========================== prototypes ======================================

//=========================== public ==========================================

void uart_init_ow(void) {

    // 115200 baud
    uint32_t baudrate = 115200;

    // clocking source
    // modulation

    // enable UART1 TX/RX
    // clear UART1 reset bit

    uart_init(UART_STDIO_DEV, baudrate, uart_rx_isr, NULL);

}

void uart_setCallbacks(uart_tx_cbt txCb, uart_rx_cbt rxCb) {
    uart_vars.txCb = txCb;
    uart_vars.rxCb = rxCb;



}

void uart_enableInterrupts(void){
  // USART_ITConfig(USART1, USART_IT_TC,   ENABLE);
  // USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);



}

void uart_disableInterrupts(void){

}

void uart_clearRxInterrupts(void){
    // clear Rx interrupt flag, so another byte can be captured.
}

void uart_clearTxInterrupts(void){
    // clear tx interrupt flag
}

void uart_writeByte(uint8_t byteToWrite){
    // write one byte to UART TX BUFFER

    // uart_write(UART_STDIO_DEV, byteToWrite, 1);
}

uint8_t uart_readByte(void){
    // read one byte to UART RX BUFFER
  return 0;
}

//=========================== private =========================================

//=========================== interrupt handlers ==============================

void uart_tx_isr(void) {
   uart_clearTxInterrupts(); // TODO: do not clear, but disable when done
   uart_vars.txCb();
}

void uart_rx_isr(void) {
   uart_clearRxInterrupts(); // TODO: do not clear, but disable when done
   uart_vars.rxCb();
}
