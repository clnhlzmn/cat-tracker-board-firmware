/*
 * uart.c
 *
 * Created: 11/7/2019 8:02:31 AM
 *  Author: Colin
 */ 

#include <stdbool.h>
#include "sam.h"
#include "hal_atomic.h"
#include "queue.h"
#include "uart.h"

#define QUEUE_SIZE (128)
QUEUE(uart, uint8_t, QUEUE_SIZE);

static volatile struct queue_uart rx_queue;

void uart_init(void) {
    //enable SERCOM1 bus
    PM->APBCMASK.bit.SERCOM1_ = 1;

    //set pin mux for SERCOM1 (peripheral D) to have control over pa16
    PORT->Group[0].PINCFG[16].bit.PMUXEN = 1;
    PORT->Group[0].PMUX[16/2].bit.PMUXE = MUX_PA16C_SERCOM1_PAD0;

    //enable gclk for SERCOM1 from gen 0
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_SERCOM1_CORE;

    SERCOM1->USART.CTRLA.bit.SWRST = 1;
    while (SERCOM1->USART.SYNCBUSY.bit.SWRST) {}

    SERCOM1->USART.CTRLA.bit.ENABLE = 0;
    while (SERCOM1->USART.SYNCBUSY.bit.ENABLE) {}

    //software reset
    SERCOM1->USART.CTRLA.bit.SWRST = 1;
    while (SERCOM1->USART.CTRLA.bit.SWRST || SERCOM1->USART.SYNCBUSY.bit.SWRST) {}
    
    //configure ctrla
    SERCOM1->USART.CTRLA.reg = 
        SERCOM_USART_CTRLA_MODE_USART_INT_CLK | 
        SERCOM_USART_CTRLA_RXPO(0) |
        SERCOM_USART_CTRLA_TXPO(1) |
        SERCOM_USART_CTRLA_DORD;

    //configure ctrlb
    SERCOM1->USART.CTRLB.reg = SERCOM_USART_CTRLB_RXEN;
    while (SERCOM1->USART.SYNCBUSY.bit.CTRLB) {}

    //set baud rate to 9600
    SERCOM1->USART.BAUD.reg = 65326;

    queue_uart_init(&rx_queue);

    //enable interrupts
    SERCOM1->USART.INTENSET.reg = 
        SERCOM_USART_INTENSET_RXC;

    NVIC_EnableIRQ(SERCOM1_IRQn);
    
    SERCOM1->USART.CTRLA.bit.ENABLE = 1;
    while (SERCOM1->USART.SYNCBUSY.bit.ENABLE) {}
}

int uart_receive(uint8_t *buf, int n) {
    if (!buf) return 0;
    int ret = 0;
    CRITICAL_SECTION_ENTER();
    //empty queue as much as possible
    while (n > 0) {
        if (queue_uart_pop(&rx_queue, &buf[ret]) != 0) {
            break;
        }
        ret++;
        n--;
    }
    CRITICAL_SECTION_LEAVE();
    return ret;
}

void SERCOM1_Handler(void) {
    if (SERCOM1->USART.INTFLAG.bit.RXC) {
        uint8_t rx = SERCOM1->USART.DATA.reg;
        queue_uart_push(&rx_queue, &rx);
    }
}