/*
 * timer.h
 *
 *  Created on: Mar 17, 2025
 *      Author: vigneshv
 */

#ifndef TIMER_H_
#define TIMER_H_

void clock_init(void);
void set(void);
void clear(void);
void gpio_init(void);
void timer6_init(void);
uint16_t getCNTval(void);

#endif /* TIMER_H_ */
