/*
 * uart.h
 *
 *  Created on: 21-Feb-2025
 *      Author: vigneshv
 */

#ifndef UART_H_
#define UART_H_
#include<stdint.h>

#define SEGGER_USRT_PORT (0)
#define NULL (void*)0

typedef void (*rx_cmplt_f_ptr_t)(unsigned int, unsigned char);
typedef int (*tx_cmplt_f_ptr_t)(unsigned int);

void BSP_UART_Init(void);
void BSP_UART_SetReadCallback(uint8_t, rx_cmplt_f_ptr_t);
void BSP_UART_SetWriteCallback(uint8_t, tx_cmplt_f_ptr_t);
void BSP_UART_Write1(uint8_t, uint8_t);
void uart_init(void);
void uart_send_byte(uint8_t);

#endif /* UART_H_ */
