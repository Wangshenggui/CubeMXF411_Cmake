/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "led.h"
#include "led_fsm.h"
#include "system.h"
#include "usbd_cdc_if.h"
#include "key.h"
#include "key_fsm.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// 定义LED结构体
LED_Structure led1;
LED_Structure ledx;
// 定义LED状态机结构体
LED_FSM_Structure led1_fsm;
LED_FSM_Structure ledx_fsm;
// 定义按键结构体
Key_Structure key;
Key_Structure keyx;
// 定义按键状态机结构体
KEY_FSM_Structure key_fsm;
KEY_FSM_Structure keyx_fsm;


void KeyClickTest()
{
  uint8_t buf[100];
  int len = sprintf(buf,"key\r\n");
  CDC_Transmit_FS(buf,len);
}
void KeyxClickTest()
{
  uint8_t buf[100];
  int len = sprintf(buf,"keyx\r\n");
  CDC_Transmit_FS(buf,len);
}

void KeyDoubleClickTest()
{
  uint8_t buf[100];
  int len = sprintf(buf,"key double\r\n");
  CDC_Transmit_FS(buf,len);
}
void KeyxDoubleClickTest()
{
  uint8_t buf[100];
  int len = sprintf(buf,"keyx double\r\n");
  CDC_Transmit_FS(buf,len);
}

void KeyPressLongTest()
{
  uint8_t buf[100];
  int len = sprintf(buf,"key long\r\n");
  CDC_Transmit_FS(buf,len);
}
void KeyxPressLongTest()
{
  uint8_t buf[100];
  int len = sprintf(buf,"keyx long\r\n");
  CDC_Transmit_FS(buf,len);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  // 初始化LED
  led1 = LED_Init(led_GPIO_Port,  led_Pin,  LED_POLARITY_LOW);
  ledx = LED_Init(ledx_GPIO_Port, ledx_Pin, LED_POLARITY_LOW);
  // 初始化LED状态机
  led1_fsm = LED_FSM_Init(&led1);
  ledx_fsm = LED_FSM_Init(&ledx);
  // 初始设置500ms闪烁
  LED_FSM_SetBlinkEvent(&led1_fsm,500,500);
  LED_FSM_SetBlinkEvent(&ledx_fsm,500,500);

  // 初始化KEY
  key  = Key_Init(key_GPIO_Port,  key_Pin,  KEY_POLARITY_LOW);
  keyx = Key_Init(keyx_GPIO_Port, keyx_Pin, KEY_POLARITY_LOW);
  // 初始化按键状态机
  key_fsm  = KEY_FSM_Init(&key,   KeyClickTest
#ifdef DOUBLE_CLICK_ENABLE
    ,KeyDoubleClickTest
#endif
#ifdef LONG_PRESS_ENABLE
    ,KeyPressLongTest
#endif
  );

  keyx_fsm  = KEY_FSM_Init(&keyx,   KeyxClickTest
#ifdef DOUBLE_CLICK_ENABLE
    ,KeyxDoubleClickTest
#endif
#ifdef LONG_PRESS_ENABLE
    ,KeyxPressLongTest
#endif
  );
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // 获取系统计数器
    uint32_t tick = HAL_GetTick();

    KEY_FSM_Run(&key_fsm,tick);
    KEY_FSM_Run(&keyx_fsm,tick);

    // 运行LED状态机
    LED_FSM_Run(&led1_fsm, tick);
    LED_FSM_Run(&ledx_fsm, tick);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
