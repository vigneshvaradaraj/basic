/*
 * main implementation: use this 'C' sample to create your own application
 *
 */
#include "S32K144.h"
#include "core_cm4.h"

void clock_pll_init(void)
{
	//select SOSC as input clock to the Micro controller
	SCG->SOSCCFG |= (1 << 2);// need to check this bit

	/* Range=2: Medium freq (SOSC between 1MHz-8MHz)*/
	SCG->SOSCCFG |= (1 << 5);//select input clock range bw (1Mhz - 8Mhz)

	//select the pll source as SOSC
	SCG->SPLLCFG |= (1 << 0);

	//set PREDIV to 1, pll_input_clk = SOSC/2 = 8/2 = 4Mhz //(8Mhz min - 40Mhz max) need to ask
	SCG->SPLLCFG |= (1 << 8);

	//multiply the input 4 Mhz by 45 to make clock as 180 Mhz
	SCG->SPLLCFG |= (0x1D << 16);

	//bydefault the PLL output clock is devided by 2
	//SPLL_CLK = 90Mhz

	//select SPLL_CLK as system clock in run mode
	SCG->RCCR |= (6 << 24);

	//set the division factor for core clock and system clock core_clk = pll_clock/1 = 90Mhz /2 = 45Mhz (80Mhz max)
	SCG->RCCR |= (1 << 16);

	//bus_clk = core_clk / 2 = 45Mhz / 1 = 45Mhz (48Mhz max)
	SCG->RCCR |= (0 << 4);

	//flash_clk = core_clk / 6 = 45Mhz / 5 = 9Mhz (26.67Mhz max)
	SCG->RCCR |= (5 << 0);

	//SPLLDIV1_CLK = PLL_CLK / 2 = 90Mhz / 2 = 45Mhz
	SCG->SPLLDIV |= (2 << 0);

	//SPLLDIV2_CLK = PLL_CLK / 4 = 90Mhz / 4 = 22.5Mhz
	SCG->SPLLDIV |= (3 << 8);

	//SOSCDIV2 = SOSC/2 = 8Mhz/2 = 4Mhz
	SCG->SOSCDIV |= (2 << 8);
}

void gpioInit(void)
{
	//enable clock for PORTD
	PCC->PCCn[PCC_PORTD_INDEX] |= (1 << 30);

	//select the PD0 as output pin
	PTD->PDDR |= (1 << 0);

	//select PD0 as GPIO from mux
	PORTD->PCR[0] |= (1 << 8);
}

void pitChannelInit(void)
{
	//select SOSCDIV2_CLK for LPIT peripheral
	PCC->PCCn[PCC_LPIT_INDEX] |= (1 << 24);

	//enable clock for LPIT
	PCC->PCCn[PCC_LPIT_INDEX] |= (1 << 30);

	//enable clock for LPIT0 module
	LPIT0->MCR = 1;

	//enable channel0 interrupt
	LPIT0->MIER = 1;

	//load time period value to the timer value register
	LPIT0->TMR[0].TVAL = 4000000;

	//enable the timer channel 0
	LPIT0->TMR[0].TCTRL |= (1 << 0);
}

void initNVICforPit(void)
{
	//LPIT0_Ch0_IRQn
	NVIC->ISER[(((uint32_t)(int32_t)LPIT0_Ch0_IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)(int32_t)LPIT0_Ch0_IRQn) & 0x1FUL));
	//NVIC->ISPR[(((uint32_t)(int32_t)LPIT0_Ch0_IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)(int32_t)LPIT0_Ch0_IRQn) & 0x1FUL));
}
void toggle(void)
{
	//toggle PD0
	PTD->PTOR |= (1 << 0);
}

int main(void)
{
	clock_pll_init();
	gpioInit();
	pitChannelInit();
	initNVICforPit();
	while(1)
	{
		;
	}

	return 0;
}

void LPIT0_Ch0_IRQHandler(void)
{
	toggle();
}
