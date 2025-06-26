#include <stdint.h>

#define CAN_MAX_DATA_SIZE 8
#define ISO_TP_MAX_MESSAGE_SIZE 1024
#define ISO_TP_FRAME_TYPE_MASK 0xF0
#define ISO_TP_SINGLE_FRAME 0x00
#define ISO_TP_FIRST_FRAME  0x10
#define ISO_TP_CONSECUTIVE_FRAME 0x20
#define ISO_TP_FLOW_CONTROL 0x30

typedef enum
{
    CAN_FAILURE,
    CAN_SUCCESS
}can_error_t;


typedef struct 
{
    uint32_t id;
    uint8_t dlc;
    uint8_t data[8];
}CAN_Frame;

typedef enum
{
    NO_ACTION,
    DOWNLOAD_IN_PROGRESS,
    DOWNLOAD_COMPLETED
}isotp_status_t;

can_error_t can_init(void);
