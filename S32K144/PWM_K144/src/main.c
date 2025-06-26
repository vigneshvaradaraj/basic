/*
 * main implementation: use this 'C' sample to create your own application
 *
 */
#include "S32K144.h" /* include peripheral declarations S32K144 */
#include "cm4.h"
int idle_counter = 0; /* main loop idle counter */
int lpit0_ch0_flag_counter = 0; /* LPIT0 chan 0 timeout counter */

void SOSC_init_8MHz(void)
{
 SCG->SOSCDIV=0x00000101; /* SOSCDIV1 & SOSCDIV2 =1: divide by 1 */
 SCG->SOSCCFG=0x00000024; /* Range=2: Medium freq (SOSC between 1MHz-8MHz)*/
 /* HGO=0: Config xtal osc for low power */
/* EREFS=1: Input is external XTAL */
 while(SCG->SOSCCSR & SCG_SOSCCSR_LK_MASK); /* Ensure SOSCCSR unlocked */
 SCG->SOSCCSR=0x00000001; /* LK=0: SOSCCSR can be written */
 /* SOSCCMRE=0: OSC CLK monitor IRQ if enabled */
/* SOSCCM=0: OSC CLK monitor disabled */
/* SOSCERCLKEN=0: Sys OSC 3V ERCLK output clk disabled */
/* SOSCLPEN=0: Sys OSC disabled in VLP modes */
/* SOSCSTEN=0: Sys OSC disabled in Stop modes */
/* SOSCEN=1: Enable oscillator */
 while(!(SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK)); /* Wait for sys OSC clk valid */
}
void SPLL_init_160MHz(void)
{
 while(SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK); /* Ensure SPLLCSR unlocked */
 SCG->SPLLCSR = 0x00000000; /* SPLLEN=0: SPLL is disabled (default) */
 SCG->SPLLDIV = 0x00000302; /* SPLLDIV1 divide by 2; SPLLDIV2 divide by 4 */
 SCG->SPLLCFG = 0x00180000; /* PREDIV=0: Divide SOSC_CLK by 0+1=1 */
 /* MULT=24: Multiply sys pll by 4+24=40 */
/* SPLL_CLK = 8MHz / 1 * 40 / 2 = 160 MHz */
 while(SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK); /* Ensure SPLLCSR unlocked */
 SCG->SPLLCSR = 0x00000001; /* LK=0: SPLLCSR can be written */
 /* SPLLCMRE=0: SPLL CLK monitor IRQ if enabled */
/* SPLLCM=0: SPLL CLK monitor disabled */
/* SPLLSTEN=0: SPLL disabled in Stop modes */
/* SPLLEN=1: Enable SPLL */
 while(!(SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK)); /* Wait for SPLL valid */
}
void NormalRUNmode_80MHz (void)
{ /* Change to normal RUN mode with 8MHz SOSC, 80 MHz PLL*/
 SCG->RCCR=SCG_RCCR_SCS(6) /* PLL as clock source*/
 |SCG_RCCR_DIVCORE(0b01) /* DIVCORE=1, div. by 2: Core clock = 160/2 MHz = 80 MHz*/
 |SCG_RCCR_DIVBUS(0b01) /* DIVBUS=1, div. by 2: bus clock = 40 MHz*/
 |SCG_RCCR_DIVSLOW(0b10); /* DIVSLOW=2, div. by 3: SCG slow, flash clock= 26 2/3 MHz*/
 while (((SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT ) != 6) {}
 /* Wait for sys clk src = SPLL */
}

void NVIC_init_IRQs (void) {
 NVIC->ICPR[1] = 1 << (48 % 32); /* IRQ48-LPIT0 ch0: clr any pending IRQ*/
 NVIC->ISER[1] = 1 << (48 % 32); /* IRQ48-LPIT0 ch0: enable IRQ */
 NVIC->IP[48] =0x0A; /* IRQ48-LPIT0 ch0: priority 10 of 0-15*/
}
void PORT_init (void)
{
 PCC-> PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock for PORT D */
 PTD->PDDR |= 1<<4; /* Port D0: Data Direction= output */
 PORTD->PCR[4] = 0x00000100; /* Port D0: MUX = ALT1, GPIO (to blue LED on EVB) */

 PCC->PCCn[PCC_PORTE_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clock for PORTE */
 PORTE->PCR[4] |= PORT_PCR_MUX(5); /* Port E4: MUX = ALT5, CAN0_RX */
 PORTE->PCR[5] |= PORT_PCR_MUX(5); /* Port E5: MUX = ALT5, CAN0_TX */
 PCC->PCCn[PCC_PORTD_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTD */
 PORTD->PCR[16] = 0x00000100; /* Port D16: MUX = GPIO (to green LED) */
 PTD->PDDR |= 1<<16;
}
void LPIT0_init (void)
{
 PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6); /* Clock Src = 6 (SPLL2_DIV2_CLK)*/
 PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clk to LPIT0 regs */
 LPIT0->MCR = 0x00000001; /* DBG_EN-0: Timer chans stop in Debug mode */
 /* DOZE_EN=0: Timer chans are stopped in DOZE mode */
/* SW_RST=0: SW reset does not reset timer chans, regs */
 /* M_CEN=1: enable module clk (allow writing other LPIT0 regs) */
 LPIT0->MIER = 0x00000001; /* TIE0=1: Timer Interrupt Enabled fot Chan 0 */
 LPIT0->TMR[0].TVAL = 80000000; /* Chan 0 Timeout period: 80M clocks */
 LPIT0->TMR[0].TCTRL = 0x00000001; /* T_EN=1: Timer channel is enabled */
 /* CHAIN=0: channel chaining is disabled */
/* MODE=0: 32 periodic counter mode */
/* TSOT=0: Timer decrements immediately based on restart */
/* TSOI=0: Timer does not stop after timeout */
/* TROT=0 Timer will not reload on trigger */
/* TRG_SRC=0: External trigger source */
/* TRG_SEL=0: Timer chan 0 trigger source is selected*/
}
void WDOG_disable (void)
{
 WDOG->CNT=0xD928C520; /*Unlock watchdog*/
 WDOG->TOVAL=0x0000FFFF; /*Maximum timeout value*/
 WDOG->CS = 0x00002100; /*Disable watchdog*/
}

int main(void)
{
	uint32_t regValue = 0;
	WDOG_disable();
	SOSC_init_8MHz(); /* Initialize system oscillator for 8 MHz xtal */
	SPLL_init_160MHz(); /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	NormalRUNmode_80MHz(); /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
	PORT_init(); /* Configure ports */
	NVIC_init_IRQs(); /* Enable desired interrupts and priorities */
	LPIT0_init(); /* Initialize PIT0 for 1 second timeout */
	regValue = SCG->PARAM;
	idle_counter++;
	for (;;)
	{
		idle_counter++;
	}
}
void LPIT0_Ch0_IRQHandler (void)
{
	LPIT0->MSR |= LPIT_MSR_TIF0_MASK; /* Clear LPIT0 timer flag 0 */
	/* Perform read-after-write to ensure flag clears before ISR exit */
	lpit0_ch0_flag_counter++; /* Increment LPIT0 timeout counter */
	//PTD->PTOR |= 1<<16;
	PTD->PTOR |= 1<<4; /* Toggle output on port D0 (blue LED) */
}
