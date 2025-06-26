#ifndef CAN_TP_H_
#define CAN_TP_H_

#include <stdint.h>

#define CAN_ID_FW_SENT 0x12DD0520
#define CAN_ID_FW_RECIEVE 0x188

typedef enum {
	SYS_FALSE,
	SYS_TRUE
}Sys_t;

void TP_Init(char*);
void TP_Process(void);
Sys_t CANBus_RxFrame(void);
Sys_t CANBus_TxFrame(uint8_t*,uint8_t);

#endif /*CAN_TP_H_*/