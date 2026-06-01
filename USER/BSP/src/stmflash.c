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
    
    // 参数检查
    if(pData == NULL) return 1;
    if(StartAddress < 0x08000000 || StartAddress > 0x0807FFFF) return 1;
    
    uint32_t endAddr = StartAddress + NumToWrite - 1;
    if(endAddr > 0x0807FFFF) return 1;
    
    // 检查是否需要擦除
    int startSector = GetSectorFromAddress(StartAddress);
    int endSector = GetSectorFromAddress(endAddr);
    if(startSector < 0 || endSector < 0) return 1;
    
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
        
        for(int sector = startSector; sector <= endSector; sector++) {
            __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
            
            FLASH_Erase_Sector(sector, FLASH_VOLTAGE_RANGE_3);
            
            if(HAL_FLASH_GetError() != HAL_FLASH_ERROR_NONE) {
                HAL_FLASH_Lock();
                return 1;
            }
        }
        
        HAL_FLASH_Lock();
        
        // 验证擦除
        for(uint32_t addr = checkAddr; addr <= checkEnd; addr += 4) {
            if(*(uint32_t*)addr != 0xFFFFFFFF) {
                return 1;
            }
        }
    }
    
    // 写入数据
    HAL_FLASH_Unlock();
    
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
            return 1;
        }
        
        if(*(uint32_t*)wordAddr != wordData) {
            HAL_FLASH_Lock();
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
            return 1;
        }
        
        if(*(uint32_t*)addr != wordData) {
            HAL_FLASH_Lock();
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
            return 1;
        }
        
        if(*(uint32_t*)currentAddr != wordData) {
            HAL_FLASH_Lock();
            return 1;
        }
    }
    
    HAL_FLASH_Lock();
    
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

// void Test_FlashWrite64KB_Static(void)
// {
//     // 静态分配（注意：64KB会占用较多RAM，可能需要修改堆栈大小）
//     #define STATIC_WORD_COUNT 14000  // 注意：14000字 = 56KB，不是64KB
//     static uint32_t writeBuffer[STATIC_WORD_COUNT];
//     static uint32_t readBuffer[STATIC_WORD_COUNT];
    
//     uint32_t startAddr = 0x08070000;
//     uint32_t i;
//     uint8_t result;
//     uint32_t startTime, endTime;
//     uint32_t eraseStartTime, eraseEndTime;
    
//     debug_info("\r\n");
//     debug_info("========================================\r\n");
//     debug_info("    64KB Flash 测试（静态内存）\r\n");
//     debug_info("========================================\r\n");
    
//     // 打印测试配置
//     debug_info("测试配置:\r\n");
//     debug_info("  起始地址: 0x%08lX\r\n", startAddr);
//     debug_info("  数据字数: %lu\r\n", (unsigned long)STATIC_WORD_COUNT);
//     debug_info("  数据大小: %lu 字节 (%.2f KB)\r\n", (unsigned long)(STATIC_WORD_COUNT * 4), (float)(STATIC_WORD_COUNT * 4) / 1024);
//     debug_info("  writeBuffer地址: 0x%08lX\r\n", (unsigned long)writeBuffer);
//     debug_info("  readBuffer地址: 0x%08lX\r\n", (unsigned long)readBuffer);
//     debug_info("  Buffer总占用RAM: %lu 字节 (%.2f KB)\r\n", (unsigned long)(STATIC_WORD_COUNT * 8), (float)(STATIC_WORD_COUNT * 8) / 1024);
    
//     // 检查地址对齐
//     if(startAddr & 0x03) {
//         debug_info("[错误] 起始地址未4字节对齐！\r\n");
//         return;
//     }
    
//     // 检查地址范围
//     uint32_t endAddr = startAddr + STATIC_WORD_COUNT * 4 - 1;
//     debug_info("  地址范围: 0x%08lX - 0x%08lX\r\n", startAddr, endAddr);
//     if(endAddr > 0x0807FFFF) {
//         debug_info("[错误] 地址超出Flash范围！\r\n");
//         return;
//     }
    
//     // 检查扇区信息
//     int startSector = GetSectorFromAddress(startAddr);
//     int endSector = GetSectorFromAddress(endAddr);
//     debug_info("  起始扇区: %d\r\n", startSector);
//     debug_info("  结束扇区: %d\r\n", endSector);
    
//     // 准备数据（简单模式）
//     debug_info("\r\n准备测试数据...\r\n");
//     startTime = HAL_GetTick();
//     for(i = 0; i < STATIC_WORD_COUNT; i++) {
//         writeBuffer[i] = i;  // 递增模式
//         if(i < 10) {
//             debug_info("  writeBuffer[%lu] = 0x%08lX\r\n", (unsigned long)i, writeBuffer[i]);
//         }
//     }
//     endTime = HAL_GetTick();
//     debug_info("数据准备完成，耗时: %lu ms\r\n", endTime - startTime);
    
