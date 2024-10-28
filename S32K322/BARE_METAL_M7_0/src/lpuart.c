
#include "S32K322_LPUART.h"
#define UART  IP_LPUART_1
#define OSR   8
#define BAUD_RATE (115200)

void lpuart_init(void)
{
	//UART->PINCFG = ;//select input trigger
	UART->BAUD &= ~(1 << 31);// match address mode disable
	UART->BAUD &= ~(1 << 30);// match address mode disable
	UART->BAUD &= ~(1 << 29);// disable 10 bit mode
	UART->BAUD &= ~(1 << 23);// disable DMA
	UART->BAUD &= ~(1 << 21);// disable DMA
	UART->BAUD &= ~(1 << 18);// address match wakeup
	UART->BAUD &= ~(1 << 19);// address match wakeup
	UART->BAUD &= ~(1 << 17);// rising edge sampling
	UART->BAUD |= (1 << 16);// Resynchronization Disable
	UART->BAUD &= ~(1 << 15);// LIN Break Detect Interrupt disable
	UART->BAUD &= ~(1 << 14);//RX Input Active Edge Interrupt disable
	UART->BAUD &= ~(1 << 13);//one stop bit
	UART->BAUD |= (OSR << 24);
	UART->BAUD |= calculate_baudrate() & 0x1fff;
	//todo: need to select OSR & SBR
	UART->STAT = (1 << 29);//selecting MSB or LSB first
	UART->CTRL = (1 << 19);//enable transmitter 
	UART->CTRL = (1 << 18);//enable reciever
	UART->CTRL = (1 << 21);//reciever interrupt enable
	
	UART->CTRL &= ~(1 << 7);//select lpuart normal mode
	UART->CTRL &= ~(1 << 4);//select 8 data bits
	UART->CTRL &= ~(1 << 1);//disable parity
	
}

uint16_t calculate_baudrate(void)
{
	uint16_t SBR = 0;
	//if OSR is selected as 8, baudrate 115200, clk = 40Mhz
	//if osr less than or equal to seven meens need to enable both edge sample
	SBR = 40000000/(BAUD_RATE * (OSR + 1));
	
	return SBR;
}

void uart_tx(uint8_t data)
{
	if(UART->STAT & (1 << 23))
	{
		//if TX data regiter is empty 
		UART->DATA = data;
	}
}
