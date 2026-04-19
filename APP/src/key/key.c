#include "key.h"


/*key初始化*/
Key_Structure Key_Init(GPIO_TypeDef* gpio, uint16_t pin, KEY_POLARITY polarity)
{
    Key_Structure key;

    key.GPIOx = gpio;
    key.GPIO_Pin = pin;
    key.polarity = polarity;

    return key;
}

/*获取按键电平状态*/
KEY_STATE Key_GetState(Key_Structure* key)
{
    KEY_STATE state = KEY_STATE_Release;
    // 低电平按下
    if(key->polarity == KEY_POLARITY_LOW)
    {
        state = (KEY_GET_STATE(key->GPIOx,key->GPIO_Pin) == GPIO_PIN_RESET)\
                ?KEY_STATE_Press:KEY_STATE_Release;
    }
    else
    {
        state = (KEY_GET_STATE(key->GPIOx,key->GPIO_Pin) == GPIO_PIN_SET)\
                ?KEY_STATE_Press:KEY_STATE_Release;
    }
    
    return state;
}



