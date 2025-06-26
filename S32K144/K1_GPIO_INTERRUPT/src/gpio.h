/*
 * gpio.h
 *
 *  Created on: 04-Nov-2024
 *      Author: vigneshv
 */

#include "S32K144.h"

#ifndef GPIO_H_
#define GPIO_H_

void NVIC_init_IRQs(IRQn_Type);
void PORT_init(void);
void WDOG_disable(void);
void gpio_interrupt_CB(void);

#endif /* GPIO_H_ */
