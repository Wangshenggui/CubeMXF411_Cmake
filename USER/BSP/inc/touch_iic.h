#ifndef _TOUCH_IIC_H
#define _TOUCH_IIC_H

#include "main.h"



//IO方向设置
void TOUCH_SDA_OUT();
void TOUCH_SDA_IN();

//IO操作函数	 
#define TOUCH_IIC_SCL(x)	x?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET):\
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET)//SCL     
#define TOUCH_IIC_SDA(x)	x?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET):\
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET)//SDA	 
#define TOUCH_READ_SDA	HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7)//输入SDA 
 

//IIC所有操作函数
void TOUCH_IIC_Init(void);                	//初始化IIC的IO口				 
void TOUCH_IIC_Start(void);				//发送IIC开始信号
void TOUCH_IIC_Stop(void);	  				//发送IIC停止信号
void TOUCH_IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t TOUCH_IIC_Read_Byte(unsigned char ack);	//IIC读取一个字节
uint8_t TOUCH_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void TOUCH_IIC_Ack(void);					//IIC发送ACK信号
void TOUCH_IIC_NAck(void);					//IIC不发送ACK信号








#endif
