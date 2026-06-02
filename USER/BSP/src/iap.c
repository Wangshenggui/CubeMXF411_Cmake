#include "iap.h"
#include "stmflash.h"

iapfun jump2app;
//appxaddr:应用程序的起始地址
//appbuf:应用程序CODE.
//appsize:应用程序大小(字节).
void iap_write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t appsize)
{
	FlashWriteMulti_Byte(appxaddr, appbuf, appsize);
}

//跳转到应用程序段
//appxaddr:用户代码起始地址.
void iap_load_app(uint32_t appxaddr)
{
    // 关闭所有中断
    __disable_irq();
    
    // 禁用所有外设中断
    for(int i = 0; i < 8; i++) {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }

    HAL_RCC_DeInit();   // 恢复RCC时钟
    __DSB();
    __ISB();
    
    // 设置栈指针并跳转
    __set_MSP(*(uint32_t*)appxaddr);
    jump2app=(iapfun)*(uint32_t*)(appxaddr+4);
    jump2app();
}

