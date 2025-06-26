/*
 * main implementation: use this 'C' sample to create your own application
 *
 */
#include "gpio.h"
#include "timer.h"
#include "lpuart.h"
#include <string.h>
#include "boot.h"

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

#define GO_TO_APP_CMD "goto_app"
#define REQUEST_TO_SEND_OPCODE "send_code"
#define HOLD_THE_SENDING "hold"
#define HOST_READY "Host_ready"
#define OPCODE_MODE "opcode"

void clock_init(void)
{
	SCG->SOSCCFG |= (1 << 2);//select the internal 8Mhz OSC as SOSC
	//need to select run mode from power management controller (PMC)
	SCG->RCCR |= (1 << 24);//select the SOSC as system clock
	SCG->RCCR |= (1 << 16);//core clock = SOSC/2
	SCG->RCCR |= (1 << 4);//bus clock = SOSC/4
	
	SMC->PMCTRL &= ~(0 << 5);//selecting run mode 
	SMC->PMCTRL &= ~(0 << 6);

	SCG->SOSCDIV |= (4 << 8);//if SOSC is 8Mhz then LPIT clk = 1Mhz
}


int main(void)
{
	clock_init();
	WDOG_disable();
	PORT_init(); /* Configure ports */
	NVIC_init_IRQs(PORTC_IRQn); /* Enable desired interrupts and priorities */
	lpit_clock_init();
	lpit_init();
	lpit_interrupt_init(LPIT0_Ch0_IRQn);
	for (;;)
	{
		if(cmd_ready_to_read == true)
		{
			if(strcmp(GO_TO_APP_CMD,cmd_buffer) == true)
			{
				goto_app();
			}
			else if(strcmp(HOST_READY,cmd_buffer) == true)
			{
				LPUART1_transmit_string(REQUEST_TO_SEND_OPCODE);
				cmd_mode = false;
				opcode_mode = true;
			}
			else if(strcmp(OPCODE_MODE,cmd_buffer) == true)
			{
				cmd_mode = false;
				opcode_mode = true;
			}
		}
		else if(data_ready_to_read == true)
		{
			write_opcodes_to_flash(1024);
		}
	}
}

void HardFault_Handler(void)
{
	while(1);
}
