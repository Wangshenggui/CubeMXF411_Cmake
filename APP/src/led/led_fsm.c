#include "led_fsm.h"
#include "system.h"


// 定义led1状态机结构体
LED_FSM_Structure led1_fsm_struct = 
{
    .event = LED_FSM_EVENT_OFF,
    .state = LED_FSM_STATE_OFF,
    .blink_on_tick = 0,
    .blink_off_tick = 0,
    .blink_on_time = 0,             // 闪烁亮时间
    .blink_off_time = 0,            // 闪烁灭时间
    .led_struct = &led1_struct,
};
// 定义led2状态机结构体
LED_FSM_Structure led2_fsm_struct = 
{
    .event = LED_FSM_EVENT_OFF,
    .state = LED_FSM_STATE_OFF,
    .blink_on_tick = 0,
    .blink_off_tick = 0,
    .blink_on_time = 0,             // 闪烁亮时间
    .blink_off_time = 0,            // 闪烁灭时间
    .led_struct = &led2_struct,
};

/*LED状态机初始化*/
void LED_SFM_Init(LED_FSM_Structure* fsm, LED_Structure* led_struct)
{
    (void)fsm;
    (void)led_struct;
    // 定义结构体时已经初始化，除非有什么特殊处理
}

/*设置led闪烁*/
void LED_FSM_SetBlinkEvent(LED_FSM_Structure* fsm, uint32_t on_time, uint32_t off_time)
{
    fsm->event = LED_FSM_EVENT_BLINK;
    fsm->state = LED_FSM_STATE_COUNT;   // 用来判断第一次进入闪烁
    fsm->blink_on_tick = 0;
    fsm->blink_off_tick = 0;
    fsm->blink_on_time = on_time;
    fsm->blink_off_time = off_time;
}

/*关闭led*/
void LED_FSM_SetOFFEvent(LED_FSM_Structure* fsm)
{
    fsm->event = LED_FSM_EVENT_OFF;
    fsm->state = LED_FSM_STATE_OFF;
}

/*开启led*/
void LED_FSM_SetONEvent(LED_FSM_Structure* fsm)
{
    fsm->event = LED_FSM_EVENT_ON;
    fsm->state = LED_FSM_STATE_OFF;
}

/*LED状态机运行处理函数*/
void LED_FSM_Run(LED_FSM_Structure* fsm, uint32_t tick)
{
    switch(fsm->event)
    {
        // 关闭事件
        case(LED_FSM_EVENT_OFF):
        {
            LED_SetState(fsm->led_struct,LED_STATE_OFF);
            fsm->state = LED_FSM_STATE_OFF;
            fsm->event = LED_FSM_EVENT_IDLE;    // 只执行一次
        }
        break;

        // 开启事件
        case(LED_FSM_EVENT_ON):
        {
            LED_SetState(fsm->led_struct,LED_STATE_ON);
            fsm->state = LED_FSM_STATE_ON;
            fsm->event = LED_FSM_EVENT_IDLE;    // 只执行一次
        }
        break;

        // 闪烁事件
        case(LED_FSM_EVENT_BLINK):
        {
            // 首次进入闪烁状态
            if((fsm->state != LED_FSM_STATE_ON) \
            && (fsm->state != LED_FSM_STATE_OFF))
            {
                LED_SetState(fsm->led_struct, LED_STATE_ON);
                fsm->blink_on_tick = tick;  // 记录开始亮的时间
                fsm->state = LED_FSM_STATE_ON;
            }
            else if(fsm->state == LED_FSM_STATE_OFF)
            {
                // 当前是灭的状态，检查是否该亮了
                if(get_tick_diff(tick, fsm->blink_off_tick) >= fsm->blink_off_time)
                {
                    LED_SetState(fsm->led_struct, LED_STATE_ON);
                    fsm->blink_on_tick = tick;  // 记录开始亮的时间
                    fsm->state = LED_FSM_STATE_ON;
                }
            }
            else if(fsm->state == LED_FSM_STATE_ON)
            {
                // 当前是亮的状态，检查是否该灭了
                if(get_tick_diff(tick, fsm->blink_on_tick) >= fsm->blink_on_time)
                {
                    LED_SetState(fsm->led_struct, LED_STATE_OFF);
                    fsm->blink_off_tick = tick;   // 记录开始灭的时间
                    fsm->state = LED_FSM_STATE_OFF;
                }
            }
        }
        break;

        // 默认事件(空闲)
        default:
        {
            
        }
        break;
    }
}

