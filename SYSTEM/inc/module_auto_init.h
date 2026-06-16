/*段收集自动初始化*/
#ifndef MODULE_AUTO_INIT_H
#define MODULE_AUTO_INIT_H

typedef void (*init_func_t)(void);

extern init_func_t __init_start_level0[];  // level0: 系统级初始化
extern init_func_t __init_end_level0[];
extern init_func_t __init_start_level1[];  // level1: BSP基础驱动初始化
extern init_func_t __init_end_level1[];
extern init_func_t __init_start_level2[];  // level2: 中间件初始化
extern init_func_t __init_end_level2[];
extern init_func_t __init_start_level3[];  // level3: APP应用初始化
extern init_func_t __init_end_level3[];

// 定义初始化级别
#define INIT_LEVEL_SYS    0  // 系统级初始化
#define INIT_LEVEL_BSP    1  // BSP基础驱动初始化
#define INIT_LEVEL_MID    2  // 中间件初始化
#define INIT_LEVEL_APP    3  // APP应用初始化

// 将宏值转换为字符串
#define STRINGIFY(x) #x
#define STRINGIFY_VALUE(x) STRINGIFY(x)
// #define MODULE_INIT(init_fn, level) \
//     static init_func_t __module_##init_fn \
//     __attribute__((section(".init.level" #level), used)) = init_fn
#define MODULE_INIT(init_fn, level) \
    static init_func_t __module_##init_fn \
    __attribute__((section(".init.level" STRINGIFY_VALUE(level)), used)) = init_fn

void module_auto_init_all(void);

#endif