//     // 检查目标地址当前内容
//     debug_info("\r\n检查目标地址当前内容...\r\n");
//     uint32_t nonErasedCount = 0;
//     for(i = 0; i < (STATIC_WORD_COUNT > 100 ? 100 : STATIC_WORD_COUNT); i++) {
//         uint32_t addr = startAddr + i * 4;
//         uint32_t currentVal = *(uint32_t*)addr;
//         if(currentVal != 0xFFFFFFFF) {
//             nonErasedCount++;
//             if(nonErasedCount <= 10) {
//                 debug_info("  地址 0x%08lX 当前值: 0x%08lX (未擦除状态)\r\n", addr, currentVal);
//             }
//         }
//     }
//     if(nonErasedCount > 0) {
//         debug_info("  发现 %lu 个地址未处于擦除状态（需要先擦除）\r\n", (unsigned long)nonErasedCount);
//     } else {
//         debug_info("  所有检查的地址均处于擦除状态(0xFFFFFFFF)\r\n");
//     }
    
//     // 擦除
//     debug_info("\r\n开始擦除Flash...\r\n");
//     eraseStartTime = HAL_GetTick();
//     result = FlashErase(startAddr);
//     eraseEndTime = HAL_GetTick();
    
//     if(result != 0) {
//         debug_info("[错误] Flash擦除失败！耗时: %lu ms\r\n", eraseEndTime - eraseStartTime);
//         // 获取详细错误信息
//         uint32_t errorCode = HAL_FLASH_GetError();
//         debug_info("  HAL Flash错误码: 0x%08lX\r\n", errorCode);
//         // if(errorCode & HAL_FLASH_ERROR_PROGRAM) debug_info("    - 编程错误\r\n");
//         if(errorCode & HAL_FLASH_ERROR_WRP) debug_info("    - 写保护错误\r\n");
//         // if(errorCode & HAL_FLASH_ERROR_OPTV) debug_info("    - 选项字节错误\r\n");
//         return;
//     }
//     debug_info("Flash擦除成功，耗时: %lu ms\r\n", eraseEndTime - eraseStartTime);
    
//     // 擦除后验证
//     debug_info("\r\n擦除后验证...\r\n");
//     uint32_t erasedFailCount = 0;
//     for(i = 0; i < (STATIC_WORD_COUNT > 100 ? 100 : STATIC_WORD_COUNT); i++) {
//         uint32_t addr = startAddr + i * 4;
//         uint32_t val = *(uint32_t*)addr;
//         if(val != 0xFFFFFFFF) {
//             erasedFailCount++;
//             if(erasedFailCount <= 10) {
//                 debug_info("  [警告] 地址 0x%08lX 擦除后值为 0x%08lX (应为0xFFFFFFFF)\r\n", addr, val);
//             }
//         }
//     }
//     if(erasedFailCount == 0) {
//         debug_info("  擦除验证通过\r\n");
//     } else {
//         debug_info("  [警告] 有 %lu 个地址擦除不彻底\r\n", (unsigned long)erasedFailCount);
//     }
    
//     // 写入
//     debug_info("\r\n开始写入Flash...\r\n");
//     startTime = HAL_GetTick();
//     result = FlashWriteMulti(startAddr, writeBuffer, STATIC_WORD_COUNT);
//     endTime = HAL_GetTick();
    
//     if(result != 0) {
//         debug_info("[错误] 写入失败！耗时: %lu ms\r\n", endTime - startTime);
//         uint32_t errorCode = HAL_FLASH_GetError();
//         debug_info("  HAL Flash错误码: 0x%08lX\r\n", errorCode);
//         return;
//     }
//     debug_info("写入成功，耗时: %lu ms\r\n", endTime - startTime);
//     debug_info("  平均写入速度: %.2f KB/s\r\n", (float)(STATIC_WORD_COUNT * 4) / (endTime - startTime) * 1000 / 1024);
    
