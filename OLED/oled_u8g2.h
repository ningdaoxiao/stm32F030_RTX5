/*
 * @Date: 2024-09-29 10:30:06
 * @LastEditors: Thonelyne chenwg1914@gmail.com
 * @LastEditTime: 2024-09-30 13:33:28
 * @FilePath: \01-stm32G0_demo\OLED\oled_u8g2.h
 */
#ifndef __oled_u8g2_H
#define __oled_u8g2_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "u8g2.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */



/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */
#define MAX_LEN    128  //
#define OLED_ADDRESS  0x78 // oled??????
#define OLED_CMD   0x00  // ???
#define OLED_DATA  0x40  // ???
 
/* USER CODE BEGIN Prototypes */
 uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
 uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
 void u8g2Init(u8g2_t *u8g2);
 #ifdef __cplusplus
}
#endif
#endif /*__ i2c_H */
/* USER CODE END Prototypes */
