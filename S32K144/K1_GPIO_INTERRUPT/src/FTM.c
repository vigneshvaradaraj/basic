//void FTM0_CH1_PWM_init(void)
//{
// FTM0->CONTROLS[1].CnSC = 0x00000028; /* FTM0 ch1: edge-aligned PWM, low true pulses */
// /* CHIE (Chan Interrupt Ena) = 0 (default) */
// /* MSB:MSA (chan Mode Select)=0b10, Edge Align PWM*/
// /* ELSB:ELSA (chan Edge/Level Select)=0b10, low true */
// FTM0->CONTROLS[1].CnV = 46875; /* FTM0 ch1 compare value (~75% duty cycle) */
//}
//
//void FTM0_init(void)
//{
// PCC->PCCn[PCC_FLEXTMR0_INDEX] &= ~PCC_PCCn_CGC_MASK; /* Ensure clk disabled for config */
// PCC->PCCn[PCC_FLEXTMR0_INDEX] |= PCC_PCCn_PCS(0b001)/* Clock Src=1, 8 MHz SOSCDIV1_CLK */
// | PCC_PCCn_CGC_MASK; /* Enable clock for FTM regs */
// FTM0->MODE |= FTM_MODE_WPDIS_MASK; /* Write protect to registers disabled (default) */
// FTM0->SC = 0x00030007; /* Enable PWM channel 0 output*/
//                        /* Enable PWM channel 1 output*/
//                        /* TOIE (Timer Overflow Interrupt Ena) = 0 (default) */
//                        /* CPWMS (Center aligned PWM Select) = 0 (default, up count) */
//                        /* CLKS (Clock source) = 0 (default, no clock; FTM disabled) */
//                        /* PS (Prescaler factor) = 7. Prescaler = 128 */
// FTM0->COMBINE = 0x00000000;/* FTM mode settings used: DECAPENx, MCOMBINEx, COMBINEx=0 */
// FTM0->POL = 0x00000000; /* Polarity for all channels is active high (default) */
// FTM0->MOD = 62500 -1 ; /* FTM1 counter final value (used for PWM mode) */
// /* FTM1 Period = MOD-CNTIN+0x0001 ~= 62500 ctr clks */
///* 8MHz /128 = 62.5kHz -> ticks -> 1Hz */
//}
