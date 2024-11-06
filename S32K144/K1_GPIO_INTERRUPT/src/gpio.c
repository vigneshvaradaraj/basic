/*
 * gpio.c
 *
 *  Created on: 04-Nov-2024
 *      Author: vigneshv
 */

#include "S32K144.h" /* include peripheral declarations S32K144 */
#include "core_cm4.h"

int idle_counter = 0; /* main loop idle counter */
int lpit0_ch0_flag_counter = 0; /* LPIT0 chan 0 timeout counter */

void NVIC_init_IRQs(IRQn_Type IRQn)
{
	NVIC->ICPR[(((uint32_t)(int32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
	NVIC->ISER[(((uint32_t)(int32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}
void PORT_init (void) 
{
 PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock for PORT D */
 PCC->PCCn[PCC_PORTC_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock for PORT C */
 
 PTD->PDDR |= 1<<0; /* Port D0: Data Direction= output BLUE LED OUTPUT*/
 PORTD->PCR[0] = 0x00000100; /* Port D0: MUX = ALT1, GPIO (to blue LED on EVB) */
 PTC->PDDR &= ~(1 << 12); //PC12 as input
 PORTC->PCR[12] |= (1 << 8);/*config PC12 as gpio*/
 PORTC->PCR[12] |= (10 << 16);/* PC12 both edge can trigger interrupt*/
}

void WDOG_disable (void)
{
 WDOG->CNT=0xD928C520; /*Unlock watchdog*/
 WDOG->TOVAL=0x0000FFFF; /*Maximum timeout value*/
 WDOG->CS = 0x00002100; /*Disable watchdog*/
}

void PORTC_IRQHandler(void)
{
	PORTC->PCR[12] |= (1 << 24);/*clear the interupt status flag*/
	PTD->PTOR = (1 << 0);
}


