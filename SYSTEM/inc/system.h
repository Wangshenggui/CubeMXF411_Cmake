#ifndef _SYSTEM_H
#define _SYSTEM_H

#include "main.h"
#include <stdio.h>

#define DEBUG_ENABLE 1

#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_RESET   "\033[0m"


#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : \
                      strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#if DEBUG_ENABLE
    /* 普通调试输出 */
    #define debug_printf(fmt, ...) \
    do { \
        printf("[%s:%d] [%lu]: " fmt, __FILENAME__, __LINE__, \
            HAL_GetTick(), ##__VA_ARGS__); \
    } while(0)

    /* INFO 级别（绿色）*/
    #define debug_info(fmt, ...) \
        do { \
            printf(COLOR_GREEN "[INFO] [%lu] [%s:%d]: " fmt COLOR_RESET, \
                HAL_GetTick(), __FILENAME__, __LINE__, ##__VA_ARGS__); \
        } while(0)

    /* WARN 级别（黄色）*/
    #define debug_warn(fmt, ...) \
        do { \
            printf(COLOR_YELLOW "[WARN] [%lu] [%s:%d]: " fmt COLOR_RESET, \
                HAL_GetTick(), __FILENAME__, __LINE__, ##__VA_ARGS__); \
        } while(0)

    /* ERROR 级别（红色）*/
    #define debug_error(fmt, ...) \
        do { \
            printf(COLOR_RED "[ERROR] [%lu] [%s:%d]: " fmt COLOR_RESET, \
                HAL_GetTick(), __FILENAME__, __LINE__, ##__VA_ARGS__); \
        } while(0)

#else
    #define debug_printf(fmt, ...) ((void)0)
    #define debug_info(fmt, ...)   ((void)0)
    #define debug_warn(fmt, ...)   ((void)0)
    #define debug_error(fmt, ...)  ((void)0)
#endif
    
/*计算时间差*/
static inline uint32_t get_tick_diff(uint32_t current, uint32_t previous)
{
    return current - previous;
}

#endif

