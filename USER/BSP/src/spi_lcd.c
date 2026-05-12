#include "spi_lcd.h"


/******************************************************************************
	  ����˵����LCD��������д�뺯��
	  ������ݣ�dat  Ҫд��Ĵ�������
	  ����ֵ��  ��
******************************************************************************/
void LCD_Writ_Bus(u8 dat)
{
	u8 i;
	LCD_CS_Clr();
//	for (i = 0; i < 8; i++)
//	{
//		LCD_SCLK_Clr();
//		if (dat & 0x80)
//			LCD_SDIN_Set();
//		else
//			LCD_SDIN_Clr();
//		LCD_SCLK_Set();
//		dat <<= 1;
//	}
	
	// while(HAL_SPI_Transmit_DMA(&hspi1,&dat,1) != HAL_OK)
	HAL_SPI_Transmit_DMA(&hspi1,&dat,1);
//	SPI1->DR=dat;
	LCD_CS_Set();
}
/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
	LCD_CS_Clr();
	LCD_DC_Clr();//д����
	LCD_Writ_Bus(dat);
	
	LCD_CS_Set();
}

/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
	LCD_CS_Clr();
	LCD_DC_Set();//д����
	LCD_Writ_Bus(dat);
	
	LCD_CS_Set();
}


void Lcd_Init(void)
{
	LCD_CS_Set();
	LCD_RST_Set();
	LCD_DC_Set();
	
	LCD_RST_Clr();
	HAL_Delay(20);
	LCD_RST_Set();
	HAL_Delay(20);
	LCD_BLK_Set();
	
	//GC9A01 +HSD1.09 init code /////////////
	LCD_WR_REG(0xFE);
	LCD_WR_REG(0xEF);
	LCD_WR_REG(0xEB);
	LCD_WR_DATA8(0x14);
	LCD_WR_REG(0x84);
	LCD_WR_DATA8(0x40);
	LCD_WR_REG(0x88);
	LCD_WR_DATA8(0x0A);
	LCD_WR_REG(0x89);
	LCD_WR_DATA8(0x21);
	LCD_WR_REG(0x8A);
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0x8B);
	LCD_WR_DATA8(0x80);
	LCD_WR_REG(0x8C);
	LCD_WR_DATA8(0x01);
	LCD_WR_REG(0x8D);
	LCD_WR_DATA8(0x03);
	LCD_WR_REG(0x8F);
	LCD_WR_DATA8(0xFF);
	LCD_WR_REG(0xB6);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x60);
	LCD_WR_REG(0x36);
	LCD_WR_DATA8(0x88);
	LCD_WR_REG(0x3A);
	LCD_WR_DATA8(0x05);
	LCD_WR_REG(0x90);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x08);
	LCD_WR_REG(0xBD);
	LCD_WR_DATA8(0x06);
	LCD_WR_REG(0xBC);
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0xFF);
	LCD_WR_DATA8(0x60);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0x04);
	LCD_WR_REG(0xC3);
	LCD_WR_DATA8(0x2F);
	LCD_WR_REG(0xC4);
	LCD_WR_DATA8(0x2F);
	LCD_WR_REG(0xC9);
	LCD_WR_DATA8(0x25);
	LCD_WR_REG(0xBE);
	LCD_WR_DATA8(0x11);
	LCD_WR_REG(0xE1);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x0E);
	LCD_WR_REG(0xDF);
	LCD_WR_DATA8(0x21);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x02);
	LCD_WR_REG(0xF0);
	LCD_WR_DATA8(0x49);
	LCD_WR_DATA8(0x0e);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x25);
	LCD_WR_DATA8(0x2e);
	LCD_WR_REG(0xF1);
	LCD_WR_DATA8(0x44);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x73);
	LCD_WR_DATA8(0x2F);
	LCD_WR_DATA8(0x30);
	LCD_WR_DATA8(0x6F);
	LCD_WR_REG(0xF2);
	LCD_WR_DATA8(0x49);
	LCD_WR_DATA8(0x0e);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x25);
	LCD_WR_DATA8(0x2e);
	LCD_WR_REG(0xF3);
	LCD_WR_DATA8(0x44);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x73);
	LCD_WR_DATA8(0x2F);
	LCD_WR_DATA8(0x30);
	LCD_WR_DATA8(0x6F);
	LCD_WR_REG(0xED);
	LCD_WR_DATA8(0x1B);
	LCD_WR_DATA8(0x8B);
	LCD_WR_REG(0xAE);
	LCD_WR_DATA8(0x77);
	LCD_WR_REG(0xCD);
	LCD_WR_DATA8(0x63);
	LCD_WR_REG(0xAC);
	LCD_WR_DATA8(0x27);
	LCD_WR_REG(0x70);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x06);//VGH
	LCD_WR_DATA8(0x0F); //VGL
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x03);
	LCD_WR_REG(0xE8);
	LCD_WR_DATA8(0x24);
	LCD_WR_REG(0x60);
	LCD_WR_DATA8(0x38);
	LCD_WR_DATA8(0x0B);
	LCD_WR_DATA8(0x6D);
	LCD_WR_DATA8(0x6D);
	LCD_WR_DATA8(0x39);
	LCD_WR_DATA8(0xF0);
	LCD_WR_DATA8(0x6D);
	LCD_WR_DATA8(0x6D);
	LCD_WR_REG(0x61);
	LCD_WR_DATA8(0x38);
	LCD_WR_DATA8(0xF4);
	LCD_WR_DATA8(0x6D);
	LCD_WR_DATA8(0x6D);
	LCD_WR_DATA8(0x38);
	LCD_WR_DATA8(0xF7);
	LCD_WR_DATA8(0x6D);
	LCD_WR_DATA8(0x6D);/////////////////////////////////
	LCD_WR_REG(0x62);
	LCD_WR_DATA8(0x38);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x71);
	LCD_WR_DATA8(0xED);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x38);
	LCD_WR_DATA8(0x0F);
	LCD_WR_DATA8(0x71);
	LCD_WR_DATA8(0xEF);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x70);
	LCD_WR_REG(0x63);
	LCD_WR_DATA8(0x38);
	LCD_WR_DATA8(0x11);
	LCD_WR_DATA8(0x71);
	LCD_WR_DATA8(0xF1);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x38);
	LCD_WR_DATA8(0x13);
	LCD_WR_DATA8(0x71);
	LCD_WR_DATA8(0xF3);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x70);
	LCD_WR_REG(0x64);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x29);
	LCD_WR_DATA8(0xF1);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0xF1);
	LCD_WR_DATA8(0x00);//
	LCD_WR_DATA8(0x1a);//
	LCD_WR_REG(0x66);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x98);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x32);
	LCD_WR_DATA8(0x45);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0x67);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x54);
	LCD_WR_DATA8(0x67);
	LCD_WR_DATA8(0x45);
	LCD_WR_DATA8(0xcd);
	LCD_WR_REG(0x74);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x85); //85
	LCD_WR_DATA8(0x80);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x4E);
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0x98);
	LCD_WR_DATA8(0x3e);
	LCD_WR_DATA8(0x07);
	LCD_WR_REG(0x99);
	LCD_WR_DATA8(0x3e);
	LCD_WR_DATA8(0x07);
	LCD_WR_REG(0x35);
	LCD_WR_REG(0x21);
	HAL_Delay(120);
	//--------end gamma setting--------------//
	LCD_WR_REG(0x11);
	HAL_Delay(320);
	LCD_WR_REG(0x29);
	HAL_Delay(120);
	LCD_WR_REG(0x2C);

	LCD_BLK_Set();

	LCD_Clear(BLACK);
}

