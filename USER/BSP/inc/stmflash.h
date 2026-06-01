#ifndef _STMFLASH_H
#define _STMFLASH_H

#include "main.h"

// 获取扇区编号
int GetSectorFromAddress(uint32_t address);

// 清除扇区
uint8_t FlashErase(uint32_t StartAddress);

// 写数据
uint8_t FlashWrite(uint32_t StartAddress, uint32_t data);

// 读数据
uint32_t FlashRead(uint32_t StartAddress);

// 连续写入多个32位数据
// StartAddress: 起始地址
// pData: 要写入的数据指针
// NumToWrite: 要写入的数据个数
// 返回值: 0=成功, 1=失败
uint8_t FlashWriteMulti(uint32_t StartAddress, uint32_t *pData, uint32_t NumToWrite);
uint8_t FlashWriteMulti_Byte(uint32_t StartAddress, uint8_t *pData, uint32_t NumToWrite);

// 连续读取多个32位数据
// StartAddress: 起始地址
// pData: 读取数据存储指针
// NumToRead: 要读取的数据个数
// 返回值: 0=成功, 1=失败
uint8_t FlashReadMulti(uint32_t StartAddress, uint32_t *pData, uint32_t NumToRead);


#endif
