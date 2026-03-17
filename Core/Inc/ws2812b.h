#ifndef WS2812B_H
#define WS2812B_H

#include "gpio.h"

// 定义WS2812B的数据引脚和GPIO端口
#define WS2812B_PIN      GPIO_PIN_6  // 数据引脚（例如 GPIOA_PIN_6）
#define WS2812B_PORT     GPIOA      // GPIO端口（例如 GPIOA）

// LED数量
#define LED_COUNT        1          // 设定控制LED的数量（例如1个LED）

// RGB颜色结构体定义
typedef struct {
    uint8_t R;  // 红色分量
    uint8_t G;  // 绿色分量
    uint8_t B;  // 蓝色分量
} RGB_Color;

// 函数声明
//void WS2812_Init(void);                 // 初始化GPIO
void WS2812_SetColor(uint8_t ledIndex, RGB_Color color); // 设置指定LED的颜色
void WS2812_SendData(void);             // 发送RGB数据到WS2812B
void WS2812_SendByte(uint8_t data);    // 发送单个字节数据

#endif // WS2812B_H
