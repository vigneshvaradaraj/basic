#define HSEON  (1 << 16)
#define HSERDY (1 << 17)
#define PWREN  (1 << 28)
#define VOS_MAX_120MZ (1 << 14)|(1 << 15)
#define VOS_MAX_144MZ (1 << 14)&(~(1 << 15))
#define VOS_MAX_168MZ (~(1 << 14))&(1<<15)
void config_clock()
{
	//1)turn on the HSE
	RCC->CR |= HSEON;
	//wait for the HSE ready
	while(!(RCC->CR & HSERDY));
	//2)set the power enable clock and voltage regulator
	RCC->APB1ENR |= PWREN;
	
	//On STM32F42xxx and STM32F43xxx devices:
	//VOS[1:0] = '0x01', the maximum value of fHCLK is 120 MHz.
	//VOS[1:0] = '0x10', the maximum value of fHCLK is 144 MHz. It can be extended to
    //168 MHz by activating the over-drive mode.
	//VOS[1:0] = '0x11, the maximum value of fHCLK is 168 MHz. It can be extended to
    //180 MHz by activating the over-drive mode. 
	PWR->CR |= VOS_MAX_120MZ;
	//4)config the prescalars hclk, pclk1, pclk2
	//AHB PR
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	//APB1 PR
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
	//APB2 PR
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
	
}