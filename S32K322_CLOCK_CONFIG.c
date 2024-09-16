/*
	s32k322 clock config...
*/

void config_clock(void)
{
	PLL->CR &= ~(1 << 31);//clear this bit for power up the PLL
	while(!(PLL->SR & (1 << 2)));//wait for PLL to acquere the clock
	
	PLL->DV |= (2 << 25);//input clcok (FXOSC) deviding by 2
	PLL->ODIV_0 |= (1 << 31);//enable PLL output devider
	PLL->ODIV_0 |= ();
}