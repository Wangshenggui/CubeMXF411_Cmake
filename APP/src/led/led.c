#include "led.h"

// 定义LED1结构体
LED_Structure led1_struct = 
{
    led_blue_GPIO_Port,              // LED端口
    led_blue_Pin,                    // LED引脚
    LED_POLARITY_LOW           // 低电平点亮
};

// 定义LED2结构体
LED_Structure led2_struct = 
{
    led2_GPIO_Port,              // LED端口
    led2_Pin,                    // LED引脚
    LED_POLARITY_LOW           // 低电平点亮
};

/*设置LED状态*/
void LED_SetState(LED_Structure* led, LED_STATE state)
{
    GPIO_PinState PinState;

    if(led == NULL) return;
    
    // 判断极性
    if(led->polarity == LED_POLARITY_LOW)
    {
        PinState = (state == LED_STATE_ON) ? GPIO_PIN_RESET : GPIO_PIN_SET;
    }
    else
    {
        PinState = (state == LED_STATE_ON) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    }
    
    LED_SET_STATE(led->GPIOx, led->GPIO_Pin, PinState);
}

/*LED闪烁*/
void LED_Toggle(LED_Structure* led)
{
    LED_TOGGLE_STATE(led->GPIOx,led->GPIO_Pin);
}

