#include "stmflash.h"
#include "system.h"

// 扇区定义
uint32_t sectStartAddr[8] = {
    0x08000000, // 扇区0: 16KB
    0x08004000, // 扇区1: 16KB
    0x08008000, // 扇区2: 16KB
    0x0800C000, // 扇区3: 16KB
    0x08010000, // 扇区4: 64KB
    0x08020000, // 扇区5: 128KB
    0x08040000, // 扇区6: 128KB
    0x08060000  // 扇区7: 128KB (实际到0x0807FFFF)
};

// 获取扇区编号
int GetSectorFromAddress(uint32_t address)
{
    if(address < 0x08000000 || address > 0x0807FFFF)
        return -1;
    
    for(int i = 0; i < 7; i++) {
        if(address >= sectStartAddr[i] && address < sectStartAddr[i+1])
            return i;
    }
    
    if(address >= sectStartAddr[7] && address <= 0x0807FFFF)
        return 7;
    
    return -1;
}

// 清除扇区
uint8_t FlashErase(uint32_t StartAddress)
{
    int sect = GetSectorFromAddress(StartAddress);
    if(sect < 0) return 1; // 地址无效
    
    HAL_StatusTypeDef status;
    
    HAL_FLASH_Unlock();
    
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                          FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
    
    FLASH_Erase_Sector(sect, FLASH_VOLTAGE_RANGE_3);
    
    status = HAL_FLASH_GetError();
    HAL_FLASH_Lock();
    
    return (status != HAL_FLASH_ERROR_NONE) ? 1 : 0;
}

// 写数据
uint8_t FlashWrite(uint32_t StartAddress, uint32_t data)
{
    // 检查地址对齐
    if(StartAddress & 0x03) return 1;
    
    // 检查地址范围
    if(StartAddress < 0x08000000 || StartAddress > 0x0807FFFF) return 1;
    
    // 检查是否已擦除
    if(*(uint32_t*)StartAddress != 0xFFFFFFFF) return 1;
    
    HAL_StatusTypeDef status;
    
    HAL_FLASH_Unlock();
    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, StartAddress, data);
    HAL_FLASH_Lock();
    
    return (status != HAL_OK) ? 1 : 0;
}

// 读数据
uint32_t FlashRead(uint32_t StartAddress)
{
    if(StartAddress < 0x08000000 || StartAddress > 0x0807FFFF)
        return 0xFFFFFFFF;
    
    return *(uint32_t *)StartAddress;
}

// 连续写入多个32位数据
// StartAddress: 起始地址
// pData: 要写入的数据指针
// NumToWrite: 要写入的数据个数
// 返回值: 0=成功, 1=失败
uint8_t FlashWriteMulti(uint32_t StartAddress, uint32_t *pData, uint32_t NumToWrite)
{
    // 检查地址对齐
    if(StartAddress & 0x03) return 1;
    
    // 检查地址范围
    if(StartAddress < 0x08000000 || StartAddress > 0x0807FFFF) return 1;
    
    // 检查结束地址是否超出范围
    uint32_t endAddr = StartAddress + (NumToWrite * 4) - 1;
    if(endAddr > 0x0807FFFF) return 1;
    
    HAL_FLASH_Unlock();
    
    for(uint32_t i = 0; i < NumToWrite; i++)
    {
        uint32_t addr = StartAddress + i * 4;
        
        // 检查是否已擦除
        if(*(uint32_t*)addr != 0xFFFFFFFF) {
            HAL_FLASH_Lock();
            return 1;
        }
        
        // 写入数据
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, pData[i]) != HAL_OK) {
            HAL_FLASH_Lock();
            return 1;
        }
    }
    
    HAL_FLASH_Lock();
    return 0;
}

