/*
GPIO pin interrupt configuration

1.pin must me in input configuration
2.config the edge trigger (RT,FT,RFT)
3.enable interrupt delivery from peripheral to the processor 
(on peripheral side)
4.identify the IRQ number on witch the processor accepts the 
interrupt from that pin
5.configure the IRQ priority for the identified IRQ number
(processor side)
6.enable interrupt reception on that IRQ number (processor side)
7.Implement IRQ handler
*/

void exti_interrupt_config(bool PIN_NUM)
{
	//1)
	//enable falling edge trigger
	EXTI->FTSR |= (1 << PIN_NUM);
	//disable rising edge trigger
	EXTI->RTSR &= ~(1 << PIN_NUM);
	//we can set both also
	//2) config the port selection in SYSCFG_EXTICR
	//enable clock for SYSCFG peripheral
	RCC->APB2EN |= (1 << 14);
	//SYSCFG_EXTICR this register is going to decide 
	//extipins of which port is going to trigger the interrupt
	//there are 4 configuration register you can config 4 pins in one register
	//ex:
	SYSCFG_EXTICR1 |= (2 << 0);//PORTC 0 th pin is selected to trigger interrupt
	//config priority if needed
	//3) enable the EXTI interrupt delivery using IMR
	EXTI->IMR |= (1 << PIN_NUM);
}

void irq_hnadler(uint8_t pinNumber)
{
	if(EXTI->PR & (1 << pinNumber))
	{
		//celar the pending register
		EXTI->PR |= (1 << pinNumber);
	}
}