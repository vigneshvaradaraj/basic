#include "can_tp.h"
#include "can_uds.h"
#include "stm8s_flash.h"

#include <stdlib.h>
#include <string.h>

#define TIM4_PERIOD   	(124)  // for 1 ms
#define TX_FRAME_SIZE   (8)  // for 1 ms

static volatile uint8_t test;

static union
{
	uint32_t data;
	uint8_t bytes[4];
}memory;

TpInfo_t g_tpInfo;

static void TP_Responce(uint8_t *buffer);
static void TP_TxFlowCtrl(uint8_t length, uint8_t timeout);
static void RequestDownload(uint8_t *buffer);

void TP_Init(void)
{
	g_tpInfo.tpState 							= NORMAL_STATE;
	g_tpInfo.dataBuffer 					= (uint8_t *)malloc(RX_BUFFER_SIZE * sizeof(uint8_t)); 
	g_tpInfo.transferDataReadyFlg = SYS_FALSE;
}

void TP_Process(void)
{
	volatile uint8_t idx 					= 0;
	volatile uint16_t flashIdx 		= 0;
	volatile uint8_t tempLen 			= 0;
	volatile uint8_t memorySize		= 0;
	volatile uint8_t memoryAddressSize 		= 0;
	volatile static int32_t frameLength 	= 0;
	volatile static uint8_t headerByteCtr = 0;
	volatile uint8_t txBuffer[TX_FRAME_SIZE];
	
	switch(g_tpInfo.tpState)
	{
		case NORMAL_STATE:
		{
			if(CANBus_RxFrame(g_tpInfo.rxBuffer) == SYS_TRUE)
			{
				if(g_tpInfo.rxBuffer[0] & 0x10)
				{
					g_tpInfo.tpState 		= FIRST_FRAME_STATE;
					g_tpInfo.rxByteCtr  = 0;
				}
				else if(g_tpInfo.rxBuffer[0] & 0x20)
				{
					frameLength 			= 0;
					g_tpInfo.tpState 	= CONSECUTIVE_FRAME_STATE;
				}
				else if(g_tpInfo.rxBuffer[0] & 0x30)
				{
					g_tpInfo.tpState = FLOW_CTRL_FRAME_STATE;
				}
				else if((g_tpInfo.rxBuffer[0] & 0xF0) == 0x00)
				{
					g_tpInfo.tpState = SINGLE_FRAME_STATE;
				}
			}
			break;
		}
		
		case SINGLE_FRAME_STATE:
		{
			UDS_Process(g_tpInfo.rxBuffer);
			g_tpInfo.tpState = NORMAL_STATE;			
			break;
		}
		
		case FIRST_FRAME_STATE:
		{
			frameLength = g_tpInfo.rxBuffer[1]; //todo  [0]14 [1]00  -> length = 0x400
			frameLength = frameLength + 2; //2 byte -> function and block idx
			g_tpInfo.flashBufferLength = frameLength;
			
			for(idx = 2; idx < 8; idx++)
			{
				g_tpInfo.dataBuffer[g_tpInfo.rxByteCtr++] = g_tpInfo.rxBuffer[idx];
				frameLength--;
			}
			
			if(frameLength > 0)
			{
				g_tpInfo.tpState = CONSECUTIVE_FRAME_STATE;
				g_tpInfo.cFrameIdxCtr = 0x21;
			}						
			break;
		}
		
		case CONSECUTIVE_FRAME_STATE:
		{
			TP_TxFlowCtrl(0x00, g_tpInfo.rxBuffer[1]); //0x00 for all pending frame
			g_tpInfo.frameTimeOut = 0;
			
			while(frameLength > 0)
			{
				if(CANBus_RxFrame(g_tpInfo.rxBuffer) == SYS_TRUE)
				{
					g_tpInfo.frameTimeOut = 0; 
					
					if(g_tpInfo.cFrameIdxCtr == g_tpInfo.rxBuffer[0])
					{
						if(frameLength < 8)
						{
							tempLen = frameLength;
							
							for(idx = 1; idx <= tempLen; idx++)
							{
								g_tpInfo.dataBuffer[g_tpInfo.rxByteCtr++] = g_tpInfo.rxBuffer[idx];
								frameLength--;
							}
						}
						else
						{
							for(idx = 1; idx < 8; idx++)
							{
								g_tpInfo.dataBuffer[g_tpInfo.rxByteCtr++] = g_tpInfo.rxBuffer[idx];
								frameLength--;
							}
						}			
						
						if(++g_tpInfo.cFrameIdxCtr > 0x2F)
						{
							g_tpInfo.cFrameIdxCtr = 0x20;
						}
					}
					else
					{
						//negative frame 
						txBuffer[0] = 0x03;
						txBuffer[1] = 0x7F;	
						txBuffer[2] = 0x34;  
						txBuffer[3] = 0x13;//incorrect message length or invalid format
						txBuffer[4] = 0x00;
						txBuffer[5] = 0x00;
						txBuffer[6] = 0x00;
						txBuffer[7] = 0x00;
						g_tpInfo.tpState = NORMAL_STATE;
						CANBus_TxFrame(txBuffer);							
						return;
					}						
				}	
				/*
				if(g_tpInfo.frameTimeOut > 10000)
				{
					g_tpInfo.frameTimeOut = 0;
					//negative frame
					txBuffer[0] = 0x03;
					txBuffer[1] = 0x7F;	
					txBuffer[2] = 0x34;  
					txBuffer[3] = 0x14;//Response Too Long
					txBuffer[4] = 0x00;
					txBuffer[5] = 0x00;
					txBuffer[6] = 0x00;
					txBuffer[7] = 0x00;
					g_tpInfo.tpState = NORMAL_STATE;
					CANBus_TxFrame(txBuffer);		
					
					return;
				}
				g_tpInfo.frameTimeOut++;*/
			}
			
			if(frameLength != 0)
			{
				//negative frame
				txBuffer[0] = 0x03;
				txBuffer[1] = 0x7F;	
				txBuffer[2] = 0x34;  
				txBuffer[3] = 0x13;//incorrect message length or invalid format
				txBuffer[4] = 0x00;
				txBuffer[5] = 0x00;
				txBuffer[6] = 0x00;
				txBuffer[7] = 0x00;
				g_tpInfo.tpState = NORMAL_STATE;	
				CANBus_TxFrame(txBuffer);	
				return;				
			}
			
			if(g_tpInfo.dataBuffer[0] == REQUEST_DOWNLOAD) //Request Download
			{
				if(g_tpInfo.dataBuffer[1] != 0x00)//Data Format Identifier(0x00 not compressed/encrypted)
				{
					//negative frame
					txBuffer[0] = 0x03;
					txBuffer[1] = 0x7F;	
					txBuffer[2] = 0x34;  
					txBuffer[3] = 0x13;//incorrect message length or invalid format
					txBuffer[4] = 0x00;
					txBuffer[5] = 0x00;
					txBuffer[6] = 0x00;
					txBuffer[7] = 0x00;
					
					g_tpInfo.tpState = NORMAL_STATE;	
					CANBus_TxFrame(txBuffer);	
				}
				else
				{
					memoryAddressSize	= g_tpInfo.dataBuffer[2] & 0x0F;
					memorySize				= (g_tpInfo.dataBuffer[2] >> 4) & 0x0F;
					
					if(memoryAddressSize > 5 || memorySize > 4)
					{
						//negative frame
						txBuffer[0] = 0x03;
						txBuffer[1] = 0x7F;	
						txBuffer[2] = 0x34;  
						txBuffer[3] = 0x13;//incorrect message length or invalid format
						txBuffer[4] = 0x00;
						txBuffer[5] = 0x00;
						txBuffer[6] = 0x00;
						txBuffer[7] = 0x00;
						
						g_tpInfo.tpState = NORMAL_STATE;						
						CANBus_TxFrame(txBuffer);
						return;
					}			
					
					g_tpInfo.memoryStartingAddress = 0;
						
					for(idx = 3; idx < (3 + memoryAddressSize); idx++)
					{
						if(idx == 3)
						{
							g_tpInfo.memoryStartingAddress |= g_tpInfo.dataBuffer[idx];
						}
						else
						{
							g_tpInfo.memoryStartingAddress  = (g_tpInfo.memoryStartingAddress << 8);
							g_tpInfo.memoryStartingAddress |= g_tpInfo.dataBuffer[idx];
						}
					}
					g_tpInfo.currentMemoryAddress = g_tpInfo.memoryStartingAddress;
					g_tpInfo.memoryLength = 0;
						
					for(idx = (3 + memoryAddressSize); 
							idx < (3 + memoryAddressSize + memorySize); idx++)
					{
						if(idx == (3 + memoryAddressSize))
						{
							g_tpInfo.memoryLength |= g_tpInfo.dataBuffer[idx];
						}
						else
						{
							g_tpInfo.memoryLength  = (g_tpInfo.memoryLength << 8);
							g_tpInfo.memoryLength |= g_tpInfo.dataBuffer[idx];
						}
					}											
				}
				
				if(g_tpInfo.memoryLength < RX_BUFFER_SIZE)
				{
					txBuffer[0] = 0x03;	
					txBuffer[1] = 0x74;	//response 
					txBuffer[2] = 0x10; //2  byte
					txBuffer[3] = g_tpInfo.memoryLength; //maximum bytes
					txBuffer[4] = 0x00;
					txBuffer[5] = 0x00;
					txBuffer[6] = 0x00;
					txBuffer[7] = 0x00;
					CANBus_TxFrame(txBuffer);
				}
				else if((g_tpInfo.memoryLength > 255) && (g_tpInfo.memoryLength < 4091))
				{
					//response frame
					txBuffer[0] = 0x04;	
					txBuffer[1] = 0x74;	//response 
					txBuffer[2] = 0x20; //2  byte
					txBuffer[3] = (g_tpInfo.memoryLength >> 8) & 0x0F; 
					txBuffer[4] = (g_tpInfo.memoryLength & 0xFF);
					txBuffer[5] = 0x00;
					txBuffer[6] = 0x00;
					txBuffer[7] = 0x00;
					CANBus_TxFrame(txBuffer);
				}		
				else if(g_tpInfo.memoryLength > 4090)
				{
					//response frame
					txBuffer[0] = 0x04;	
					txBuffer[1] = 0x74;	//response 
					txBuffer[2] = 0x20; //single  byte
					txBuffer[3] = 0x0F; // B1
					txBuffer[4] = 0xFA; // B1: B1 = 0x0FFA(4090) bytes max
					txBuffer[5] = 0x00;
					txBuffer[6] = 0x00;
					txBuffer[7] = 0x00;
					CANBus_TxFrame(txBuffer);
				}
				g_tpInfo.requestDownloadFlg = SYS_TRUE;
				memset(g_tpInfo.dataBuffer, 0x00, RX_BUFFER_SIZE);
				CANBus_TxFrame(txBuffer);
			}
			
			if(g_tpInfo.dataBuffer[0] == TRANSFER_DATA)
			{
				if(g_tpInfo.requestDownloadFlg == SYS_TRUE)
				{					 
					//g_tpInfo.dataBuffer[0]	= g_tpInfo.rxBuffer[1];
					
					
					// write into flash before response
										
					if(g_tpInfo.dataBuffer[1] == 0x01)//if sequense number 1 then remove header
					{
						headerByteCtr = 0;
						g_tpInfo.rxBlockCtr = 0x01; 
						
						for(idx = 0; idx < RX_BUFFER_SIZE; idx++)						
						{
							if(g_tpInfo.dataBuffer[idx] == 0xBB)
							{
								headerByteCtr++;
								
								if(headerByteCtr > 3)
								{
									g_tpInfo.dataBufferStartIdx = idx + 13;
									break;
								}
							}
							else
							{
								headerByteCtr = 0;
							}
						}						

						for(flashIdx = g_tpInfo.dataBufferStartIdx; flashIdx < g_tpInfo.flashBufferLength; flashIdx++)	
						{
							FLASH_ProgramByte(g_tpInfo.currentMemoryAddress++, g_tpInfo.dataBuffer[flashIdx]);
							nop();
							nop();
							nop();
							nop();
							nop();//todo delay
						}						
					}
					else
					{
						if(g_tpInfo.dataBuffer[1] <= g_tpInfo.numberOfblocks)
						{
							if(g_tpInfo.rxBlockCtr == g_tpInfo.dataBuffer[1])
							{
								for(flashIdx = 2; flashIdx < g_tpInfo.flashBufferLength; flashIdx++)	
								{
									FLASH_ProgramByte(g_tpInfo.currentMemoryAddress++, g_tpInfo.dataBuffer[flashIdx]);
									nop();
									nop();
									nop();
									nop();
									nop();//todo delay
								}									
							}
							else
							{
								//negative frame
								txBuffer[0] = 0x03;
								txBuffer[1] = 0x7F;	
								txBuffer[2] = 0x36;  
								txBuffer[3] = 0x73;
								txBuffer[4] = 0x00;
								txBuffer[5] = 0x00;
								txBuffer[6] = 0x00;
								txBuffer[7] = 0x00;
								g_tpInfo.tpState = NORMAL_STATE;						
								CANBus_TxFrame(txBuffer);
								return;
							}
						}
					}
					//response frame
					memset(txBuffer, 0x00, TX_FRAME_SIZE);
					txBuffer[0] = 0x02;	
					txBuffer[1] = 0x76;	//response 
					txBuffer[2] = g_tpInfo.rxBlockCtr;
					g_tpInfo.tpState = NORMAL_STATE;
					CANBus_TxFrame(txBuffer);	
					g_tpInfo.rxBlockCtr++;																
				}
				else
				{
					/*
					//negative frame
					txBuffer[0] = 0x03;
					txBuffer[1] = 0x7F;	
					txBuffer[2] = 0x36;  
					txBuffer[3] = 0x13;//incorrect message length or invalid format
					txBuffer[4] = 0x00;
					txBuffer[5] = 0x00;
					txBuffer[6] = 0x00;
					txBuffer[7] = 0x00;
					g_tpInfo.tpState = NORMAL_STATE;						
					CANBus_TxFrame(txBuffer);
					return;
					*/
				}
				
				if(g_tpInfo.dataBuffer[1] == g_tpInfo.numberOfblocks)
				{
					g_tpInfo.tpState = NORMAL_STATE;
					g_tpInfo.requestDownloadFlg = SYS_FALSE;
				}
			}	
			g_tpInfo.tpState = NORMAL_STATE;			
			break;
		}
		
		case FLOW_CTRL_FRAME_STATE:
		{
			break;
		}
		
		default:
		{
			break;
		}		
	}
}

static void TP_TxFlowCtrl(uint8_t length, uint8_t timeout)
{
	uint8_t txBuffer[8];
	
	txBuffer[0] = 0x30;	//flow ctrl
	txBuffer[1] = length;	//next frame
	txBuffer[2] = timeout;//10ms
	txBuffer[3] = 0x00;
	txBuffer[4] = 0x00;
	txBuffer[5] = 0x00;
	txBuffer[6] = 0x00;
	txBuffer[7] = 0x00;
	
	CANBus_TxFrame(txBuffer);	
}

