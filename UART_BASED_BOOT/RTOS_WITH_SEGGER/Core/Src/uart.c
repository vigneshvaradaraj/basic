/*
 * uart.c
 *
 *  Created on: 21-Feb-2025
 *      Author: vigneshv
 */
#include "uart.h"
#include "stm32_f4xx.h"

rx_cmplt_f_ptr_t rx_func_callBack = NULL;
tx_cmplt_f_ptr_t tx_func_callBack = NULL;
void BSP_UART_Init(void)
{
	uart_init();
}
void BSP_UART_SetReadCallback(uint8_t port,rx_cmplt_f_ptr_t _SysView_OnRxComplete)
{
	rx_func_callBack = _SysView_OnRxComplete;
}
void BSP_UART_SetWriteCallback(uint8_t port,tx_cmplt_f_ptr_t _SysView_OnTxComplete)
{
	tx_func_callBack = _SysView_OnTxComplete;
}
void BSP_UART_Write1(uint8_t port, uint8_t data)
{
	uart_send_byte(data);
}

void uart_gpio_int(void)
{
	RCC->AHB1ENR |= (1 << 0);//enable clock for GPIOA
	GPIOA->MODER |= (2 << 18);//set alternate function for PA9
	GPIOA->MODER |= (2 << 20);//set alternate function for PA10
	//output type by default push pull
	//pull this pin high internally
	GPIOA->PUPDR |= (1 << 18);
	GPIOA->PUPDR |= (1 << 20);
	GPIOA->AFR[1] |= (7 << 4);//selecting uart1 as alternate functionality PA9
	GPIOA->AFR[1] |= (7 << 8);//selecting uart1 as alternate functionality PA10
}

void uart_init(void)
{
	uart_gpio_int();//for alternate functionality
	//1)enable uart 1 clock
	RCC->APB2ENR |= (1 << 4);
	//enable tx and rx
	USART1->CR1 |= (1 << 2)|(1 << 3);
	//word len 1 start bit 8 data bits n stop bits
	USART1->CR1 &= ~(1 << 12);
	//disable parity
	USART1->CR1 &= ~(1 << 10);
	//1 stop biyts
	USART1->CR2 &= ~(1 << 12);
	USART1->CR2 &= ~(1 << 13);
	//hardware flow control RTS & CTS
	USART1->CR3 &= ~(1 << 8);
	USART1->CR3 &= ~(1 << 9);

	//baurd rate
	//select OVER8 sampling
	USART1->CR1 |= (1 << 15);
	USART1->BRR = 3;//9600 buad for OVER8 = 1,Fscl = 16Mhz
	USART1->BRR |= 208 << 4;

	USART1->CR1 |= (1 << 13);//enable uart
}
void uart_send_byte(uint8_t tx_data)
{
	while(!(USART1->SR & (1 << 7)));//cheking data register empty
	USART1->DR = tx_data;
	while(!(USART1->SR & (1 << 6)));//cheking transmission is completed or not
}

void uart_interrupt_config(void)
{
	//enable tx register empty interrupt (TXEIE)
	USART1->CR1 |= (1 << 7);
	//enable rx rigister not empty interrupt (RXNEIE)
	USART1->CR1 |= (1 << 5);
}

void USART1_IRQHandler(void)
{
	uint8_t rx_data = 0;
	if((USART1->SR & (1 << 5)) && (NULL != rx_func_callBack))
	{
		rx_data = (uint8_t)USART1->DR;
		rx_func_callBack(SEGGER_USRT_PORT,rx_data);
	}
}
