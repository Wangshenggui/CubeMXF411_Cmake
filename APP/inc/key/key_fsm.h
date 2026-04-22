#ifndef _KEY_FSM_H_
#define _KEY_FSM_H_

#include "main.h"
#include "key.h"
#include "system.h"

#define DEBOUNCE_TIME 10            // 消抖时间(ms)
#define DOUBLE_CLICK_TIMEOUT 350    // 双击超时(ms)
#define LONG_PRESS_TIME 1000         // 长按触发时间(ms)

/*KEY FSM状态枚举*/
typedef enum
{
    KEY_FSM_STATE_IDLE,                     // 空闲
    KEY_FSM_STATE_RELEASE,                  // 松开
    KEY_FSM_STATE_RELEASE_DEBOUNCE,         // 松开消抖
    KEY_FSM_STATE_PRESS,                    // 按下
    KEY_FSM_STATE_PRESS_DEBOUNCE,           // 按下消抖
    KEY_FSM_STATE_WAIT_DOUBLE_CLICK,        // 等待双击和单击超时检测
    KEY_FSM_STATE_DOUBLE_CLICK,             // 双击
    KEY_FSM_STATE_PRESS_HOLD,               // 长按检测（按下持续）
    KEY_FSM_STATE_LONG_PRESS,               // 长按触发
    KEY_FSM_STATE_WAIT_LONG_PRESS_RELEASE,  // 等待长按后的松开
    KEY_FSM_STATE_COUNT
} KEY_FSM_STATE;

/*定义点击回调函数类型*/
typedef void (*ClickHandle_t)(void);
/*定义双击回调函数类型*/
typedef void (*DoubleClickHandle_t)(void);
/*定义长按回调函数类型*/
typedef void (*LongPressHandle_t)(void);
/*KEY FSM结构体*/
typedef struct
{
    KEY_FSM_STATE state;                        // 按键状态
    uint32_t last_tick;                         // 上次记录时间
    Key_Structure* key;                         // 操作对象
    ClickHandle_t click_handle;                 // 按键单击回调函数
    uint8_t press_count;                        // 按下次数（用来检测双击）
    uint32_t double_click_start_tick;           // 双击开始计时
    DoubleClickHandle_t double_click_handle;    // 双击回调函数
    uint32_t press_hold_start_tick;             // 按下之后开始计时
    LongPressHandle_t long_press_handle;        // 长按回调函数
    KEY_STATE (*get_state)(Key_Structure*);     // 获取状态函数
} KEY_FSM_Structure;

/*按键状态机初始化*/
KEY_FSM_Structure KEY_FSM_Init(
    Key_Structure* key, 
    ClickHandle_t click_handle,
    DoubleClickHandle_t double_click_handle,
    LongPressHandle_t long_press_handle);
/*按键状态机轮询*/
void KEY_FSM_Run(KEY_FSM_Structure* fsm, uint32_t tick);

#endif
