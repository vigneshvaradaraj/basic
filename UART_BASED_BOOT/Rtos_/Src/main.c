/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include "task.h"
#include "portmacro.h"
#include "projdefs.h"
#include "stm32_f4xx.h"
#include "SEGGER_SYSVIEW.h"

void canTask(void* parm);
void flashTask(void* parm);

TaskHandle_t canTaskHandle;
TaskHandle_t flashTaskHandle;

uint32_t SystemCoreClock = 16000000;

void clock_init(void)
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

void gpio_init(void)
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

int taskInit(void)
{
	if(pdPASS != xTaskCreate(canTask,"canTask",128,NULL,2,&canTaskHandle))
	{
		return -1;
	}
	if(pdPASS != xTaskCreate(flashTask,"flashTask",128,NULL,2,&flashTaskHandle))
	{
		return -1;
	}

	return 0;
}
int main(void)
{
    /* Loop forever */
	clock_init();
	gpio_init();

	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();

	if(taskInit() == 0)
	{
		vTaskStartScheduler();
	}
	for(;;)
	{
		;
	}
}

void canTask(void* parm)
{
	while(1)
	{
		set();
		taskYIELD();
		//vTaskDelay(pdMS_TO_TICKS(500));
	}
}

void flashTask(void* parm)
{

	while(1)
	{
		set();
		vTaskDelay(pdMS_TO_TICKS(500));
		clear();
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
