#define PCTIM0                0x00000002
#define PCTIM1                0x00000004
#define PCUART0               0x00000008
#define PCUART1               0x00000010
#define PCPWM1                0x00000040
#define PCI2C0                0x00000080
#define PCSPI                 0x00000100
#define PCRTC                 0x00000200
#define PCSSP1                0x00000400
#define PCAD                  0x00001000
#define PCCAN1                0x00002000
#define PCCAN2                0x00004000
#define PCGPIO                0x00008000
#define PCRIT                 0x00010000
#define PCMC                  0x00020000
#define PCQEI                 0x00040000
#define PCI2C1                0x00080000
#define PCSSP0                0x00200000
#define PCTIM2                0x00400000
#define PCTIM3                0x00800000
#define PCUART2               0x01000000
#define PCUART3               0x02000000
#define PCI2C2                0x04000000
#define PCI2S                 0x08000000
#define PCGPDMA               0x20000000
#define PCENET                0x40000000
#define PCUSB                 0x80000000
//main system clk config
void SystemInit (void)
{
//note: refer Fig 7. Clock generation for the LPC17xx in reference manual

  //main oscillator enable (it will select the extal1 &  extal2)
  LPC_SC->SCS = (1 << 5);
  //monitor the main oscillator enable status flag
  while ((LPC_SC->SCS & (1<<6)) == 0);/* Wait for Oscillator to be ready    */
  //devide the clk with respect to howmuch clk you are going to give to cpu
  LPC_SC->CCLKCFG   = (1 << 2);      //main clk is divided by 2 to produce the clk to cpu
  //selecting main oscillator clk to PLL0 
  LPC_SC->CLKSRCSEL = (1 << 0);    /* Select Clock Source for PLL0*/
  //selecting M(multiplier) and N(devider) value for FCCO = (2 × M × FIN) / N
  //FIN - input to PLL0 // external clk that we r connecting to MicoController
  //FCCO - output from PLL0
  /* F_cco0 = (2 * M * F_in) / N  */
  //F_cco0 - system clk
  //__M = 25
  //__N = 1
  LPC_SC->PLL0CFG = 0x00010018;  //need to see selction?
  //after configuring PLL0CFG register we need to feed this 0xAA and 0x55 value to PLL0FEED in same order
  //this to apply the config value to PLL0CFG register
  LPC_SC->PLL0FEED  = 0xAA;
  LPC_SC->PLL0FEED  = 0x55;

  //PLL0 Enable
  LPC_SC->PLL0CON   = 0x01;             /* PLL0 Enable                        */
  //after configuring PLL0CON register we need to feed this 0xAA and 0x55 value to PLL0FEED in same order
  //this to apply the config value to PLL0CON register
  LPC_SC->PLL0FEED  = 0xAA;
  LPC_SC->PLL0FEED  = 0x55;
  
  //Reflects the PLL0 Lock status
  //When one, PLL0 is locked onto the requested frequency
  while (!(LPC_SC->PLL0STAT & (1<<26)));/* Wait for PLOCK0                    */

  //PLL0 Connect. Setting PLLC0 to one after PLL0 has been enabled and
  //locked, then followed by a valid PLL0 feed sequence causes PLL0 to
  //become the clock source for the CPU
  //(1 << 0) PLLE0 - enable PLL0
  //(1 << 1) PLLC0 - connect PLL0
  LPC_SC->PLL0CON   = (1 << 0) | (1 << 1);             /* PLL0 Enable & Connect              */
  //after configuring PLL0CON register we need to feed this 0xAA and 0x55 value to PLL0FEED in same order
  //this to apply the config value to PLL0CON register
  LPC_SC->PLL0FEED  = 0xAA;
  LPC_SC->PLL0FEED  = 0x55;
  //(1<<24) - PLLE0_STAT status of PLLE0 bit like is PLL0 enabled or not? 
  //(1<<25) - PLLC0_STAT status of PLLC0 bit like is PLL0 connected or not?
  while (!(LPC_SC->PLL0STAT & ((1<<25) | (1<<24))));/* Wait for PLLC0_STAT & PLLE0_STAT */

  // peripheral clock selection

  //All Pclk = Cclk / 4
  //Table 40. Peripheral Clock Selection register 0 (PCLKSEL0 - address 0x400F C1A8) bit
  //description refer in reference manval
  LPC_SC->PCLKSEL0  = 0x00000000;     /* Peripheral Clock Selection         */
  LPC_SC->PCLKSEL1  = 0x00000000;

  //selecting and unselect the peripheral clk by setting and resetting this bit to save power
  //here selected clk for can1,uart0,gpio
  LPC_SC->PCONP     = PCCAN1 | PCUART0 | PCGPIO;        /* Power Control for Peripherals      */
  
  //it is for selcting clk for CLKOUT pin (clk output) 
  LPC_SC->CLKOUTCFG = 0x00;    /* Clock Output Configuration         */
}

