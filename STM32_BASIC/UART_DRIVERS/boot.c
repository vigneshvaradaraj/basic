
void goto_app(void)
{
	void (*start_app)(void);
    stack_address = *((uint32_t *)(app_start_add));
    reset_handler_add = *((uint32_t *)(app_start_add + 4));
  	start_app = (void (*)(void))reset_handler_add;
  	SCB->VTOR = (uint32_t)app_start_add;

  	__asm volatile ("cpsid i" : : : "memory");
  	__asm volatile ("msr msp, %0" : : "r" (stack_address) : "sp");
  	__asm volatile ("MSR psp, %0\n" : : "r" (stack_address) : "memory");
  	start_app();
}