/*
 * main implementation: use this 'C' sample to create your own application
 *
 */
#include "lpuart.h"
#include <string.h>
#include "boot.h"
#include "stm32_f4xx.h"
#include "flash.h"
#include "can.h"

#ifndef UART_BOOT
#define CAN_BOOT
#endif 

#if defined (__ghs__)
    #define __INTERRUPT_SVC  __interrupt
    #define __NO_RETURN _Pragma("ghs nowarning 111")
#elif defined (__ICCARM__)
    #define __INTERRUPT_SVC  __svc
    #define __NO_RETURN _Pragma("diag_suppress=Pe111")
#elif defined (__GNUC__)
    #define __INTERRUPT_SVC  __attribute__ ((interrupt ("SVC")))
    #define __NO_RETURN
#else
    #define __INTERRUPT_SVC
    #define __NO_RETURN
#endif

#define SOSC (8000000) //sosc 8 Mhz external clock

#define GO_TO_APP_CMD "Goto_app"
#define REQUEST_TO_SEND_OPCODE "send_cod"
#define HOLD_THE_SENDING "hold"
#define HOST_READY "Host_rdy"
#define OPCODE_MODE "opcode"

extern uint16_t receved_message_len,total_message_legth;
extern uint8_t opcode_buffer[1024];
uint16_t memory_segment_track = 0;

#define MATCH (0)

void clock_init()
{
	//1)trurn on the HSI
	RCC->CR |= RCC_CR_HSION;
	//wait for HSI ready
	while(!(RCC->CR & RCC_CR_HSIRDY));
	//2)set the power enable clock and voltage regulator
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_VOS;

	//3)config the flash prefetch and latancy related settings
	//FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN |FLASH_ACR_LATENCY_5WS;
	//4)config the prescalars hclk, pclk1, pclk2
	//AHB PR
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	//APB1 PR
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
	//APB2 PR
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
	/*
	//5) config the main pll
	RCC->PLLCFGR = (PLL_M << 0) | (PLL_N << 6) | (PLL_P << 16) | (RCC_PLLCFGR_PLLSRC_HSE);
    //6)enable PLL and wait for it to become ready
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));
	//7) select the clock source and wait for it to be set
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
	*/
}

void gpio_init()
{
	//1) ENABLE GPIOG CLK
	RCC->AHB1ENR |= (1 << 6);
	//2) SET PIN AS OUTPUT
	GPIOG->MODER |= (1 << 26);
	//3) CONFIG THE OP MODE
	GPIOG->OTYPER = 0;
	GPIOG->OSPEEDR = 0;
}

void set(void)
{
	GPIOG->BSRR |= (1<<13);//SET THE PIN
}

void clear(void)
{
	GPIOG->BSRR |= ((1<<13)<<16);//REST PIN
}

void delay(void)
{
	int i,j;
	for(i=10;--i;)
	for(j=100000;j--;);
}



int main(void)
{
	uint8_t dat[4] = {1,2,3,4};
	clock_init();
	uart_init();
	gpio_init();
	can_init();
	//Flash_WriteData(0x8100000,dat,4);

	for(int i = 0;0;i++)
	{
//		set();
//		delay();
//		clear();
//		delay();
		LPUART1_transmit_string(REQUEST_TO_SEND_OPCODE);
	}
//	goto_app();

	for (;;)
	{
#ifdef UART_BOOT
		if(cmd_ready_to_read == true)
		{
			if(strncmp(GO_TO_APP_CMD,cmd_buffer,strlen(GO_TO_APP_CMD)) == MATCH)
			{
				goto_app();
			}
			else if(strncmp(HOST_READY,cmd_buffer,strlen(HOST_READY)) == MATCH)
			{
				LPUART1_transmit_string(REQUEST_TO_SEND_OPCODE);
				cmd_mode = false;
				opcode_mode = true;
			}
			else if(strncmp(OPCODE_MODE,cmd_buffer,strlen(OPCODE_MODE)) == MATCH)
			{
				cmd_mode = false;
				opcode_mode = true;
			}
			cmd_ready_to_read = false;
		}
		else if(data_ready_to_read == true)
		{
			write_opcodes_to_flash(2048);
		}
#endif

#ifdef CAN_BOOT
		if(receved_message_len == opcode_buffer)
		{
			Flash_WriteData(APP_START_ADDRESS + memory_segment_track,opcode_buffer,opcode_buffer);
			memory_segment_track += APP_START_ADDRESS;
			receved_message_len = 0;
		}
		can_send(0x12345600,dat,4);
		delay();
#endif
	}
}
