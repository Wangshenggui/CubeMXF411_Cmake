#ifndef _SPI_LCD_H
#define _SPI_LCD_H

#include "main.h"
#include "spi.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef u8
#define u8 uint8_t
#endif

#ifndef u16
#define u16 uint16_t
#endif


// #define LCD_SCLK_Clr() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET)//SCL
// #define LCD_SCLK_Set() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET)

// #define LCD_SDIN_Clr() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET)//DIN
// #define LCD_SDIN_Set() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET)

#define LCD_CS_Clr()  HAL_GPIO_WritePin(SPI_LCD_CS_GPIO_Port,SPI_LCD_CS_Pin,GPIO_PIN_RESET)//CS
#define LCD_CS_Set()  HAL_GPIO_WritePin(SPI_LCD_CS_GPIO_Port,SPI_LCD_CS_Pin,GPIO_PIN_RESET)

#define LCD_RST_Clr() HAL_GPIO_WritePin(SPI_LCD_RESET_GPIO_Port,SPI_LCD_RESET_Pin,GPIO_PIN_RESET)//RES
#define LCD_RST_Set() HAL_GPIO_WritePin(SPI_LCD_RESET_GPIO_Port,SPI_LCD_RESET_Pin,GPIO_PIN_SET)

#define LCD_DC_Clr() HAL_GPIO_WritePin(SPI_LCD_DC_GPIO_Port,SPI_LCD_DC_Pin,GPIO_PIN_RESET)//DC
#define LCD_DC_Set() HAL_GPIO_WritePin(SPI_LCD_DC_GPIO_Port,SPI_LCD_DC_Pin,GPIO_PIN_SET)

#define LCD_BLK_Set()  HAL_GPIO_WritePin(SPI_LCD_BLC_GPIO_Port,SPI_LCD_BLC_Pin,GPIO_PIN_SET)


void Lcd_Init(void);
void LCD_Clear(u16 Color);
// void LCD_Clear(u8 x0,u8 y0,u8 x1,u8 y1,u16 Color);
void LCD_Fill(u8 x0,u8 y0,u8 x1,u8 y1,u16 Color);
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2);

void SendByte(uint8_t *dat);

//������ɫ
#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define BRED 0XF81F
#define GRED 0XFFE0
#define GBLUE 0X07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define GREEN 0x07E0
#define CYAN 0x7FFF
#define YELLOW 0xFFE0
#define BROWN 0XBC40 //��ɫ
#define BRRED 0XFC07 //�غ�ɫ
#define GRAY 0X8430  //��ɫ
// GUI��ɫ

#define DARKBLUE 0X01CF  //����ɫ
#define LIGHTBLUE 0X7D7C //ǳ��ɫ
#define GRAYBLUE 0X5458  //����ɫ
//������ɫΪPANEL����ɫ

#define LIGHTGREEN 0X841F //ǳ��ɫ
#define LGRAY 0XC618      //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE 0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE 0X2B12    //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)



#define USE_HORIZONTAL 0 //���ú�������������ʾ 0��1Ϊ���� 2��3Ϊ����

#if USE_HORIZONTAL == 0 || USE_HORIZONTAL == 1
#define LCD_W 240
#define LCD_H 240

#else
#define LCD_W 240
#define LCD_H 240
#endif


#ifdef __cplusplus
}
#endif

#endif
