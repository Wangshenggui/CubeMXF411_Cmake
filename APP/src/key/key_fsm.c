#include "key_fsm.h"

/*按键状态机初始化*/
KEY_FSM_Structure KEY_FSM_Init(
    Key_Structure* key, 
    ClickHandle_t click_handle,
    DoubleClickHandle_t double_click_handle,
    LongPressHandle_t long_press_handle)
{
    KEY_FSM_Structure fsm;

    fsm.state = KEY_FSM_STATE_IDLE;
    fsm.last_tick = 0;
    fsm.click_handle = click_handle;
    fsm.key = key;                      // 指向操作对象
    fsm.get_state = Key_GetState;       // 指向获取状态函数

    // 双击附加
    fsm.press_count = 0;
    fsm.double_click_start_tick = 0;
    fsm.double_click_handle = double_click_handle;

    // 长按附加
    fsm.press_hold_start_tick = 0;
    fsm.long_press_handle = long_press_handle;

    return fsm;
}

/*按键状态机轮询*/
void KEY_FSM_Run(KEY_FSM_Structure* fsm, uint32_t tick)
{
    switch(fsm->state)
    {
        // 空闲
        case(KEY_FSM_STATE_IDLE):
        {
            // 检测到按下
            if(fsm->get_state(fsm->key) == KEY_STATE_Press)
            {
                fsm->last_tick = tick;                      // 记录按下时时间
                fsm->state = KEY_FSM_STATE_PRESS_DEBOUNCE;  // 切换到按下消抖状态
            }
        }
        break;

        // 按下消抖
        case(KEY_FSM_STATE_PRESS_DEBOUNCE):
        {
            // 检测到按下
            if(fsm->get_state(fsm->key) == KEY_STATE_Press)
            {
                // 持续按下超过DEBOUNCE_TIME，确认按下
                if(get_tick_diff(tick, fsm->last_tick) > DEBOUNCE_TIME)
                {
                    fsm->state = KEY_FSM_STATE_PRESS;  // 切换到按下状态
                }
            }
            else    // 误触
            {
                fsm->state = KEY_FSM_STATE_IDLE;    // 回到空闲
            }
        }
        break;

        // 按下
        case(KEY_FSM_STATE_PRESS):
        {
            fsm->press_hold_start_tick = tick;   // 记录按下时的时间
            // 切换到按下检测
            fsm->state = KEY_FSM_STATE_PRESS_HOLD;
        }
        break;

        // 松开消抖
        case(KEY_FSM_STATE_RELEASE_DEBOUNCE):
        {
            // 继续检测松开
            if(fsm->get_state(fsm->key) == KEY_STATE_Release)
            {
                // 松开超过DEBOUNCE_TIMEms，确认松开
                if(get_tick_diff(tick, fsm->last_tick) > DEBOUNCE_TIME)
                {
                    fsm->state = KEY_FSM_STATE_RELEASE;  // 切换到松开状态
                }
            }
            else    // 误触
            {
                fsm->state = KEY_FSM_STATE_PRESS;    // 回到按下
            }
        }
        break;

        // 松开
        case(KEY_FSM_STATE_RELEASE):
        {
            fsm->press_count++;         // 点击次数
            if(fsm->press_count == 1)   // 第一次点击
            {
                fsm->double_click_start_tick = tick;    // 记录等待双击时间
                fsm->state = KEY_FSM_STATE_WAIT_DOUBLE_CLICK;   // 等待双击事件
            }
            else if(fsm->press_count == 2)   // 第二次点击
            {
                fsm->state = KEY_FSM_STATE_DOUBLE_CLICK;    // 双击触发
            }
        }
        break;

        // 等待双击
        case(KEY_FSM_STATE_WAIT_DOUBLE_CLICK):
        {
            // 检测到按下
            if(fsm->get_state(fsm->key) == KEY_STATE_Press)
            {
                fsm->last_tick = tick;                      // 记录按下时时间
                fsm->state = KEY_FSM_STATE_PRESS_DEBOUNCE;  // 切换到按下消抖状态
            }
            // 双击超时
            else if(get_tick_diff(tick, fsm->double_click_start_tick) > DOUBLE_CLICK_TIMEOUT)
            {
                fsm->press_count = 0;       // 清空按下次数
                fsm->click_handle();
                fsm->state = KEY_FSM_STATE_IDLE;    // 回到空闲
            }
        }
        break;

        // 触发双击事件
        case(KEY_FSM_STATE_DOUBLE_CLICK):
        {
            // 执行双击回调
            fsm->double_click_handle();

            fsm->double_click_start_tick = 0;
            fsm->press_count = 0;

            fsm->state = KEY_FSM_STATE_IDLE;    // 回到空闲
        }
        break;

        // 按住长按检测
        case(KEY_FSM_STATE_PRESS_HOLD):
        {
            // 等待松开
            if(fsm->get_state(fsm->key) == KEY_STATE_Release)
            {
                // 长按中途松开，且超过双击检测区间
                if(get_tick_diff(tick, fsm->press_hold_start_tick) > DOUBLE_CLICK_TIMEOUT)
                {
                    fsm->state = KEY_FSM_STATE_IDLE;
                }
                else
                {
                    fsm->last_tick = tick;                          // 记录松开时间
                    fsm->state = KEY_FSM_STATE_RELEASE_DEBOUNCE;    // 松开消抖
                }
            }
            // 超时
            else if(get_tick_diff(tick, fsm->press_hold_start_tick) >= LONG_PRESS_TIME)
            {
                fsm->press_hold_start_tick = 0;
                fsm->state = KEY_FSM_STATE_LONG_PRESS;
            }
        }
        break;

        // 触发长按
        case(KEY_FSM_STATE_LONG_PRESS):
        {
            fsm->long_press_handle();
            fsm->state = KEY_FSM_STATE_WAIT_LONG_PRESS_RELEASE;
        }
        break;

        // 等待长按后的松开
        case(KEY_FSM_STATE_WAIT_LONG_PRESS_RELEASE):
        {
            if(fsm->get_state(fsm->key) == KEY_STATE_Release)
            {
                fsm->press_count = 0;
                fsm->state = KEY_FSM_STATE_IDLE;
            }
        }
        break;
    }
}

