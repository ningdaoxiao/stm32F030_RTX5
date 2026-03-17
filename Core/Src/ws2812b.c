/*
 * @Date: 2024-11-13 17:15:06
 * @LastEditors: Thonelyne chenwg1914@gmail.com
 * @LastEditTime: 2024-11-13 17:29:15
 * @FilePath: \14-Meter\Core\Src\ws2812b.c
 */
#include "ws2812b.h"


#define LED_COUNT        1          // LED 数量，可以根据需要调整



RGB_Color leds[LED_COUNT];  // 存储LED颜色的数组

// 设置LED颜色
void WS2812_SetColor(uint8_t ledIndex, RGB_Color color) {
    if (ledIndex < LED_COUNT) {
        leds[ledIndex] = color;
    }
}

// 发送一个字节的时序数据MCU_RGB_DATA_GPIO_Port, MCU_RGB_DATA_Pin,
//void WS2812_SendByte(uint8_t data) {
//    for (int i = 7; i >= 0; i--) {
//        if ((data >> i) & 0x01) {
//            HAL_GPIO_WritePin(MCU_RGB_DATA_GPIO_Port, MCU_RGB_DATA_Pin, GPIO_PIN_SET);  // 高电平
//            //_nop_();  // 延时
//            HAL_GPIO_WritePin(MCU_RGB_DATA_GPIO_Port, MCU_RGB_DATA_Pin, GPIO_PIN_RESET);  // 低电平
//        } else {
//            HAL_GPIO_WritePin(MCU_RGB_DATA_GPIO_Port, MCU_RGB_DATA_Pin, GPIO_PIN_SET);  // 高电平
//            HAL_GPIO_WritePin(MCU_RGB_DATA_GPIO_Port, MCU_RGB_DATA_Pin, GPIO_PIN_RESET);  // 低电平
//            HAL_GPIO_WritePin(MCU_RGB_DATA_GPIO_Port, MCU_RGB_DATA_Pin, GPIO_PIN_RESET);  // 延时
//        }
//    }
//}

// 发送整个LED数据（RGB）
void WS2812_SendData(void) {
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        // 发送每个LED的颜色数据（RGB）
        WS2812_SendByte(leds[i].G);  // 先发送绿
        WS2812_SendByte(leds[i].R);  // 然后发送红
        WS2812_SendByte(leds[i].B);  // 最后发送蓝
    }
    // 假设所有数据发送完之后需要一些延时来稳定信号
    HAL_Delay(1);
}
