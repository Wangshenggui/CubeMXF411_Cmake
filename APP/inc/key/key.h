#ifndef _KEY_H_
#define _KEY_H_

#include "main.h"
#include <stdbool.h>

// GPIO操作接口
#define KEY_GET_STATE(port,pin) HAL_GPIO_ReadPin(port,pin)

/*KEY极性枚举*/
typedef enum
{
    KEY_POLARITY_LOW,   // 低电平表示按下
    KEY_POLARITY_HIGH,  // 高电平表示按下
} KEY_POLARITY;

/*KEY状态枚举*/
typedef enum
{
    KEY_STATE_Release,
    KEY_STATE_Press
} KEY_STATE;

/*key结构体*/
typedef struct
{
    GPIO_TypeDef* GPIOx;        // KEY端口
    uint16_t GPIO_Pin;          // KEY引脚
    KEY_POLARITY polarity;      // KEY极性
} Key_Structure;

/*key初始化*/
Key_Structure Key_Init(GPIO_TypeDef* gpio, uint16_t pin, KEY_POLARITY polarity);
/*获取按键电平状态*/
KEY_STATE Key_GetState(Key_Structure* key);

#endif
