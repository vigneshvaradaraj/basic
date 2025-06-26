#include <stdint.h>
#include "stm32f429xx.h"

/*
Purpose : sending 1 byte of data through UART1 in some interval (893ms)
*/

#define PLL_M  4
#define PLL_N  72
#define PLL_P  2
/*#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif
*/
void config_clock()
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
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
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

void delay(void)
{
	int i,j;
	for(i=10;--i;)
	for(j=100000;j--;);
}

uint32_t GetSYSCLKFrequency(void) {
    uint32_t sysclk;

    // Determine the clock source used for SYSCLK
    switch ((RCC->CFGR & RCC_CFGR_SWS) >> RCC_CFGR_SWS_Pos) {
        case 0:  // HSI
            sysclk = 16000000;  // HSI frequency
            break;
        case 1:  // HSE
            sysclk = 8000000;  // HSE frequency (assume 8MHz)
            break;
        case 2:  // PLL
            // Read PLL source, multipliers, dividers to calculate PLL output frequency
            sysclk = 100;
            break;
        default:
            sysclk = 16000000;  // Default HSI frequency if unknown
            break;
    }
    return sysclk;
}

uint32_t GetHCLKFrequency(void) {
    uint32_t hclk, ahb_prescaler;
    hclk = GetSYSCLKFrequency();

    // Read AHB prescaler
    ahb_prescaler = (RCC->CFGR & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos;
    if (ahb_prescaler >= 0x08) {  // Prescaler is 2, 4, 8, ..., 512
        hclk >>= (ahb_prescaler - 0x07);  // Shift right to divide
    }
    return hclk;
}

uint32_t GetPCLK2Frequency(void)
{
    uint32_t pclk2, apb2_prescaler;
    pclk2 = GetHCLKFrequency();

    // Read APB2 prescaler
    apb2_prescaler = (RCC->CFGR & RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos;
    if (apb2_prescaler >= 0x04) {  // Prescaler is 2, 4, 8, or 16
        pclk2 >>= (apb2_prescaler - 0x03);  // Shift right to divide
    }
    return pclk2;
}

void uart_gpio_int()
{
	RCC->AHB1ENR |= (1 << 0);//enable clock for GPIOA
	GPIOA->MODER |= (2 << 18);//set alternate function for PA9
	GPIOA->MODER |= (2 << 20);//set alternate function for PA10
	//output type by default push pull
	//pull this pin high internally
	GPIOA->PUPDR |= (1 << 18);
	GPIOA->PUPDR |= (1 << 20);
	GPIOA->AFR[1] |= (7 << 4);//selecting uart1 as alternate functionality PA9
	GPIOA->AFR[1] |= (7 << 8);//selecting uart1 as alternate functionality PA10
}
void uart_init()
{
	uart_gpio_int();//for alternate functionality
	//1)enable uart 1 clock
	RCC->APB2ENR |= (1 << 4);
	//enable tx and rx
	USART1->CR1 |= (1 << 2)|(1 << 3);
	//word len 1 start bit 8 data bits n stop bits
	USART1->CR1 &= ~(1 << 12);
	//disable parity
	USART1->CR1 &= ~(1 << 10);
	//1 stop biyts
	USART1->CR2 &= ~(1 << 12);
	USART1->CR2 &= ~(1 << 13);
	//hardware flow control RTS & CTS
	USART1->CR3 &= ~(1 << 8);
	USART1->CR3 &= ~(1 << 9);

	//baurd rate
	//select OVER8 sampling
	USART1->CR1 |= (1 << 15);
	USART1->BRR = 3;//9600 buad for OVER8 = 1,Fscl = 16Mhz
	USART1->BRR |= 208 << 4;

	USART1->CR1 |= (1 << 13);//enable uart
}
void uart_send_byte(uint8_t tx_data)
{
	while(!(USART1->SR & (1 << 7)));//cheking data register empty
	USART1->DR = tx_data;
	while(!(USART1->SR & (1 << 6)));//cheking transmission is completed or not
}

void uart_send_string(uint8_t* str,uint16_t length)
{
	int i = 0;
	for(i = 0;i < length;i++)
	{
		uart_send_byte(str[i]);
	}
}

void uart_interrupt_config()
{
	//enable tx register empty interrupt (TXEIE)
	USART1->CR1 |= (1 << 7);
	//enable rx rigister not empty interrupt (RXNEIE)
	USART1->CR1 |= (1 << 5);
	//enable transmission completes 
	USART1->CR1 |= (1 << );
}
void uartIsr()//need to store this in vector table
{
	//checking the source of the interrupt
	if(USART1->SR & (1 << 7));//Transmit data register empty TXE
	{
		
	}
	else if(USART1->SR & (1 << 6));//current Transmission completes TC
	{
		
	}
	else if(USART1->SR & (1 << 5));//Recieve data register not empty RXNE
	{
		
	}
}

void stop_sending_opcodes(void)
{
	uint8_t flow_control_frame[] = {0x1,0x2,0x3};
	uart_send_string(flow_control_frame,sizeof(flow_control_frame));
}

void resume_sending_opcodes(void)
{
	uint8_t flow_control_frame[] = {0x2,0x2,0x3};
	uart_send_string(flow_control_frame,sizeof(flow_control_frame));
}

int main(void)
{
    /* Loop forever */
	config_clock();
	GetPCLK2Frequency();
	uart_init();
	uint8_t count = 0;
	for(;;)
	{
		delay();
		uart_send_byte(count++);
		delay();
	}
}
