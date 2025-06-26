
#include "stm32_f429xx.h"
#include "cm4.h"
void clock_init(void)
{
	RCC->CR |= (1 << 0);//enable HSI
	while(!(RCC->CR & (1 << 1)));
	
	//bydefault HSI is selcted ad system clock
}

void set(void)
{
	GPIOG->BSRR |= (1 << 13);//set the PG13
}

void clear(void)
{
	GPIOG->BSRR |= (1 << 29);//clear the PG13
}

void gpio_init(void)
{
	RCC->AHB1ENR |= (1 << 6);//enable clock for GPIOG
	
	GPIOG->MODER |= (1 << 26);//make the PG13 as output
}

void timer6_init(void)
{
	/*need to set the timer6 for 1 sec interrupt*/
	//step 1: enable clock for timer6
	RCC->APB1ENR |= (1 << 4);//enable clock for TIMER6
	
	//for apb1 timer clock just dived the 16Mz by 16
	//RCC->CFGR |= (7 << 10);
	
	//cnt clock is 2 mhz it needs to be devided by 40000 to make the timer clock as 50Hz
	TIM6->PSC = 16000 - 1;
	
	//store 50 to auto reload preload register to generate event for every one sec
	TIM6->ARR = 1000 - 1;
	
//	TIM6->CR1 = 0;
//	TIM6->CR1 |= (1 << 7);
//	TIM6->CR1 |= (1 << 0);
//	TIM6->CR1 |= (1 << 2);
	
	NVIC_EnableIRQ(TIM6_DAC_IRQn);

	TIM6->CR1 |= (1 << 0);
	//enable interrupt 
	TIM6->DIER |= (1 <<0);
}

uint16_t getCNTval(void)
{
	return TIM6->CNT;
}
