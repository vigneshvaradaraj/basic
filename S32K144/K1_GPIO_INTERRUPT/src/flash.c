#include "S32K144.h"
typedef long unsigned int uint32_t;


// Flash commands
#define FTFC_CMD_PROGRAM_WORD  0x07U
#define FTFC_CMD_ERASE_SECTOR  0x09U

void flash_execute_command() 
{
    /* Start the command by clearing CCIF flag */
    FTFC->FSTAT |= FTFC_FSTAT_CCIF_MASK;

    /* Wait for the command to complete */
    while (!(FTFC->FSTAT & FTFC_FSTAT_CCIF_MASK));
}

//void flash_erase_sector(uint32_t address)
//{
//    /* Ensure the address is aligned to sector size */
//    if (address % FLASH_SECTOR_SIZE != 0) {
//        return; // Invalid address for sector erase
//    }
//
//    /* Clear error flags */
//    FTFC->FSTAT = FTFC_FSTAT_ACCERR_MASK | FTFC_FSTAT_FPVIOL_MASK;
//
//    /* Load the erase sector command */
//    FTFC->FCCOB0 = FTFC_CMD_ERASE_SECTOR;
//
//    /* Set the address */
//    FTFC->FCCOB1 = (uint8_t)(address >> 16U);
//    FTFC->FCCOB2 = (uint8_t)(address >> 8U);
//    FTFC->FCCOB3 = (uint8_t)(address);
//
//    /* Execute the command */
//    flash_execute_command();
//}

void flash_program_word(uint32_t address, uint32_t data) 
{
    /* Ensure the address is word-aligned */
    if (address % 4 != 0) {
        return; // Invalid address for programming
    }

    /* Clear error flags */
    FTFC->FSTAT = FTFC_FSTAT_ACCERR_MASK | FTFC_FSTAT_FPVIOL_MASK;

    /* Load the program word command */
    FTFC->FCCOB[0] = FTFC_CMD_PROGRAM_WORD;

    /* Set the address */
    FTFC->FCCOB[1] = (uint8_t)(address >> 16U);
    FTFC->FCCOB[2] = (uint8_t)(address >> 8U);
    FTFC->FCCOB[3] = (uint8_t)(address);

    /* Load the data */
    FTFC->FCCOB[4] = (uint8_t)(data >> 24U);
    FTFC->FCCOB[5] = (uint8_t)(data >> 16U);
    FTFC->FCCOB[6] = (uint8_t)(data >> 8U);
    FTFC->FCCOB[7] = (uint8_t)(data);

    /* Execute the command */
    flash_execute_command();
}

uint32_t flash_read_word(uint32_t address) {
    /* Read the word directly from the flash memory */
    return *(volatile uint32_t*)address;
}

void write_1kb_to_pflash(uint32_t start_address,uint32_t* opcode_buffer)
{
    int counter = 0;
    for(;counter < 1024;counter += 4)
    {
        flash_program_word(start_address + counter, *(opcode_buffer + counter));
    }
}
