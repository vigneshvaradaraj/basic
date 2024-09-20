/*
	s32k322 clock config...
*/

void config_clock(void)
{
	//setting crystal mode for FXOSC 
	FXOSC->CTRL |= (1 << 0);//enable FXOSC (external clock)
	FXOSC->CTRL &= ~(1 << 31);//internal oscillator not bypassed
	FXOSC->CTRL |= (1 << 24);//enable comparitor
	//need to check 
	FXOSC->CTRL |= (0x0C << 4);//GM_SEL
	FXOSC->CTRL |= (0x31 << 16);//EOCV
	while(!(FXOSC->STAT & (1 << 31)));//checking the external osc is stable or not 
	
	//1) Confirm that PLLODIV_n[DE] is 0 for all dividers
	PLL->PLLODIV[0] &= ~(1 << 31);//DE
	PLL->PLLODIV[1] &= ~(1 << 31);//DE
	//2). Confirm that PLLCR[PLLPD] is 1. 
	while(!(PLL->CR & (1 << 31)));//confirm that before intitializing PLL should be powered down
	//3)program plldv,pllfd,pllfm
	PLL->PLLDV |= (40U << 0);//multipling FXOSC by 40 (MFI) 
	PLL->PLLDV |= (1 << 12);//deviding FXOSC by 1 (RDIV)
	PLL->PLLDV |= (2 << 25);//after multipling by 40 then devide by 2(ODIV2)
	
	PLL->PLLFM = 0x0;
	PLL->PLLFM |= (1 <<30);//SSCGBYPASS
	
	PLL->PLLFD = 0x0;
	//4)Program PLLDV[ODIV2] and PLLODIV_n[DIV] to the desired values
	PLL->PLLODIV[0] = (2 << 16);//devides the 480/2+1 to produce PLL PHI0 = 160
	PLL->PLLODIV[1] = (2 << 16);//devides the 480/2+1 to produce PLL PHI1 = 160
	//5)Wait for the PLL reference clock to be stable
	while(!(FXOSC->STAT & (1 << 31)));//checking the external osc is stable or not 
	//6). Write 0 to PLLCR[PLLPD].
	PLL->CR &= ~(1 << 31);//power up the pll
	//7) Wait for PLLSR[LOCK] to be 1
	while(!(PLL->PLLSR & (1 << 2)));//checking pll acquired the clock or not
	//8) Write 1 to PLLODIV_n[DE].
	PLL->PLLODIV[0] = (1 << 31);//DE
	PLL->PLLODIV[1] = (1 << 31);//DE
	
	//MUX_0_CSC & MUX_0_CSS
	MC_CGM->MUX_0_CSC = 0;
	MC_CGM->MUX_0_CSC |= (0x8 << 24);//selecting PLL_PHI0_CLK
	while(!(((MC_CGM->MUX_0_CSS >> 17) & 0x7) == 1));//SWTRG Switch after request succeeded
	while(!(((MC_CGM->MUX_0_CSS >> 24) & 0xf) == 0x8));//PLL_PHI0_CLK clock has selected to perpheral buses
	
	//need to see PCFS(1242441) & ramp_up , ramp_down
	MC_CGM->MUX_0_DC_0 |= ((1-1) << 16);//core_clk devided by 1
	MC_CGM->MUX_0_DC_0 |= (1 << 31);//enable the devider
	
	MC_CGM->MUX_0_DC_1 |= ((2-1) << 16);//aipsFlatClk devided by 2
	MC_CGM->MUX_0_DC_1 |= (1 << 31);//enable the devider
	
	MC_CGM->MUX_0_DC_2 |= ((4-1) << 16);//aips_slow_clk devided by 4
	MC_CGM->MUX_0_DC_2 |= (1 << 31);//enable the devider
	
	MC_CGM->MUX_0_DC_3 |= ((2-1) << 16);//hse_clk devided by 2
	MC_CGM->MUX_0_DC_3 |= (1 << 31);//enable the devider
	
	MC_CGM->MUX_0_DC_4 |= ((4-1) << 16);//dcm_clk devided by 4
	MC_CGM->MUX_0_DC_4 |= (1 << 31);//enable the devider
	
	MC_CGM->MUX_0_DC_5 |= ((4-1) << 16);//lbist_clk devided by 4
	MC_CGM->MUX_0_DC_5 |= (1 << 31);//enable the devider
	
	MC_CGM->MUX_0_DC_6 |= ((1-1) << 16);//Qspi_mem_clk devided by 1
	MC_CGM->MUX_0_DC_6 |= (1 << 31);//enable the devider
}