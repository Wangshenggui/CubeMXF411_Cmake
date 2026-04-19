#ifndef _KEY_FSM_H_
#define _KEY_FSM_H_

#include "main.h"
#include "key.h"
#include "system.h"


/*KEY FSM状态枚举*/
typedef enum
{
    KEY_FSM_STATE_IDLE,                 // 空闲
    KEY_FSM_STATE_RELEASE,              // 松开
    KEY_FSM_STATE_RELEASE_DEBOUNCE,     // 松开消抖
    KEY_FSM_STATE_PRESS,                // 按下
    KEY_FSM_STATE_PRESS_DEBOUNCE,       // 按下消抖
    KEY_FSM_STATE_COUNT
} KEY_FSM_STATE;

/*定义点击回调函数类型*/
typedef void (*ClickHandle_t)(void);
/*KEY FSM结构体*/
typedef struct
{
    KEY_FSM_STATE state;                        // 按键状态
    uint32_t last_tick;                         // 上次记录时间
    Key_Structure* key;                         // 操作对象
    ClickHandle_t click_handle;                 // 按键单击回调函数
    KEY_STATE (*get_state)(Key_Structure*);     // 获取状态函数
} KEY_FSM_Structure;

/*按键状态机初始化*/
KEY_FSM_Structure KEY_SFM_Init(Key_Structure* key, ClickHandle_t click_handle);
/*按键状态机轮询*/
void KEY_FSM_Run(KEY_FSM_Structure* fsm, uint32_t tick);

#endif