void CAN_Init(LPC_CAN_TypeDef *CANx, uint32_t baudrate)
{
	uint32_t temp;
	uint16_t i;
	CHECK_PARAM(PARAM_CANx(CANx));

	if(CANx == LPC_CAN1)
	{
		/* Turn on power and clock for CAN1 */
		CLKPWR_ConfigPPWR(CLKPWR_PCONP_PCAN1, ENABLE);
		/* Set clock divide for CAN1 */
	}
	else
	{
		/* Turn on power and clock for CAN2 */
		CLKPWR_ConfigPPWR(CLKPWR_PCONP_PCAN2, ENABLE);
		/* Set clock divide for CAN2 */
	}
	//clk divition by 2
	CLKPWR_SetPCLKDiv (CLKPWR_PCLKSEL_CAN1, CLKPWR_PCLKSEL_CCLK_DIV_2);
	CLKPWR_SetPCLKDiv (CLKPWR_PCLKSEL_CAN2, CLKPWR_PCLKSEL_CCLK_DIV_2);
	CLKPWR_SetPCLKDiv (CLKPWR_PCLKSEL_ACF, CLKPWR_PCLKSEL_CCLK_DIV_2);

	/*0 (normal) The CAN Controller is in the Operating Mode, and certain registers can not
	be written.
	1 (reset) CAN operation is disabled, writable registers can be written and the current
	transmission/reception of a message is aborted.*/
	//all the can config register we can access now
	CANx->MOD = 1; // Enter Reset Mode //CAN1MOD

	CANx->IER = 0; // Disable All CAN Interrupts //interrupt enable register//CAN1IER
	CANx->GSR = 0; // global status register
	/* Request command to release Rx, Tx buffer and clear data overrun */
	//CANx->CMR = CAN_CMR_AT | CAN_CMR_RRB | CAN_CMR_CDO;
	//CAN_CMR_AT - (1<<1) - abort the current transmission
	//CAN_CMR_RRB - (1 << 2) - The information in the Receive Buffer  is released
	//CAN_CMR_CDO - (1 << 3) - The Data Overrun bit in Status Register(s) is cleared.
	CANx->CMR = (1<<1)|(1<<2)|(1<<3);//CAN1CMR
	/* Read to clear interrupt pending in interrupt capture register */
	//it is read only register if you are reading this bit it will clear automatically
	//it is one event status register
	temp = CANx->ICR;//CAN1ICR
	//some of the can config register we can access now
	CANx->MOD = 0;// Return Normal operating

	//Reset CANAF value
	//make the acceptance filter to off mode
	//no messsage will generate interrupt
	LPC_CANAF->AFMR = 0x01;//AFMR

	//clear ALUT RAM
	//need to write cmd here
	for (i = 0; i < 512; i++) {
		LPC_CANAF_RAM->mask[i] = 0x00;
	}

	LPC_CANAF->SFF_sa = 0x00;
	LPC_CANAF->SFF_GRP_sa = 0x00;
	LPC_CANAF->EFF_sa = 0x00;
	LPC_CANAF->EFF_GRP_sa = 0x00;
	LPC_CANAF->ENDofTable = 0x00;

	//came out of the filter filter acceptance off mode
	//messsages will generate interrupt
	LPC_CANAF->AFMR = 0x00;
	/* Set baudrate */
	can_SetBaudrate (CANx, baudrate);
}

void CAN_Config (void)
{
	/* Initialize CAN1 peripheral
	 * Note: Self-test mode doesn't require pin selection
	 */
	CAN_Init(LPC_CAN1, 125000);//125kbs 1mbps 500kbps

	//Enable Interrupt
	CAN_IRQCmd(LPC_CAN1, CANINT_RIE, ENABLE);//enabling reciever interrupt 
	CAN_IRQCmd(LPC_CAN1, CANINT_TIE1, ENABLE);//enabling transmitter interrupt 

	//Enable CAN Interrupt
	NVIC_EnableIRQ(CAN_IRQn);//enabling perticular irq 
	CAN_SetAFMode(LPC_CANAF,CAN_AccBP);//acceptance filter bypass mode selected
	CAN_InitMessage();
}

