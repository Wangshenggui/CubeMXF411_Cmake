#ifndef MODULE_AUTO_INIT_H
#define MODULE_AUTO_INIT_H

typedef void (*init_func_t)(void);

extern init_func_t __init_start_level0[];  // level0: 系统时钟、中断、内存等最底层初始化
extern init_func_t __init_end_level0[];
extern init_func_t __init_start_level1[];  // level1: UART、GPIO、I2C、SPI等基础驱动初始化
extern init_func_t __init_end_level1[];
extern init_func_t __init_start_level2[];  // level2: 文件系统、网络协议栈、GUI等中间件初始化
extern init_func_t __init_end_level2[];
extern init_func_t __init_start_level3[];  // level3: LED状态机、传感器、业务逻辑等应用初始化
extern init_func_t __init_end_level3[];

#define MODULE_INIT(init_fn, level) \
    static init_func_t __module_##init_fn \
    __attribute__((section(".init.level" #level), used)) = init_fn

void module_auto_init_all(void);

#endif

