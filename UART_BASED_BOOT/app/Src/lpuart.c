#include <stdint.h>
#include "stm32_f4xx.h"
#include "lpuart.h"
#include <stdbool.h>
#include "cm4.h"

#ifndef UART_RX_BUFFER_SIZE
#define UART_RX_BUFFER_SIZE (1024)
#endif

uint8_t uart_rx_buffer[UART_RX_BUFFER_SIZE] = {0};
uint8_t cmd_buffer[UART_CMD_BUFFER_SIZE] = {0};

int data_index = 0;
int cmd_index = 0;
bool data_ready_to_read = false;
bool cmd_ready_to_read = false;
bool cmd_mode = true;
bool opcode_mode = false;


/*
Purpose : sending 1 byte of data through UART1 in some interval (893ms)
*/

#define PLL_M  4
#define PLL_N  72
#define PLL_P  2
/*#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif
*/

void uart_gpio_int()
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
void uart_init()
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
	uart_interrupt_config();
	USART1->CR1 |= (1 << 13);//enable uart
}
void uart_send_byte(uint8_t tx_data)
{
	while(!(USART1->SR & (1 << 7)));//cheking data register empty
	USART1->DR = tx_data;
	while(!(USART1->SR & (1 << 6)));//cheking transmission is completed or not
}

void LPUART1_transmit_string(char str[])
{
	int i = 0;
	for(i = 0;str[i];i++)
	{
		uart_send_byte(str[i]);
	}
}

uint8_t LPUART1_receive_char(void)
{ /* Function to Receive single Char */
	 uint8_t receive;
	 receive = USART1->DR; /* Read received data*/
	 return receive;
}

void read(void)
{
	if(USART1->SR & (1 << 5))
	{
		while(0);
	}
}

void uart_interrupt_config()
{
	//enable rx rigister not empty interrupt (RXNEIE)
	USART1->CR1 |= (1 << 5);

	NVIC_EnableIRQ(USART1_IRQn);
}
void USART1_IRQHandler(void)//need to store this in vector table
{
	if(USART1->SR & (1 << 5))//Recieve data register not empty RXNE
	{
	    if(true == cmd_mode)
	    {
	        cmd_buffer[cmd_index++] = LPUART1_receive_char();

	        if(cmd_index >= 8)
	        {
	            cmd_index = 0;
	            cmd_ready_to_read = true;
	        }
	    }
	    else if(true == opcode_mode)
	    {
	        uart_rx_buffer[data_index++] = LPUART1_receive_char();
	        if(UART_RX_BUFFER_SIZE <= data_index)
	        {
	            cmd_mode = true;
	            opcode_mode = false;
	            data_index = 0;
	            data_ready_to_read = true;
	        }
	    }
	}
}