//     // 写入后即时验证（读取刚写入的数据）
//     debug_info("\r\n写入后即时验证...\r\n");
//     uint32_t immediateErrorCount = 0;
//     for(i = 0; i < (STATIC_WORD_COUNT > 50 ? 50 : STATIC_WORD_COUNT); i++) {
//         uint32_t addr = startAddr + i * 4;
//         uint32_t readVal = *(uint32_t*)addr;
//         if(readVal != writeBuffer[i]) {
//             immediateErrorCount++;
//             if(immediateErrorCount <= 10) {
//                 debug_info("  [错误] 地址 0x%08lX: 写入0x%08lX, 读回0x%08lX\r\n", 
//                        addr, writeBuffer[i], readVal);
//             }
//         }
//     }
//     if(immediateErrorCount == 0) {
//         debug_info("  即时验证通过（前%lu个字）\r\n", (unsigned long)(STATIC_WORD_COUNT > 50 ? 50 : STATIC_WORD_COUNT));
//     } else {
//         debug_info("  [错误] 发现 %lu 个即时错误\r\n", (unsigned long)immediateErrorCount);
//     }
    
//     // 读取
//     debug_info("\r\n开始批量读取Flash...\r\n");
//     startTime = HAL_GetTick();
//     result = FlashReadMulti(startAddr, readBuffer, STATIC_WORD_COUNT);
//     endTime = HAL_GetTick();
    
//     if(result != 0) {
//         debug_info("[错误] 读取失败！耗时: %lu ms\r\n", endTime - startTime);
//         return;
//     }
//     debug_info("读取成功，耗时: %lu ms\r\n", endTime - startTime);
//     debug_info("  平均读取速度: %.2f MB/s\r\n", (float)(STATIC_WORD_COUNT * 4) / (endTime - startTime) / 1024);
    
//     // 显示前10个读取的数据
//     debug_info("\r\n前10个读取的数据:\r\n");
//     for(i = 0; i < 10 && i < STATIC_WORD_COUNT; i++) {
//         debug_info("  readBuffer[%lu] = 0x%08lX\r\n", (unsigned long)i, readBuffer[i]);
//     }
    
//     // 完整验证
//     debug_info("\r\n开始完整数据验证...\r\n");
//     uint32_t errorCount = 0;
//     uint32_t firstErrorAddr = 0;
//     uint32_t firstErrorExpected = 0;
//     uint32_t firstErrorActual = 0;
    
//     startTime = HAL_GetTick();
//     for(i = 0; i < STATIC_WORD_COUNT; i++) {
//         if(writeBuffer[i] != readBuffer[i]) {
//             errorCount++;
//             if(errorCount == 1) {
//                 firstErrorAddr = startAddr + i * 4;
//                 firstErrorExpected = writeBuffer[i];
//                 firstErrorActual = readBuffer[i];
//             }
//             if(errorCount <= 20) {
//                 debug_info("  错误 #%lu @0x%08lX: 期望=0x%08lX, 实际=0x%08lX\r\n", 
//                        (unsigned long)errorCount, startAddr + i*4, writeBuffer[i], readBuffer[i]);
//             } else if(errorCount == 21) {
//                 debug_info("  ... (后续错误省略)\r\n");
//             }
//         }
//     }
//     endTime = HAL_GetTick();
//     debug_info("验证耗时: %lu ms\r\n", endTime - startTime);
    
//     // 输出测试结果
//     debug_info("\r\n");
//     debug_info("========================================\r\n");
//     debug_info("            测试结果汇总\r\n");
//     debug_info("========================================\r\n");
//     debug_info("  数据总量: %lu 字节 (%.2f KB)\r\n", (unsigned long)(STATIC_WORD_COUNT * 4), (float)(STATIC_WORD_COUNT * 4) / 1024);
//     debug_info("  擦除耗时: %lu ms\r\n", eraseEndTime - eraseStartTime);
//     debug_info("  写入耗时: %lu ms\r\n", endTime - startTime);
//     debug_info("  读取耗时: %lu ms\r\n", endTime - startTime);
//     debug_info("  错误数量: %lu\r\n", (unsigned long)errorCount);
    
//     if(errorCount == 0) {
//         debug_info("\r\n");
//         debug_info("╔═══════════════════════════════════════╗\r\n");
//         debug_info("║  ✓✓✓ 64KB数据验证完全正确！ ✓✓✓  ║\r\n");
//         debug_info("╚═══════════════════════════════════════╝\r\n");
//     } else {
//         debug_info("\r\n");
//         debug_info("╔═══════════════════════════════════════╗\r\n");
//         debug_info("║  ✗✗✗ 测试失败，发现 %lu 个错误！ ✗✗✗  ║\r\n", (unsigned long)errorCount);
//         debug_info("╚═══════════════════════════════════════╝\r\n");
//         debug_info("  第一个错误地址: 0x%08lX\r\n", firstErrorAddr);
//         debug_info("  期望值: 0x%08lX\r\n", firstErrorExpected);
//         debug_info("  实际值: 0x%08lX\r\n", firstErrorActual);
//     }
// }