Status CAN_SendMsg (LPC_CAN_TypeDef *CANx, CAN_MSG_Type *CAN_Msg)
{
	uint32_t data;
	CHECK_PARAM(PARAM_CANx(CANx));
	CHECK_PARAM(PARAM_ID_FORMAT(CAN_Msg->format));
	if(CAN_Msg->format==STD_ID_FORMAT)
	{
		CHECK_PARAM(PARAM_ID_11(CAN_Msg->id));
	}
	else
	{
		CHECK_PARAM(PARAM_ID_29(CAN_Msg->id));
	}
	CHECK_PARAM(PARAM_DLC(CAN_Msg->len));
	CHECK_PARAM(PARAM_FRAME_TYPE(CAN_Msg->type));

	//Check status of Transmit Buffer 1
	if (CANx->SR & (1<<2))
	{
		/* Transmit Channel 1 is available */
		/* Write frame informations and frame data into its CANxTFI1,
		 * CANxTID1, CANxTDA1, CANxTDB1 register */
		CANx->TFI1 &= ~0x000F0000;
		CANx->TFI1 |= (CAN_Msg->len)<<16;
		if(CAN_Msg->type == REMOTE_FRAME)
		{
			CANx->TFI1 |= (1<<30); //set bit RTR
		}
		else
		{
			CANx->TFI1 &= ~(1<<30);
		}
		if(CAN_Msg->format == EXT_ID_FORMAT)
		{
			CANx->TFI1 |= (1<<31); //set bit FF
		}
		else
		{
			CANx->TFI1 &= ~(1<<31);
		}

		/* Write CAN ID*/
		CANx->TID1 = CAN_Msg->id;

		/*Write first 4 data bytes*/
		data = (CAN_Msg->dataA[0])|(((CAN_Msg->dataA[1]))<<8)|((CAN_Msg->dataA[2])<<16)|((CAN_Msg->dataA[3])<<24);
		CANx->TDA1 = data;

		/*Write second 4 data bytes*/
		data = (CAN_Msg->dataB[0])|(((CAN_Msg->dataB[1]))<<8)|((CAN_Msg->dataB[2])<<16)|((CAN_Msg->dataB[3])<<24);
		CANx->TDB1 = data;

		 /*Write transmission request*/
		 CANx->CMR = 0x21;
		 return SUCCESS;
	}
	//check status of Transmit Buffer 2
	else if(CANx->SR & (1<<10))
	{
		/* Transmit Channel 2 is available */
		/* Write frame informations and frame data into its CANxTFI2,
		 * CANxTID2, CANxTDA2, CANxTDB2 register */
		CANx->TFI2 &= ~0x000F0000;
		CANx->TFI2 |= (CAN_Msg->len)<<16;
		if(CAN_Msg->type == REMOTE_FRAME)
		{
			CANx->TFI2 |= (1<<30); //set bit RTR
		}
		else
		{
			CANx->TFI2 &= ~(1<<30);
		}
		if(CAN_Msg->format == EXT_ID_FORMAT)
		{
			CANx->TFI2 |= (1<<31); //set bit FF
		}
		else
		{
			CANx->TFI2 &= ~(1<<31);
		}

		/* Write CAN ID*/
		CANx->TID2 = CAN_Msg->id;

		/*Write first 4 data bytes*/
		data = (CAN_Msg->dataA[0])|(((CAN_Msg->dataA[1]))<<8)|((CAN_Msg->dataA[2])<<16)|((CAN_Msg->dataA[3])<<24);
		CANx->TDA2 = data;

		/*Write second 4 data bytes*/
		data = (CAN_Msg->dataB[0])|(((CAN_Msg->dataB[1]))<<8)|((CAN_Msg->dataB[2])<<16)|((CAN_Msg->dataB[3])<<24);
		CANx->TDB2 = data;

		/*Write transmission request*/
		CANx->CMR = 0x41;
		return SUCCESS;
	}
	//check status of Transmit Buffer 3
	else if (CANx->SR & (1<<18))
	{
		/* Transmit Channel 3 is available */
		/* Write frame informations and frame data into its CANxTFI3,
		 * CANxTID3, CANxTDA3, CANxTDB3 register */
		CANx->TFI3 &= ~0x000F0000;
		CANx->TFI3 |= (CAN_Msg->len)<<16;
		if(CAN_Msg->type == REMOTE_FRAME)
		{
			CANx->TFI3 |= (1<<30); //set bit RTR
		}
		else
		{
			CANx->TFI3 &= ~(1<<30);
		}
		if(CAN_Msg->format == EXT_ID_FORMAT)
		{
			CANx->TFI3 |= (1<<31); //set bit FF
		}
		else
		{
			CANx->TFI3 &= ~(1<<31);
		}

		/* Write CAN ID*/
		CANx->TID3 = CAN_Msg->id;

		/*Write first 4 data bytes*/
		data = (CAN_Msg->dataA[0])|(((CAN_Msg->dataA[1]))<<8)|((CAN_Msg->dataA[2])<<16)|((CAN_Msg->dataA[3])<<24);
		CANx->TDA3 = data;

		/*Write second 4 data bytes*/
		data = (CAN_Msg->dataB[0])|(((CAN_Msg->dataB[1]))<<8)|((CAN_Msg->dataB[2])<<16)|((CAN_Msg->dataB[3])<<24);
		CANx->TDB3 = data;

		/*Write transmission request*/
		CANx->CMR = 0x81;
		return SUCCESS;
	}
	else
	{
		return ERROR;
	}
}

