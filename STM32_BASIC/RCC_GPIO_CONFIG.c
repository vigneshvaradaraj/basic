/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f429xx.h"

#define PLL_M  4
#define PLL_N  180
#define PLL_P  0
/*#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif
*/
void config_clock()
{
	//1)trurn on the HSE
	RCC->CR |= RCC_CR_HSEON;
	//wait for HSE ready
	while(!(RCC->CR & RCC_CR_HSERDY));
	//2)set the power enable clock and voltage regulator
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_VOS;

	//3)config the flash prefetch and latancy related settings
	FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN |FLASH_ACR_LATENCY_5WS;
	//4)config the prescalars hclk, pclk1, pclk2
	//AHB PR
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	//APB1 PR
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
	//APB2 PR
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
	//5) config the main pll
	RCC->PLLCFGR = (PLL_M << 0) | (PLL_N << 6) | (PLL_P << 16) | (RCC_PLLCFGR_PLLSRC_HSE);
    //6)enable PLL and wait for it to become ready
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));
	//7) select the clock source and wait for it to be set
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
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
	for(i=50;--i;)
	for(j=100000;j--;);
}

int main(void)
{
    /* Loop forever */
	config_clock();
	gpio_init();//for output pin

	for(;;)
	{
		set();
		delay();
		clear();
		delay();
	}
}
