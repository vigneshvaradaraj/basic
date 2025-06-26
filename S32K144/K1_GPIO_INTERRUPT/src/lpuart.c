#include "S32K144.h" /* include peripheral declarations S32K144 */
#include "LPUART.h"
#include <stdbool.h>

#ifndef UART_RX_BUFFER_SIZE
#define UART_RX_BUFFER_SIZE (1024)
#endif

uint8_t uart_rx_buffer[UART_RX_BUFFER_SIZE] = {0};
uint8_t cmd_buffer[UART_CMD_BUFFER_SIZE] = {0};

int data_index = 0;
int cmd_index = 0;
bool data_ready_to_read = false;
bool cmd_ready_to_read = false;
bool cmd_mode = true;
bool opcode_mode = false;

void LPUART1_init(void) /* Init. summary: 9600 baud, 1 stop bit, 8 bit format, no parity */
{
 PCC->PCCn[PCC_LPUART1_INDEX] &= ~PCC_PCCn_CGC_MASK; /* Ensure clk disabled for config */
 PCC->PCCn[PCC_LPUART1_INDEX] |= PCC_PCCn_PCS(0b001) /* Clock Src= 1 (SOSCDIV2_CLK) */
 | PCC_PCCn_CGC_MASK; /* Enable clock for LPUART1 regs */
 LPUART1->BAUD = 0x0F000034; /* Initialize for 9600 baud, 1 stop: */
 /* SBR=52 (0x34): baud divisor = 8M/9600/16 = ~52 */
/* OSR=15: Over sampling ratio = 15+1=16 */
/* SBNS=0: One stop bit */
/* BOTHEDGE=0: receiver samples only on rising edge */
/* M10=0: Rx and Tx use 7 to 9 bit data characters */
/* RESYNCDIS=0: Resync during rec'd data word supported */
/* LBKDIE, RXEDGIE=0: interrupts disable */
/* TDMAE, RDMAE, TDMAE=0: DMA requests disabled */
/* MAEN1, MAEN2, MATCFG=0: Match disabled */
 LPUART1->CTRL=0x000C0000; /* Enable transmitter & receiver, no parity, 8 bit char: */
 /* RE=1: Receiver enabled */
/* TE=1: Transmitter enabled */
/* PE,PT=0: No hw parity generation or checking */
/* M7,M,R8T9,R9T8=0: 8-bit data characters*/
/* DOZEEN=0: LPUART enabled in Doze mode */
 /* ORIE,NEIE,FEIE,PEIE,TIE,TCIE,RIE,ILIE,MA1IE,MA2IE=0: no IRQ*/
 /* TxDIR=0: TxD pin is input if in single-wire mode */
/* TXINV=0: TRansmit data not inverted */
/* RWU,WAKE=0: normal operation; rcvr not in statndby */
/* IDLCFG=0: one idle character */
/* ILT=0: Idle char bit count starts after start bit */
/* SBK=0: Normal transmitter operation - no break char */
/* LOOPS,RSRC=0: no loop back */
LPUART1->CTRL |= (1 << 21);//enable the receiver interrupt
}
// S32K1xx Series Cookbook, Rev. 5, December 2020
// Software examples
// 38 NXP Semiconductors
void LPUART1_transmit_char(char send) { /* Function to Transmit single Char */
 while((LPUART1->STAT & LPUART_STAT_TDRE_MASK)>>LPUART_STAT_TDRE_SHIFT==0);
 /* Wait for transmit buffer to be empty */
 LPUART1->DATA=send; /* Send data */
}
void LPUART1_transmit_string(char data_string[]) 
{ /* Function to Transmit whole string */
    uint32_t i=0;
    while(data_string[i] != '\0') 
    { /* Send chars one at a time */
        LPUART1_transmit_char(data_string[i]);
        i++;
    }
}
char LPUART1_receive_char(void) { /* Function to Receive single Char */
 char receive;
 while((LPUART1->STAT & LPUART_STAT_RDRF_MASK)>>LPUART_STAT_RDRF_SHIFT==0);
 /* Wait for received buffer to be full */
 receive= LPUART1->DATA; /* Read received data*/
 return receive;
}

void lpuart1_callback(void)
{
    //need to map it to vector table
    if(true == cmd_mode)
    {
        cmd_buffer[cmd_index++] = LPUART1_receive_char();

        if(UART_CMD_BUFFER_SIZE >= cmd_index)
        {
            cmd_index = 0;
            cmd_ready_to_read = true;
        }
    }
    else if(true == opcode_mode)
    {
        uart_rx_buffer[data_index++] = LPUART1_receive_char();
        if(UART_RX_BUFFER_SIZE >= data_index)
        {
            cmd_mode = true;
            opcode_mode = false;
            data_index = 0;
            data_ready_to_read = true;
        }
    }
}
