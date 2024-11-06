/*
 * main implementation: use this 'C' sample to create your own application
 *
 */
#include "S32K144.h"
#include "gpio.h"

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

void clock_init(void)
{
	SCG->SOSCCFG |= (1 << 2);//select the internal 8Mhz OSC as SOSC
	//need to select run mode from power management controller (PMC)
	SCG->RCCR |= (1 << 24);//select the SOSC as system clock
	SCG->RCCR |= (1 << 16);//core clock = SOSC/2
	SCG->RCCR |= (1 << 4);//bus clock = SOSC/4
	
	SMC->PMCTRL &= ~(0 << 5);//selecting run mode 
	SMC->PMCTRL &= ~(0 << 6);
}


int main(void)
{
	clock_init();
	WDOG_disable();
	PORT_init(); /* Configure ports */
	NVIC_init_IRQs(PORTC_IRQn); /* Enable desired interrupts and priorities */
	for (;;)
	{
	  ;
	}
}
