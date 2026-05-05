#include "system.h"
#include "usbd_cdc_if.h"


// 重定向printf
int _write(int file, char *ptr, int len)
{
  (void)file;
  
  // 等待上次完成
  while(cdc_tx_complete == 0);
  cdc_tx_complete = 0;
  if (CDC_Transmit_FS((uint8_t*)ptr, len) == USBD_OK)
  {
    return len;
  }
  return -1;
}

