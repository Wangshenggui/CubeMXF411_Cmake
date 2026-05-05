#include "module_auto_init.h"

/**
 * @brief 执行指定级别的所有模块初始化函数
 * 
 * @param start 该级别初始化函数数组的起始地址
 * @param end   该级别初始化函数数组的结束地址
 * 
 * @note 该函数会遍历从 start 到 end 之间的所有函数指针，
 *       并依次调用每个非空的初始化函数
 */
static void init_level(init_func_t *start, init_func_t *end)
{
    init_func_t *p;  // 函数指针，指向当前要执行的初始化函数
    
    /* 遍历该级别的所有初始化函数 */
    for (p = start; p < end; p++)
    {
        /* 检查函数指针是否有效（非空） */
        if (*p)
        {
            (*p)();  // 调用初始化函数
        }
    }
}

/**
 * @brief 执行所有模块的自动初始化
 * 
 * @note 按照优先级顺序执行各级别的初始化函数：
 *       - Level 0: 最先执行，通常用于最基础的硬件初始化
 *       - Level 1: 第二优先级，用于核心服务初始化
 *       - Level 2: 第三优先级，用于设备驱动初始化
 *       - Level 3: 最后执行，用于应用层初始化
 * 
 * @warning 初始化函数必须已经通过链接脚本或编译器特性
 *          被放置在对应的段（section）中
 */
void module_auto_init_all(void)
{
    /* 按优先级从高到低依次执行各级别的初始化 */
    init_level(__init_start_level0, __init_end_level0);  // 级别0：最高优先级
    init_level(__init_start_level1, __init_end_level1);  // 级别1
    init_level(__init_start_level2, __init_end_level2);  // 级别2
    init_level(__init_start_level3, __init_end_level3);  // 级别3：最低优先级
}