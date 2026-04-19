#include "key_fsm.h"

/*按键状态机初始化*/
KEY_FSM_Structure KEY_SFM_Init(Key_Structure* key, ClickHandle_t click_handle)
{
    KEY_FSM_Structure fsm;

    fsm.state = KEY_FSM_STATE_IDLE;
    fsm.last_tick = 0;
    fsm.click_handle = click_handle;
    fsm.key = key;                      // 指向操作对象
    fsm.get_state = Key_GetState;       // 指向获取状态函数

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
                // 持续按下超过10ms，确认按下
                if(get_tick_diff(tick, fsm->last_tick) > 10)
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
            // 等待松开
            if(fsm->get_state(fsm->key) == KEY_STATE_Release)
            {
                fsm->last_tick = tick;                          // 记录松开时间
                fsm->state = KEY_FSM_STATE_RELEASE_DEBOUNCE;    // 松开消抖
            }
        }
        break;

        // 松开消抖
        case(KEY_FSM_STATE_RELEASE_DEBOUNCE):
        {
            // 继续检测松开
            if(fsm->get_state(fsm->key) == KEY_STATE_Release)
            {
                // 松开超过10ms，确认松开
                if(get_tick_diff(tick, fsm->last_tick) > 10)
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
            fsm->click_handle();
            fsm->state = KEY_FSM_STATE_IDLE;    // 回到空闲
        }
        break;
    }
}

