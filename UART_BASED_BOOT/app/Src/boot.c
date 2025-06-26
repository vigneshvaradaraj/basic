#include "lpuart.h"
#include "flash.h"
#include <stdint.h>
#include "cm4.h"
#define RESUME_CMD_TO_HOST  "RESUME"

uint32_t stack_address;
uint32_t reset_handler_add;

void (*start_app)(void) = 0;

void goto_app(void)
{
	void (*start_app)(void);
    stack_address = *((uint32_t *)(APP_START_ADDRESS));
    reset_handler_add = *((uint32_t *)(APP_START_ADDRESS + 4));
  	start_app = (void (*)(void))reset_handler_add;
  	SCB->VTOR = (uint32_t)APP_START_ADDRESS;

  	__asm volatile ("cpsid i" : : : "memory");
  	__asm volatile ("msr msp, %0" : : "r" (stack_address) : "sp");
  	__asm volatile ("MSR psp, %0\n" : : "r" (stack_address) : "memory");
  	start_app();
}

void write_opcodes_to_flash(uint16_t opcode_byte_length)
{
    Flash_WriteData(APP_START_ADDRESS,uart_rx_buffer,(opcode_byte_length/4));
    LPUART1_transmit_string(RESUME_CMD_TO_HOST);
}