/********************************************************************//**
 * @brief		Receive message data
 * @param[in]	CANx pointer to LPC_CAN_TypeDef, should be:
 * 				- LPC_CAN1: CAN1 peripheral
 * 				- LPC_CAN2: CAN2 peripheral
 * @param[in]	CAN_Msg point to the CAN_MSG_Type Struct, it will contain received
 *  			message information such as: ID, DLC, RTR, ID Format
 * @return 		Status:
 * 				- SUCCESS: receive message successfully
 * 				- ERROR: receive message unsuccessfully
 *********************************************************************/
Status CAN_ReceiveMsg (LPC_CAN_TypeDef *CANx, CAN_MSG_Type *CAN_Msg)
{
	uint32_t data;

	CHECK_PARAM(PARAM_CANx(CANx));

	//check status of Receive Buffer
	if((CANx->SR &0x00000001))
	{
		/* Receive message is available */
		/* Read frame informations */
		CAN_Msg->format   = (uint8_t)(((CANx->RFS) & 0x80000000)>>31);
		CAN_Msg->type     = (uint8_t)(((CANx->RFS) & 0x40000000)>>30);
		CAN_Msg->len      = (uint8_t)(((CANx->RFS) & 0x000F0000)>>16);


		/* Read CAN message identifier */
		CAN_Msg->id = CANx->RID;

		/* Read the data if received message was DATA FRAME */
		if (CAN_Msg->type == DATA_FRAME)
		{
			/* Read first 4 data bytes */
			data = CANx->RDA;
			*((uint8_t *) &CAN_Msg->dataA[0])= data & 0x000000FF;
			*((uint8_t *) &CAN_Msg->dataA[1])= (data & 0x0000FF00)>>8;;
			*((uint8_t *) &CAN_Msg->dataA[2])= (data & 0x00FF0000)>>16;
			*((uint8_t *) &CAN_Msg->dataA[3])= (data & 0xFF000000)>>24;

			/* Read second 4 data bytes */
			data = CANx->RDB;
			*((uint8_t *) &CAN_Msg->dataB[0])= data & 0x000000FF;
			*((uint8_t *) &CAN_Msg->dataB[1])= (data & 0x0000FF00)>>8;
			*((uint8_t *) &CAN_Msg->dataB[2])= (data & 0x00FF0000)>>16;
			*((uint8_t *) &CAN_Msg->dataB[3])= (data & 0xFF000000)>>24;

		/*release receive buffer*/
		CANx->CMR = 0x04;
		}
		else
		{
			/* Received Frame is a Remote Frame, not have data, we just receive
			 * message information only */
			CANx->CMR = 0x04; /*release receive buffer*/
			return SUCCESS;
		}
	}
	else
	{
		// no receive message available
		return ERROR;
	}
	return SUCCESS;
}
