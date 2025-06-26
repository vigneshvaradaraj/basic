#include "can.h"
#include "stm32_f4xx.h"
#include "cm4.h"

isotp_status_t isoTpStatus;

uint8_t opcode_buffer[ISO_TP_MAX_MESSAGE_SIZE] = {0};
CAN_Frame can_frame = {0,0,{0,0,0,0,0,0,0,0}};
uint16_t receved_message_len = 0,total_message_legth = 0;

can_error_t can_interrupt_init(void)
{
    //enable fifo0 full interrupt
    CAN1->IER |= (1 << 2);
    //enable fifo1 full interrupt
    CAN1->IER |= (1 << 5);
    NVIC_EnableIRQ(CAN1_RX0_IRQn);
    NVIC_EnableIRQ(CAN1_RX1_IRQn);
    NVIC_SetPriorityGrouping(0);
    NVIC_SetPriority(CAN1_RX1_IRQn,2);
    NVIC_SetPriority(CAN1_RX0_IRQn,1);
}

can_error_t can_init(void)
{
    RCC->AHB1ENR |= (1 << 3);//enable clock for gpioD
    //config PD0 and PD1 as alternate functionality
    GPIOD->MODER |= (2 << 0);
    GPIOD->MODER |= (2 << 2);
    //config the AF9 for CAN1
    GPIOD->AFR[0] |= (9 << 0);
    GPIOD->AFR[0] |= (9 << 4);

    //enable clock for can1
    RCC->APB1ENR |= (1 << 25);
    //reset can peripheral
    CAN1->MCR |= (1 << 15);
    //exit sleepmode
    CAN1->MCR &= ~(1 << 1);
    //enter into init mode
    CAN1->MCR |= (1 << 0);
    //wait for hardware enter into init mode
    while(!(CAN1->MSR & (1 << 0)));
    //internal oscillator 16Mhz (125Kbps)
    //config time quanta
    //CAN1->BTR = (9 << 20)|(2 << 16)|(13 << 0);
    //CAN1->BTR = 0x001C0003;
    //CAN1->BTR = 0x001C0001;

    //can_interrupt_init();

    //exit init mode
    CAN1->MCR &= ~(1 << 0);
    //wait for hardware to enter into normal mode
    while(CAN1->MSR & (1 << 0));

    return CAN_SUCCESS;
}

void can_send(uint32_t id,uint8_t* data,uint8_t size)
{
    //wait for a while to txMailbox0 empty
    while(!(CAN1->TSR & (1 << 26)));

    CAN1->sTxMailBox[0].TIR |= (1 << 2);//extended ID
    CAN1->sTxMailBox[0].TIR |= (id << 3);
    CAN1->sTxMailBox[0].TDTR = size;
    CAN1->sTxMailBox[0].TDLR = *((uint32_t*)data);
    CAN1->sTxMailBox[0].TDHR = *((uint32_t*)(data + 4));

    CAN1->sTxMailBox[0].TIR |= (1 << 0);
    //wait for the current
    while(!(CAN1->TSR & (1 << 0)));

    while(0);
}

//void can_recieve(bool fifo_flag)
//{
//    CAN_Frame frame = {0,0,0};
//
//    CAN1->sFIFOMailBox[0].RIR;
//    //frame.dlc;
//    //memcpy(frame.data,,8);
//    //frame.id;
//    if(iso_tp_receive(&frame,&opcodeBuffer,&opcodeBufferLen) == 0)
//    {
//        //todo:
//    }
//}

int iso_tp_receive(CAN_Frame *frame, uint8_t *buffer, uint16_t *message_length,uint16_t* total_message_legth) 
{
    static uint8_t temp_buffer[ISO_TP_MAX_MESSAGE_SIZE];
    static uint16_t expected_length = 0;
    static uint16_t received_length = 0;
    static uint8_t sequence_number = 1;

    uint8_t frame_type = frame->data[0] & ISO_TP_FRAME_TYPE_MASK;

    switch (frame_type) 
	{
        case ISO_TP_SINGLE_FRAME:
            *message_length = frame->data[0] & 0x0F;
            *total_message_legth = *message_length;
            memcpy(buffer, &frame->data[1], *message_length);
            return 0;

        case ISO_TP_FIRST_FRAME:
            expected_length = ((frame->data[0] & 0x0F) << 8) | frame->data[1];
            *total_message_legth = expected_length;
            memcpy(temp_buffer, &frame->data[2], 6);
            received_length = 6;
            sequence_number = 1;
            return 1; // Indicate multi-frame reception

        case ISO_TP_CONSECUTIVE_FRAME:
            if ((frame->data[0] & 0x0F) != sequence_number) {
                printf("Sequence number mismatch\n");
                return -1;
            }
            uint8_t bytes_to_copy = (expected_length - received_length > 7) ? 7 : (expected_length - received_length);
            memcpy(&temp_buffer[received_length], &frame->data[1], bytes_to_copy);
            received_length += bytes_to_copy;
            *message_length = received_length;
            sequence_number++;

            if (received_length >= expected_length) {
                memcpy(buffer, temp_buffer, expected_length);
                *message_length = expected_length;
                return 0; // Message fully received
            }
            return 1; // Waiting for more frames

        default:
            printf("Unknown frame type\n");
            return -1;
    }
}

//int getOpcodeBufferLen(void)
//{
//    return opcodeBufferLen;
//}

void CAN1_RX0_IRQHandler(void)
{
    if(CAN1->RF0R & 0x3)
    {

        can_frame.id = (CAN1->sFIFOMailBox[0].RIR >> 3);
        can_frame.dlc = CAN1->sFIFOMailBox[0].RDTR & 0x0f;
        can_frame.data[0] = CAN1->sFIFOMailBox[0].RDLR & 0xff;
        can_frame.data[1] = (CAN1->sFIFOMailBox[0].RDLR >> 8) & 0xff;
        can_frame.data[2] = (CAN1->sFIFOMailBox[0].RDLR >> 16) & 0xff;
        can_frame.data[3] = (CAN1->sFIFOMailBox[0].RDLR >> 24) & 0xff;
        can_frame.data[4] = CAN1->sFIFOMailBox[0].RDHR & 0xff;
        can_frame.data[5] = (CAN1->sFIFOMailBox[0].RDHR >> 8) & 0xff;
        can_frame.data[6] = (CAN1->sFIFOMailBox[0].RDHR >> 16) & 0xff;
        can_frame.data[7] = (CAN1->sFIFOMailBox[0].RDHR >> 24) & 0xff;

        CAN1->RF0R |= (1 << 5);//relese the fifo
    }

    iso_tp_receive(&can_frame,opcode_buffer,&receved_message_len,&total_message_legth);
	while(0);
}

void CAN1_RX1_IRQHandler(void)
{
    //if rxfifo1
    //can_recieve(1);
	while(0);
}
