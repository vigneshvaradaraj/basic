#include <stdbool.h>
typedef unsigned char uint8_t;

#define UART_RX_BUFFER_SIZE (2048)
#define UART_CMD_BUFFER_SIZE (20)

extern bool data_ready_to_read;
extern bool data_ready_to_read;
extern bool cmd_ready_to_read;
extern bool cmd_mode;
extern bool opcode_mode;
extern uint8_t uart_rx_buffer[UART_RX_BUFFER_SIZE];
extern uint8_t cmd_buffer[UART_CMD_BUFFER_SIZE];

void LPUART1_transmit_string(char[]);
void uart_init(void);
void uart_interrupt_config();
void read(void);
