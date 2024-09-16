#define SA 0x6A
void i2c_init(I2C_Handle_t* handle)
{
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
	handle->instance->OAR1 |= (handle->config.I2C_device_address << 1) | (1 << 14);
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
