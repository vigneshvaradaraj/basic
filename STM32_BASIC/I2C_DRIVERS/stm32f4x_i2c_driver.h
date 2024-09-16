
typedef struct
{
	uint32_t i2c_cr1;
	uint32_t i2c_xr2;
	uint32_t i2c_oar1;
	uint32_t i2c_oar2;
	uint32_t i2c_timingr;
	uint32_t i2c_timoutr;
	uint32_t i2c_icr;
	uint32_t i2c_pecr;
	uint32_t i2c_rxdr;
	uint32_t i2c_txdr;
}I2C_register_def_t;

typedef enum
{
	I2C1_BASE,
	I2C2_BASE,
	I2C3_BASE,
	I2C4_BASE
}i2c_instance_t;

typedef struct
{
	uint32_t I2C_SCLSpeed;
	uint8_t  I2C_device_address;
	uint8_t  I2C_Ack_Ctrl;
	uint16_t I2C_FM_Duty_Cycle;
}I2C_config_t;

typedef struct
{
	i2c_instance_t* instance;
	I2C_config_t config;
}I2C_Handle_t;

#define 	I2C1_BASE (uint32_t)(0x40005400)
#define 	I2C2_BASE (uint32_t)(0x40005800)
#define 	I2C3_BASE (uint32_t)(0x40005C00)
#define 	I2C4_BASE (uint32_t)(0x40008400)

#define     I2C1 (I2C_register_def_t*)I2C1_BASE
#define     I2C2 (I2C_register_def_t*)I2C2_BASE
#define     I2C3 (I2C_register_def_t*)I2C3_BASE
#define     I2C4 (I2C_register_def_t*)I2C4_BASE

#define     I2C1_CLK_ENABLE_BIT_POS (1 << 21)
#define     I2C2_CLK_ENABLE_BIT_POS (1 << 22)
#define     I2C3_CLK_ENABLE_BIT_POS (1 << 23)

//for clk enable
#define     I2C1_CLOCK_EN() (RCC_BASE->RCC_APB1ENR1 |= I2C1_CLK_ENABLE_BIT_POS)
#define     I2C2_CLOCK_EN() (RCC_BASE->RCC_APB1ENR1 |= I2C2_CLK_ENABLE_BIT_POS)
#define     I2C3_CLOCK_EN() (RCC_BASE->RCC_APB1ENR1 |= I2C3_CLK_ENABLE_BIT_POS)

//for clk disable
#define     I2C1_CLOCK_DIS() (RCC_BASE->RCC_APB1ENR1 &= ~I2C1_CLK_ENABLE_BIT_POS)
#define     I2C2_CLOCK_DIS() (RCC_BASE->RCC_APB1ENR1 &= ~I2C2_CLK_ENABLE_BIT_POS)
#define     I2C3_CLOCK_DIS() (RCC_BASE->RCC_APB1ENR1 &= ~I2C3_CLK_ENABLE_BIT_POS)

//clk speed
#define     I2C_SCL_SPEED_NM  100000
#define     I2C_SCL_SPEED_FM  400000

//ack en/dis
#define     I2C_ACK_ENABLE		1
#define     I2C_ACK_DISABLE		0

//duty cycle
#define  	I2C_FM_DUTY_2		0
#define  	I2C_FM_DUTY_16_2	1

void i2c_init(I2C_Handle_t);

