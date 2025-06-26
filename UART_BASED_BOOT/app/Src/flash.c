#define buffer_size (1024)
#define app_start_add (0x08008000)

#include "stm32_f4xx.h"  // Include the device header for STM32F4 family
#include <stdint.h>

#define FLASH_SECTOR7_BASE_ADDR   app_start_add   // Start of Sector 7 for STM32F4xx
#define FLASH_KEY1                0x45670123
#define FLASH_KEY2                0xCDEF89AB

uint32_t data_to_write[2] = {0x12345678, 0x9ABCDEF0}; // Example data to write
uint32_t data_read[2];

/* Function to Unlock Flash */
void Flash_Unlock(void)
{
    if ((FLASH->CR & FLASH_CR_LOCK) != 0)  // If Flash is locked
    {
        FLASH->KEYR = FLASH_KEY1;  // Unlock Flash by writing keys
        FLASH->KEYR = FLASH_KEY2;
    }
}

/* Function to Lock Flash */
void Flash_Lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;  // Set the LOCK bit in CR register
}

/* Function to Erase Flash Sector */
void Flash_EraseSector(uint32_t Sector)
{
    // Unlock Flash
    Flash_Unlock();

    // Wait until Flash is not busy
    while (FLASH->SR & FLASH_SR_BSY);

    // Set the sector number and sector erase mode
    FLASH->CR &= ~FLASH_CR_SNB;                  // Clear the sector number bits
    FLASH->CR |= FLASH_CR_SER | (Sector << 3);   // Set SER (sector erase) and select sector
    FLASH->CR |= FLASH_CR_STRT;                  // Start the erase operation

    // Wait for the operation to complete
    while (FLASH->SR & FLASH_SR_BSY);

    // Clear SER bit and sector number
    FLASH->CR &= ~(FLASH_CR_SER | FLASH_CR_SNB);

    // Lock Flash
    Flash_Lock();
}

/* Function to Program Flash */
void Flash_WriteData(uint32_t Address, uint8_t *Data, uint32_t Length)
{
    // Unlock Flash
    Flash_Unlock();

    // Wait until Flash is not busy
    while (FLASH->SR & FLASH_SR_BSY);

    for (uint32_t i = 0; i < Length; i++)
    {
        FLASH->CR |= FLASH_CR_PG;                  // Enable programming mode
        *(volatile uint8_t*)(Address + i) = Data[i]; // Write data to address

        // Wait for the operation to complete
        while (FLASH->SR & FLASH_SR_BSY);

        // Check for errors
        if (FLASH->SR & (FLASH_SR_PGSERR | FLASH_SR_PGPERR | FLASH_SR_PGAERR))
        {
            // Handle the error (here we just clear the flags for simplicity)
            FLASH->SR |= FLASH_SR_PGSERR | FLASH_SR_PGPERR | FLASH_SR_PGAERR;
            break;
        }

        FLASH->CR &= ~FLASH_CR_PG;  // Disable programming mode after each write
    }

    // Lock Flash
    Flash_Lock();
}

/* Function to Read Flash */
void Flash_ReadData(uint32_t Address, uint32_t *Data, uint32_t Length)
{
    // Copy data from flash memory to the data buffer
    for (uint32_t i = 0; i < Length; i++)
    {
        Data[i] = *(volatile uint32_t*)(Address + i * 4);
    }
}
