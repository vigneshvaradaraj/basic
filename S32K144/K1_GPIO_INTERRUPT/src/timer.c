#include "timer.h"
#include "core_cm4.h"

#define PCS (24) //peripheral clock select
#define CGC (30) //clock gate control
#define LPIT_CLK_ENABLE (0)
#define LPIT_CHANNAL_ENABLE (0)

#define LPIT_CHANNAL0_INTERRUPT_STATUS (0)

void lpit_interrupt_init(IRQn_Type IRQn)
{
	NVIC->ICPR[(((uint32_t)(int32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
	NVIC->ISER[(((uint32_t)(int32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

void lpit_clock_init(void)
{
    //need to config for 1 MS
    //this clcok selection only happens when cgc = 0
    //means need to enable this before cgc = 1
	SCG->SOSCDIV |= (1 << 8);// SOSCDIV2 = SOSC(external clock)/1
    PCC->PCCn[PCC_LPIT_INDEX] |= (1 << PCS);//SOSCDIV2 selected for lpit
	//to lock the clock (no more clock modification)
	PCC->PCCn[PCC_LPIT_INDEX] |= (1 << CGC);
}

void lpit_init(void)
{
	//enable peripheral clock
	LPIT0->MCR |= (1 << LPIT_CLK_ENABLE);
	
	LPIT0->TMR[0].TVAL = 8000;//8 thousand for 1MS for input clock 8Mhz
	LPIT0->TMR[0].TCTRL = (1 << LPIT_CHANNAL_ENABLE);//enable channal0
}

void lpit_channal0_callback(void)
{
	PTD->PTOR = (1 << 0);
}

void LPIT0_Ch0_IRQHandler(void)
{
	if(LPIT0->MSR & (1 << LPIT_CHANNAL0_INTERRUPT_STATUS))
	{
		lpit_channal0_callback();
	}
}
