/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
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
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
// #include "oled_u8g2.h"
// #include "u8g2.h"
#include "drv_ina226.h"
#include <stdio.h>
#include "oled.h"
#include "drv_tps55289.h"
#include "drv_leds.h"
#include "drv_keys.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define INA226_UPFATE_TIME 200
#define ADC_MAX_DELAY      10
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
typedef struct
{
    uint32_t days;
    uint8_t  hours;
    uint8_t  minutes;
    uint8_t  seconds;
    uint16_t milliseconds;
} Time_DHMS_MS_t;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */


volatile uint32_t time_update_1ms = 0;
volatile bool ina226_update_flag = 0;
volatile uint8_t key_tick_flag = 0;
//volatile uint8_t breathe_flag = 0;
volatile bool i2c_busy = 0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
HAL_StatusTypeDef I2C_Lock(void);
void I2C_Unlock(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
    uint32_t sys_time = 0;
    uint32_t v_tps55289_vol = 5000; //mv
    Time_DHMS_MS_t sys_time_t;
    uint8_t current_select_5mR = 0;
    
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
  MX_ADC_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_TIM14_Init();
  /* USER CODE BEGIN 2 */

    // INA226 初始化
    if (I2C_Lock() == HAL_OK)
    {
        INA226_Init(&hi2c1);
        I2C_Unlock();
    }
    if (I2C_Lock() == HAL_OK)
    {
        OLED_Init();
        I2C_Unlock();
    }
//    INA226_Init(&hi2c1);
//    OLED_Init();
    if (I2C_Lock() == HAL_OK)
    {
        if (TPS55289_Init(&hi2c1) != TPS55289_OK)
        {
            OLED_ShowString(0, 5, "TPS55289_ERR", 12);
        }
        else
        {
            TPS55289_SetVoltage_Calc(v_tps55289_vol);
            TPS55289_SetCurrentLimit(TPS55289_IOUT_LIMIT_MAX);
        }
        I2C_Unlock();
    }

    if (I2C_Lock() == HAL_OK)
    {
        I2C_Scan();
        I2C_Unlock();
    }
    
    HAL_Delay(1000);
    // OLED_Clear();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    // HAL_TIMEx_PWMN_Start_IT(&htim14,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim14, TIM_CHANNEL_1);
    HAL_TIM_Base_Start_IT(&htim14);
    // I2C_Scan();

    /**
     * @brief STM32F030F4P6 16KB Flash 4KB RAM 48MHz clock
     *
     */
    if (I2C_Lock() == HAL_OK)
    {
        I2C_Scan();
        I2C_Unlock();
    }
    if (I2C_Lock() == HAL_OK)
    {
        OLED_Clear();
        OLED_ShowString(0, 0, "Bus_V =:      mV", 12);
        OLED_ShowString(0, 1, "Current:      uA", 12);
//        OLED_ShowString(0, 2, "I_MAX = 320mA", 12);
        OLED_ShowString(0, 2, "Vout set=     mV", 12);
        OLED_ShowString(0, 3, "Vbat vol=     mV", 12);
        OLED_ShowString(0, 6, "123456789012345678901", 12);
        OLED_ShowString(0, 7, "Time:00 00:00:00", 12);
//        OLED_ShowString(0, 3, "ADC value:      ", 12);
//        OLED_ShowString(0, 4, "Read Count:", 12);
        I2C_Unlock();
    }

    //  HAL_GPIO_WritePin(MCU_SYS_LED_GPIO_Port, MCU_SYS_LED_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOF, MCU_005R_SEL_Pin, GPIO_PIN_RESET);
    current_select_5mR = 0;
    while (1)
    {
//        if (key_tick_flag) 
//        { 
//            key_tick_flag = 0; 
//            Key_Tick();            
//        }    
        if (ina226_update_flag)
        {
            ina226_update_flag = false;
            drv_ina226_oled_display_task(current_select_5mR);
            //1. start adc
            HAL_ADC_Start(&hadc);
            //2. waite for convert ok
            if (HAL_ADC_PollForConversion(&hadc, ADC_MAX_DELAY) == HAL_OK)
            {
                uint32_t adc_dr = HAL_ADC_GetValue(&hadc);
//                uint32_t bat_vol= adc_dr*23;
                uint32_t bat_vol= ((uint32_t)(adc_dr * 10 + 682) * 1000) / 554;
                //4. display on oled
                OLED_ShowNum(79, 3, bat_vol, 5, 12);                
                OLED_ShowNum(79, 4, adc_dr, 5, 12);
            }
            OLED_ShowNum(79, 2, v_tps55289_vol, 5, 12);      
        }
        sys_time = HAL_GetTick() *0.001;
        sys_time_t.seconds = sys_time % 60;
        sys_time_t.minutes = sys_time / 60;
        sys_time_t.hours = sys_time / 3600;
        sys_time_t.days = sys_time /86400;
      //OLED_ShowString(0, 7, "Time:00 00:00:00", 12); //6*8的字符大小
        OLED_ShowNum(42, 7, sys_time_t.days, 2, 12);
        OLED_ShowNum(66, 7, sys_time_t.hours, 2, 12);
        OLED_ShowNum(90, 7, sys_time_t.minutes, 2, 12);
        OLED_ShowNum(114, 7, sys_time_t.seconds, 2, 12);
        /**
         * @brief Key 1 Function test
         *
         */
        if (Key_Check(KEY_1, KEY_SINGLE))
        {
            // OLED_ShowString(0, 3, "KEY1_SINGLE", 12);
            v_tps55289_vol += 1;
        }
        if (Key_Check(KEY_1, KEY_DOUBLE))
        {
            // OLED_ShowString(0, 3, "KEY1_DOUBLE", 12);
            v_tps55289_vol += 10;
        }
        if (Key_Check(KEY_1, KEY_LONG))
        {
            // OLED_ShowString(0, 3, "KEY1_LONG", 12);
        }
        if (Key_Check(KEY_1, KEY_REPEAT))
        {
            // OLED_ShowString(0, 3, "KEY1_REPEAT", 12);
            v_tps55289_vol += 100;
        }
        /**
         * @brief Key 2 Function test
         *
         */
        if (Key_Check(KEY_2, KEY_SINGLE))
        {
            // OLED_ShowString(0, 4, "KEY2_SINGLE", 12);
            v_tps55289_vol -= 1;
        }
        if (Key_Check(KEY_2, KEY_DOUBLE))
        {
            // OLED_ShowString(0, 4, "KEY2_DOUBLE", 12);
            v_tps55289_vol -= 10;
        }
        if (Key_Check(KEY_2, KEY_LONG))
        {
            // OLED_ShowString(0, 4, "KEY2_LONG", 12);
        }
        if (Key_Check(KEY_2, KEY_REPEAT))
        {
            // OLED_ShowString(0, 4, "KEY2_REPEAT", 12);
            v_tps55289_vol -= 100;
        }
        /**
         * @brief Key 3 Function test
         *
         */
        if (Key_Check(KEY_3, KEY_SINGLE))
        {
            // OLED_ShowString(0, 5, "KEY3_SINGLE", 12);
            TPS55289_SetVoltage_Calc(v_tps55289_vol);
        }
        if (Key_Check(KEY_3, KEY_DOUBLE))
        {
//            OLED_ShowString(0, 5, "KEY3_DOUBLE", 12);
        }
        if (Key_Check(KEY_3, KEY_LONG))
        {
            // OLED_ShowString(0, 5, "KEY3_LONG", 12);  
        }
        if (Key_Check(KEY_3, KEY_REPEAT))
        {
//            OLED_ShowString(0, 5, "KEY3_REPEAT", 12);
        }
        /**
         * @brief Key pwr Function test
         *
         */
        if (Key_Check(KEY_PWR, KEY_SINGLE))
        {
            HAL_GPIO_WritePin(GPIOF, MCU_005R_SEL_Pin, GPIO_PIN_SET);
            current_select_5mR = 1;
            OLED_ShowString(0, 6, "cur_sel_5mR", 12);
        }
        if (Key_Check(KEY_PWR, KEY_DOUBLE))
        {
            HAL_GPIO_WritePin(GPIOF, MCU_005R_SEL_Pin, GPIO_PIN_RESET);
            current_select_5mR = 0;
            OLED_ShowString(0, 6, "cur_sel_50R", 12);
//            OLED_ShowString(0, 6, "KEY_PWR_DOUBLE", 12);
//            I2C1_BusRecovery();
        }
        if (Key_Check(KEY_PWR, KEY_LONG))
        {
            OLED_ShowString(0, 6, "KEY_PWR_LONG", 12);
        }
        if (Key_Check(KEY_PWR, KEY_REPEAT))
        {
            OLED_ShowString(0, 6, "KEY_PWR_REPEAT", 12);
        }
    Breathe_Task();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//    if (htim->Instance == TIM14)
//    {
//        if (time_update_1ms <= 10000)
//        {
//            time_update_1ms += 1;
//        }
//        else
//        {
//            time_update_1ms = 0;
//        }
//        if (time_update_1ms % 200 && (!ina226_update_flag))
//            ina226_update_flag = true;

//        Key_Tick();
//        Breathe_Task();
//    }
//}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM14)
    {
//        if (time_update_1ms < 10000) 
//        {
            time_update_1ms++; 
//        }            
//        else 
//        {
//            time_update_1ms = 0;
//        }
        
        if ((time_update_1ms % 200) == 0) 
            ina226_update_flag = 1;
        if ((time_update_1ms % 20) == 0) 
            key_tick_flag = 1;
        Breathe_Task();
        Key_Tick(); 
    }
}


HAL_StatusTypeDef I2C_Lock(void)
{
    if (i2c_busy) return HAL_BUSY;
    i2c_busy = 1;
    return HAL_OK;
}

void I2C_Unlock(void)
{
    i2c_busy = 0;
}

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
#ifdef USE_FULL_ASSERT
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
