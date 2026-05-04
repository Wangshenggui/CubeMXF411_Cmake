#include "module_auto_init.h"

static void init_level(init_func_t *start, init_func_t *end) {
    init_func_t *p;
    
    for (p = start; p < end; p++) {
        if (*p) {
            (*p)();  // 调用初始化函数
        }
    }
}

void module_auto_init_all(void) {
    init_level(__init_start_level0, __init_end_level0);
    init_level(__init_start_level1, __init_end_level1);
    init_level(__init_start_level2, __init_end_level2);
    init_level(__init_start_level3, __init_end_level3);
}
