#define APP_START_ADDRESS (0x080E0000)//1MB for Boot another 1MB for App
void Flash_WriteData(uint32_t Address, uint8_t *Data, uint32_t Length);
typedef long unsigned int uint32_t;
