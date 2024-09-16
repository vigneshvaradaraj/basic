#define SA 0x6A

void i2c_gpio_init()
{
	//enable clock for GPIOB
	RCC->AHB1 |= (1 << 1);
	//bydefault push pull enabled
	GPIOB->PUPDR |= (1 << 12);//internally pulling up the PB6
	GPIOB->PUPDR |= (1 << 14);//internally pulling up the PB7
	//enable alternate function for PB6
	GPIOB->MODER |= (2 << 12);
	//enable alternate function for PB7
	GPIOB->MODER |= (2 << 14);
	//setting PB6 as SCL
	GPIOB->AFR[0] |= (4 << 24);
	//setting PB7 as SDA
	GPIOB->AFR[0] |= (4 << 28);
}

void interrupt_config()
{
	//enable i2c peripheral interrupt interrupt
	I2C1->CR2 |= (1 << 9) | (1 << 9);
	//enable IRQ according to the peripheral irq number
	NVIC_EnableIRQ(I2C1_EV_IRQn);//enable IRQ by using CMSIS-CM4 file
	//in i2c there is two irqs for event and errors
}

void i2c_isr()
{
	//check the event here from status register
	if(I2C1->SR1 & (1 << 7))// Data register empty (transmitters)
	{
		//clears by calling this function (writing to DR register)
		i2c_master_send_data();
	}
	else if(I2C1->SR1 & (1 << 6))
	{
		//read the data register here
		i2cBuf = I2C1->DR;
	}
}

void i2c_clk_init()
{
	//enable clock for I2C1 peripheral
	RCC->APB1 |= (1 << 21);
}
void i2c_init(I2C_Handle_t* handle)
{
	//enable clock for I2C peripheral
	i2c_clk_init();
	//to generate a 100KHz SCL frequency 
	//APB1 Clock(PCL1) = 16Mhz
	//1)config mode in CCR reg (15th bit)
	//2)program FREQ field of CR2 with the value of PCLK1
	//3)calculate and progaram CCR value in CCR field of CCR reg
	
	//if DUTY = 0 (for standared mode)-------------------------------
	//Thigh(SCL) = CCR * Tpclk1
	//Tlow(SCL) = CCR * Tpclk1
	
	//for 100Khz 5us is Thigh(SCL)
	//for 16Mhz clk the time value is 62.5ns
	//then CCR = 80;
	//FREQ = 16,CCR = 80 for scl = 100Khz
	
	//if DUTY = 1:(to reach 400Khz)----------------------------------
	//Thigh(SCL) = 9 * CCR * Tpclk1
	//Tlow(SCL) = 16 * CCR * Tpclk1
	
	//ack cntrl bit
	handle->instance->CR1 |= handle->config.I2C_Ack_Ctrl << 10;
	//config FREQ field of CR2
	handle->instance->CR2 |= (16000000/1000000) & 0x3F;
	//only if the device is used as slave
	//need to keep that 14th bit high
	handle->instance->OAR1 |= (SA << 1) | (1 << 14);
	handle->instance->CCR |= 80;
}

void i2c_master_send_data(I2C_Handle_t* Handle,uint8_t* tx_buffer,uint32_t len)
{
	uint8_t slave_address = SA;
	uint32_t dummy;
	//1)generate start condition
	handle->instance->CR1 |= (1 << 8);
	//2)confirm that start generation is completed by checking the SB flag in SR1
		//until SB is cleared SCL will be streched (pulled low)
	while(!(handle->instance->SR1 & (1 << 0)));
	//3)send the address of the slave with read/write bit set to w(0) 
	slave_address = slave_address << 1;
	slave_address &= ~(1); //slave add + r/w bit
	handle->instance->DR = slave_address;
	//4) confirm that address phase is completed by checking the ADDR flag in SR1 
	while(!(handle->instance->SR1 & (1 << 1)));
	//5) clear the ADDR flag according to its SW sequence
	//note: until ADDR is cleared SCL will be stretched
	dummy = handle->instance->SR1;
	dummy = handle->instance->SR2;
	(void)dummy;
	//6) send data until length becomes 0
	while(len > 0)
	{
		while(!(handle->instance->SR1 & (1 << TXE));//todo: need to check tx completed bit 
		handle->instance->DR = *tx_buffer;
		tx_buffer++;
		len--;
	}
	//7)when len becomes 0 wait for the TXE=1 and BTF=1 before generating the STOP condition
	// note: TXE=1,BTF=1, means that both SR and DR are empty and next transmission should begin
	// when BTF=1 SCL will be stretched (pulled to LOW)
	while(!(handle->instance->SR1 & (1 << TXE));
	while(!(handle->instance->SR1 & (1 << BTF));
	
	//8) genetare stop condition and master need not to wait for the completion of stop condition
	//note: generating STOP, automatically clears the BTF
	handle->instance->CR1 |= (1 << 9);
}

void main()
{
	while(1)
	{
		;
	}
	
}
