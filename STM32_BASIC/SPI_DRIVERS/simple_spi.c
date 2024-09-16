//SPI1
void spi_peripheral_clock_init()
{
	//enable clock for SPI1
	RCC->APB2ENR |= (1 << 12);
}
void spi_gpio_init()
{
	//enable the clock for GPIOA
	RCC->AHB1 |= (1 << 0);
	//todo:set AF of gpio
	//AF5
	//PA4-nss
	//PA5-clk
	//PA6-miso
	//PA7-mosi
	GPIOA->MODER |= (2 << 8);//alternate function for PA4
	GPIOA->MODER |= (2 << 10);//alternate function for PA5
	GPIOA->MODER |= (2 << 12);//alternate function for PA6
	GPIOA->MODER |= (2 << 14);//alternate function for PA7
	
	GPIOA->AFR[0] |= (5 << 16);//SPI selected as AF for PA4 
	GPIOA->AFR[0] |= (5 << 20);//SPI selected as AF for PA5
	GPIOA->AFR[0] |= (5 << 24);//SPI selected as AF for PA6
	GPIOA->AFR[0] |= (5 << 28);//SPI selected as AF for PA7
}
void spi_init()
{
	//1)config device as spi master, bydefault it is slave
	SPI1->CR1 |= (1 << 2);
	
	//2)bus config
	//15th bit makes bydefault 2line-bidirectional mode
	SPI1->BIDIMODE &= ~(1 << 15);
	
	//3)can set baud rate of the clock using CR1
	//bydefault baud rate is Fpclk/2 (bit 3 of cr1)
	`
	//4)confi data frame formate (bit 11 of cr1)
	//letting it be bydefault 8bit perframe
	
	//5)SPI mode selection CPOL,CPHA bit 0,1 of cr1 
	//for now letting it be bydefault mode 0
	
	
	//6) enable SPI 
	SPI1->CR1 |= (1 << 6);
}

void spi_send(uint8_t data)
{
	while(!(SPI1->SR & (1 << 1)));
	SPI1->DR = data;
}

uint8_t spi_recieve(void)
{
	while(!(SPI1->SR & (1 << 0)));
	return (uint8_t)SPI1->DR;
}

void spi_interrupt_config(void)
{
	//need to enable SPI1 IRQ number ITEVTEN
	
	//need to define interrupt priority
	
	//enable tx buffer empty interrupt
	SPI1->CR2 |= (1 << 7);
	//enable rx buffer not empty
	SPI1->CR2 |= (1 << 6);
	
	NVIC_EnableIRQ(SPI1_IRQn);//enable IRQ by using CMSIS-CM4 file
}

void SPI1_ISR(void)
{
	//analyse event take action according to that
	
}
/*SPI debugging steps if SPI peripheral not working
1) master mode bit must be enabled in the configuration register
if you are configuring the peripheral as master 
2) SPI peripheral enable bit must be enabled
3) SPI peripheral clock must be enabled.bydefault clocks to almost all the peripherals in the MC will be disabled to save power.

*/