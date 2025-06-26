#ifndef CAN_TP_H_
#define CAN_TP_H_

#include "app_config.h"

typedef enum
{
	NORMAL_STATE,
	SINGLE_FRAME_STATE,
	FIRST_FRAME_STATE,
	CONSECUTIVE_FRAME_STATE,
	FLOW_CTRL_FRAME_STATE,
	MAX_STATE
}TP_State_t;

typedef struct
{
	bool requestDownloadFlg;
	bool transferDataReadyFlg;
	bool rxFrameCompletedFlg;
	
	uint8_t rxBuffer[8];
	uint8_t *dataBuffer;
	uint8_t dataBufferStartIdx;
	uint16_t rxBlockCtr;
	uint16_t numberOfblocks;
	uint16_t rxByteCtr;
	uint16_t cFrameIdxCtr;
	uint8_t frameLength;
	uint32_t memoryStartingAddress;
	uint32_t memoryLength;
	uint32_t currentMemoryAddress;
	uint32_t flashBufferLength;
	uint32_t frameTimeOut;
	TP_State_t tpState;	
}TpInfo_t;

void TP_Init(void);
void TP_Process(void);


#endif /*CAN_TP_H_*/