uint8_t FlashWriteMulti_Byte(uint32_t StartAddress, uint8_t *pData, uint32_t NumToWrite)
{
    uint32_t currentAddr;
    uint32_t remain;
    uint32_t dataIdx = 0;
    uint32_t wordData;
    HAL_StatusTypeDef status;
    __disable_irq();  // Flash操作期间禁止中断
    
    // 参数检查
    if(pData == NULL) {
        __enable_irq();
        return 1;
    }
    if(StartAddress < 0x08000000 || StartAddress > 0x0807FFFF) {
        __enable_irq();
        return 1;
    }
    
    uint32_t endAddr = StartAddress + NumToWrite - 1;
    if(endAddr > 0x0807FFFF) {
        __enable_irq();
        return 1;
    }
    
    // 检查是否需要擦除
    int startSector = GetSectorFromAddress(StartAddress);
    int endSector = GetSectorFromAddress(endAddr);
    if(startSector < 0 || endSector < 0) {
        __enable_irq();
        return 1;
    }
    
    uint8_t needErase = 0;
    uint32_t checkAddr = StartAddress & ~0x03;
    uint32_t checkEnd = (endAddr + 3) & ~0x03;
    
    for(uint32_t addr = checkAddr; addr <= checkEnd; addr += 4) {
        uint32_t currentWord = *(uint32_t*)addr;
        uint32_t targetWord = 0xFFFFFFFF;
        
        for(uint32_t i = 0; i < 4; i++) {
            uint32_t byteAddr = addr + i;
            if(byteAddr >= StartAddress && byteAddr < StartAddress + NumToWrite) {
                ((uint8_t*)&targetWord)[i] = pData[byteAddr - StartAddress];
            }
        }
        
        if((currentWord & targetWord) != targetWord) {
            needErase = 1;
            break;
        }
    }
    
    // 如果需要擦除，执行擦除
    if(needErase) {
        HAL_FLASH_Unlock();
        
        // 擦除前先清除所有标志位
        __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                              FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR |
                              FLASH_FLAG_BSY);  // 增加 BSY 标志清除
        
        for(int sector = startSector; sector <= endSector; sector++) {
            FLASH_Erase_Sector(sector, FLASH_VOLTAGE_RANGE_3);
            
            // 等待擦除完成
            while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY)) {
                // 等待
            }
            
            if(HAL_FLASH_GetError() != HAL_FLASH_ERROR_NONE) {
                HAL_FLASH_Lock();
                __enable_irq();
                return 1;
            }
        }
        
        HAL_FLASH_Lock();
        
        // 验证擦除（注意：这里直接读取Flash可能会慢）
        for(uint32_t addr = checkAddr; addr <= checkEnd; addr += 4) {
            if(*(uint32_t*)addr != 0xFFFFFFFF) {
                __enable_irq();
                return 1;
            }
        }
    }
    
    // 写入前等待Flash空闲
    while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY)) {
        // 等待之前的Flash操作完成
    }
    
    // 写入数据
    HAL_FLASH_Unlock();
    
    // 写入前再次清除标志位
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                          FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
    
    remain = NumToWrite;
    currentAddr = StartAddress;
    
    // 处理未对齐的起始部分
    if(currentAddr & 0x03) {
        uint32_t wordAddr = currentAddr & ~0x03;
        uint32_t offset = currentAddr & 0x03;
        uint32_t bytesToWrite = 4 - offset;
        if(bytesToWrite > remain) bytesToWrite = remain;
        
        wordData = *(uint32_t*)wordAddr;
        
        for(uint32_t i = 0; i < bytesToWrite; i++) {
            ((uint8_t*)&wordData)[offset + i] = pData[dataIdx++];
        }
        
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, wordAddr, wordData) != HAL_OK) {
            HAL_FLASH_Lock();
            __enable_irq();
            return 1;
        }
        
        // 等待编程完成
        while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY));
        
        if(*(uint32_t*)wordAddr != wordData) {
            HAL_FLASH_Lock();
            __enable_irq();
            return 1;
        }
        
        currentAddr += bytesToWrite;
        remain -= bytesToWrite;
    }
    
    // 处理中间对齐部分
    uint32_t wordCount = remain / 4;
    for(uint32_t i = 0; i < wordCount; i++) {
        uint32_t addr = currentAddr + i * 4;
        wordData = *(uint32_t*)(pData + dataIdx);
        
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, wordData) != HAL_OK) {
            HAL_FLASH_Lock();
            __enable_irq();
            return 1;
        }
        
        while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY));
        
        if(*(uint32_t*)addr != wordData) {
            HAL_FLASH_Lock();
            __enable_irq();
            return 1;
        }
        
        dataIdx += 4;
        remain -= 4;
    }
    currentAddr += wordCount * 4;
    
    // 处理结尾未对齐部分
    if(remain > 0) {
        wordData = *(uint32_t*)currentAddr;
        
        for(uint32_t i = 0; i < remain; i++) {
            ((uint8_t*)&wordData)[i] = pData[dataIdx++];
        }
        
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, currentAddr, wordData) != HAL_OK) {
            HAL_FLASH_Lock();
            __enable_irq();
            return 1;
        }
        
        while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY));
        
        if(*(uint32_t*)currentAddr != wordData) {
            HAL_FLASH_Lock();
            __enable_irq();
            return 1;
        }
    }
    
    HAL_FLASH_Lock();
    
    // 清除一次标志位，为下次操作做准备
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                          FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
    
    __enable_irq();  // 恢复中断
    
    return 0;
}

// 连续读取多个32位数据
// StartAddress: 起始地址
// pData: 读取数据存储指针
// NumToRead: 要读取的数据个数
// 返回值: 0=成功, 1=失败
uint8_t FlashReadMulti(uint32_t StartAddress, uint32_t *pData, uint32_t NumToRead)
{
    // 检查地址对齐
    if(StartAddress & 0x03) return 1;
    
    // 检查地址范围
    if(StartAddress < 0x08000000 || StartAddress > 0x0807FFFF) return 1;
    
    // 检查结束地址是否超出范围
    uint32_t endAddr = StartAddress + (NumToRead * 4) - 1;
    if(endAddr > 0x0807FFFF) return 1;
    
    for(uint32_t i = 0; i < NumToRead; i++)
    {
        uint32_t addr = StartAddress + i * 4;
        pData[i] = *(uint32_t*)addr;
    }
    
    return 0;
}