/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
	uint8_t buff[2];
	LCD_CS_Clr();
	LCD_DC_Set();//д����
	
	buff[0]=dat>>8;
	buff[1]=dat;
	HAL_SPI_Transmit_DMA(&hspi1,buff,2);
	
	LCD_CS_Set();
}

/******************************************************************************
	  ����˵����������ʼ�ͽ�����ַ
	  ������ݣ�x1,x2 �����е���ʼ�ͽ�����ַ
				y1,y2 �����е���ʼ�ͽ�����ַ
	  ����ֵ��  ��
******************************************************************************/
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_WR_REG(0x2a); //�е�ַ����
	LCD_WR_DATA(x1);
	LCD_WR_DATA(x2);
	LCD_WR_REG(0x2b); //�е�ַ����
	LCD_WR_DATA(y1);
	LCD_WR_DATA(y2);
	LCD_WR_REG(0x2c); //������д
}

/******************************************************************************
	  ����˵����LCD��������
	  ������ݣ���
	  ����ֵ��  ��
******************************************************************************/

static volatile uint8_t spi_tx_complete = 1;
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	spi_tx_complete = 1;
}

// 定义分块大小（根据可用RAM调整）
#define BLOCK_HEIGHT 120	// 240行数的整数倍数，便于整除
static uint16_t block_buffer[LCD_W * BLOCK_HEIGHT];

void LCD_Clear(u16 Color)
{
    uint32_t block_pixels = LCD_W * BLOCK_HEIGHT;
    
    // 填充缓冲区
    for(uint32_t i = 0; i < block_pixels; i++)
        block_buffer[i] = Color;
    
    // 设置地址（整个屏幕）
    LCD_Address_Set(0, 0, LCD_W - 1, LCD_H - 1);
    LCD_CS_Clr();
    LCD_DC_Set();
    
    // 240x240 只需要2个完整块（120+120行）
    uint16_t blocks = LCD_H / BLOCK_HEIGHT;  // = 2
    
    for(uint16_t block = 0; block < blocks; block++)
    {
        // 等待上一块完成
        while(spi_tx_complete != 1);
        
        spi_tx_complete = 0;
        HAL_SPI_Transmit_DMA(&hspi1, (uint8_t*)block_buffer, 
                            LCD_W * BLOCK_HEIGHT * 2);
    }
    
    LCD_CS_Set();
}

void LCD_Fill(u8 x0,u8 y0,u8 x1,u8 y1,u16 Color)
{
    uint16_t width  = x1 - x0 + 1;
    uint16_t height = y1 - y0 + 1;

    uint32_t block_pixels = width * BLOCK_HEIGHT;

    // 填充 block buffer
    for(uint32_t i = 0; i < block_pixels; i++)
        block_buffer[i] = Color;

    LCD_Address_Set(x0, y0, x1, y1);

    LCD_CS_Clr();
    LCD_DC_Set();

    uint16_t full_blocks = height / BLOCK_HEIGHT;
    uint16_t remain_rows = height % BLOCK_HEIGHT;

    // 发送完整块
    for(uint16_t b = 0; b < full_blocks; b++)
    {
        while(spi_tx_complete != 1);

        spi_tx_complete = 0;

        HAL_SPI_Transmit_DMA(
            &hspi1,
            (uint8_t*)block_buffer,
            width * BLOCK_HEIGHT * 2
        );
    }

    // 发送剩余行
    if(remain_rows)
    {
        while(spi_tx_complete != 1);

        spi_tx_complete = 0;

        HAL_SPI_Transmit_DMA(
            &hspi1,
            (uint8_t*)block_buffer,
            width * remain_rows * 2
        );
    }

    LCD_CS_Set();
}

