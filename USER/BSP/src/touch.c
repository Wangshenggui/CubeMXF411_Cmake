#include "touch.h"



//向CST816写入一次数据
// reg:起始寄存器地址
// buf:数据缓缓存区
// len:写数据长度
//返回值:0,成功;1,失败.
uint8_t CST816_WR_Reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
	uint8_t i;
	uint8_t ret = 0;
	TOUCH_IIC_Start();
	TOUCH_IIC_Send_Byte(FT_CMD_WR); //发送写命令
	TOUCH_IIC_Ack();
	TOUCH_IIC_Send_Byte(reg & 0XFF); //发送低8位地址
	TOUCH_IIC_Ack();
	for (i = 0; i < len; i++)
	{
		TOUCH_IIC_Send_Byte(buf[i]); //发数据
		ret = TOUCH_IIC_Wait_Ack();
		if (ret)
			break;
	}
	TOUCH_IIC_Stop(); //产生一个停止条件
	return ret;
}

void CST816_Init(void)
{
	uint8_t temp;
	
	TOUCH_IIC_Init();
	
	CST816_RST(0);
	HAL_Delay(50);
	CST816_RST(1);
	HAL_Delay(100);
	TOUCH_IIC_SDA(1);
	TOUCH_IIC_SCL(1);
	HAL_Delay(10);
	temp = 0;
	CST816_WR_Reg(FT_DEVIDE_MODE, &temp, 1);  //进入正常操作模式
	temp = 21;								  //触摸有效值，22，越小越灵敏
	CST816_WR_Reg(FT_ID_G_THGROUP, &temp, 1); //设置触摸有效值
	temp = 12;								  //激活周期，不能小于12，最大14
	CST816_WR_Reg(FT_ID_G_PERIODACTIVE, &temp, 1);
}

//从CST816读出一次数据
// reg:起始寄存器地址
// buf:数据缓缓存区
// len:读数据长度
void CST816_RD_Reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
	uint8_t i;
	TOUCH_IIC_Start();
	TOUCH_IIC_Send_Byte(FT_CMD_WR); //发送写命令
	TOUCH_IIC_Ack();
	TOUCH_IIC_Send_Byte(reg & 0XFF); //发送低8位地址
	TOUCH_IIC_Ack();
	TOUCH_IIC_Start();
	TOUCH_IIC_Send_Byte(FT_CMD_RD); //发送读命令
	TOUCH_IIC_Ack();
	for (i = 0; i < len; i++)
	{
		*buf++ = TOUCH_IIC_Read_Byte(1); //读入1B数据到接收数据缓冲区中
	}
	TOUCH_IIC_NAck(); // n个字节读完,发送非应答位
	TOUCH_IIC_Stop();	  //产生一个停止条件
}

const uint16_t CST816_TPX_TBL[5] =
{
	FT_TP1_REG,
	FT_TP2_REG,
	FT_TP3_REG,
	FT_TP4_REG,
	FT_TP5_REG
};
TouchPointRefTypeDef TPR_Structure;
void CST816_Scan(void)
{
	uint8_t i = 0;
	uint8_t sta = 0;
	uint8_t buf[6] = {0};
	CST816_RD_Reg(0x02, &sta, 1); //读取触摸点的状态

	if (sta & 0x0f) //判断是否有触摸点按下，0x02寄存器的低4位表示有效触点个数
	{
		TPR_Structure.TouchSta = ~(0xFF << (sta & 0x0F)); //~(0xFF << (sta & 0x0F))将点的个数转换为触摸点按下有效标志
		for (i = 0; i < 1; i++)							  //分别判断触摸点1-5是否被按下
		{
			if (TPR_Structure.TouchSta & (1 << i))		  //读取触摸点坐标
			{											  //被按下则读取对应触摸点坐标数据
				CST816_RD_Reg(CST816_TPX_TBL[i], buf, 6); //读取XY坐标值
				// printf("0--%x\r\n",buf[0]);
				// printf("1--%x\r\n",buf[1]);
				// printf("2--%x\r\n",buf[2]);
				// printf("3--%x\r\n",buf[3]);
				// printf("4--%x\r\n",buf[4]);
				// printf("5--%x\r\n",buf[5]);
				TPR_Structure.x[i] = buf[2];
				TPR_Structure.y[i] = buf[4];
				//TPR_Structure.x[i]=((uint16_t)(buf[1]&0X0F)<<8)+buf[2];
				//TPR_Structure.y[i]=((uint16_t)(buf[3]&0X0F)<<8)+buf[4];
				if ((buf[1] & 0XC0) != 0X80)
				{
					TPR_Structure.x[i] = TPR_Structure.y[i] = 0; //必须是contact事件，才认为有效
					return;
				}
			}
		}
		TPR_Structure.TouchSta |= TP_PRES_DOWN; //触摸按下标记
	}
	
	else
	{
		if (TPR_Structure.TouchSta & TP_PRES_DOWN) //之前是被按下的
			TPR_Structure.TouchSta &= ~0x80;	   //触摸松开标记
		else
		{
			TPR_Structure.x[0] = 0;
			TPR_Structure.y[0] = 0;
			TPR_Structure.TouchSta &= 0xe0; //清楚触摸点有效标记
		}
	}
}





