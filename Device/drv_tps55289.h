// -*- coding: utf-8 -*-
/**
 * @file    tps55289.h
 * @brief   Driver for TPS55289 Buck-Boost Converter (I2C interface)
 * @author  Thonelyne
 * @date    2025-09-19
 * @version 1.0.0
 */

#ifndef __DRV_TPS55289_H__
#define __DRV_TPS55289_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c.h"   
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define TPS55289_SHELL 0

#if TPS55289_SHELL == 1
#include "shell.h"

#endif	
/* -------------------------------------------------------------------------- */
/*                                Configuration                                */
/* -------------------------------------------------------------------------- */    
/**
 * @brief 可能的 I2C 地址（7 位），取决于 ADDR SEL 引脚配置
 * 查手册确认具体地址
 */
#define TPS55289_MODE 	1
#if	TPS55289_MODE==0
#define  htps55289_addr  0x75
#else
#define  htps55289_addr  0x74
#endif

/* -------------------------------------------------------------------------- */
/*                              Register definitions                          */
/* -------------------------------------------------------------------------- */

/* 寄存器地址定义（手册中表 7-1 等） */
#define TPS55289_REG_REF_LSB         0x00U  /* REF 寄存器低字节 :contentReference[oaicite:1]{index=1} */
#define TPS55289_REG_REF_MSB         0x01U  /* REF 寄存器高字节 */
#define TPS55289_REG_IOUT_LIMIT      0x02U  /* 电流限制寄存器 :contentReference[oaicite:2]{index=2} */
#define TPS55289_REG_VOUT_SR         0x03U  /* Slew Rate 寄存器 :contentReference[oaicite:3]{index=3} */
#define TPS55289_REG_INTFB_EXTFB     0x04U  /* 内部或外部反馈选择 etc. :contentReference[oaicite:4]{index=4} */
#define TPS55289_REG_CDC             0x05U
#define TPS55289_REG_OE_DISCHG       0x06U  /* OE + DISCHARGE 控制等位 / 启动控制寄存器 :contentReference[oaicite:5]{index=5} */
#define TPS55289_REG_STATUS          0x07U  //Only read

//#define INTFB_RATIO 0x03

//#if   (INTFB_RATIO == 0x00)
//    #define TPS55289_INTFB   0.2256f

//#elif (INTFB_RATIO == 0x01)
//    #define TPS55289_INTFB   0.1128f

//#elif (INTFB_RATIO == 0x02)
//    #define TPS55289_INTFB   0.0752f

//#elif (INTFB_RATIO == 0x03)
//    #define TPS55289_INTFB   0.0564f   // 默认最常用比例（5V/12V/24V）

//#else
//    #error "INTFB_RATIO value invalid! Must be 0x00 ~ 0x03"
//#endif


#define TPS_REF_MIN_MV     45    // 45mV
#define TPS_REF_STEP_uV    564   // 564uV


/* -------------------------------------------------------------------------- */
/*                              Typedefs & Enums                              */
/* -------------------------------------------------------------------------- */                                                                                                                                                                                                                                                                                                                                                                                                                                                        
/**
 * @brief TPS55289 状态码
 */
typedef enum {
    TPS55289_OK = 0,
    TPS55289_ERROR,
    TPS55289_I2C_ERROR,
    TPS55289_INVALID_PARAM,
} tps55289_status_t;

//typedef struct
//{
//    uint8_t ref_lsb;   // 写入寄存器 0x00
//    uint8_t ref_msb;   // 写入寄存器 0x01（只用低 3 bit）
//    uint16_t ref_raw; // 11-bit 原始值，调试可用
//} TPS55289_REF_t;
/**
 * @brief 电流限制选项
 * （根据手册定义可能的阶梯值或寄存器可设置值）
 */
typedef enum {
    //
    TPS55289_IOUT_LIMIT_MIN = 0,
    TPS55289_IOUT_LIMIT_MID = 0xC0,
    TPS55289_IOUT_LIMIT_DEF = 0xE4,
    TPS55289_IOUT_LIMIT_MAX = 0xFF,
    TPS55289_IOUT_LIMIT_DIS = 0x00,
    // … 根据手册添加更多
} tps55289_iout_limit_t;

//static const uint32_t INTFB_TABLE[4] = {
//    2256,  // 0.2256
//    1128,  // 0.1128
//    752,   // 0.0752
//    564    // 0.0564
//};

/* -------------------------------------------------------------------------- */
/*                            Public API / Function Prototypes               */
/* -------------------------------------------------------------------------- */

/**
 * @brief 初始化 TPS55289 驱动
 * @param hi2c       指向 HAL I2C handle 的指针
 * @param i2c_addr   用于该芯片的 I2C 从地址（7 位）
 * @return tps55289_status_t
 */
tps55289_status_t TPS55289_Init(I2C_HandleTypeDef *hi2c);

/**
 * @brief 设置输出电压
 * @param ref_value  REF 寄存器所需值（参考手册，低字节先写，再写高字节）
 * @return tps55289_status_t
 */
//tps55289_status_t TPS55289_SetVoltage(uint16_t ref_value);

/**
 * @brief 设置电流限制
 * @param limit_enum 电流限制枚举类型
 * @return tps55289_status_t
 */
tps55289_status_t TPS55289_SetCurrentLimit(tps55289_iout_limit_t limit_enum);
void TPS55289_SetVoltage_Calc(uint16_t vout);

#ifdef __cplusplus
extern "C" {
#endif
/**
 *
 */
#endif
