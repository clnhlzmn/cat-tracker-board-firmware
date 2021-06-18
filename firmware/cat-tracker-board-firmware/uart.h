/*
 * uart.h
 *
 * Created: 6/18/2021 10:41:49 AM
 *  Author: colin
 */ 


#ifndef UART_H_
#define UART_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void uart_init(void);

//receive into the given buffer of size n, returns number of characters received
int uart_receive(uint8_t *buf, int n);

#ifdef __cplusplus
}
#endif

#endif /* UART_H_ */
