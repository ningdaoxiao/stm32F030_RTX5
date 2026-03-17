/*
 * @Date: 2024-11-09 15:37:06
 * @LastEditors: Thonelyne chenwg1914@gmail.com
 * @LastEditTime: 2024-11-13 15:51:19
 * @FilePath: \14-Meter\Core\Inc\ina226.h
 */
#ifndef INA226_H
#define INA226_H

#include "i2c.h"  // 根据你的MCU型号选择合适的HAL库头文件

// INA226 I2C地址
#define INA226_ADR 0x45                 //7-bit mode;A1 A0 to VCC
// Every INA226 REG have 2Byte / 16bit
#define INA226_CFG_REG          0x00    // RW  All-register reset, shunt voltage and bus voltage ADC conversion times and averaging, operating mode.
#define INA226_SHUNT_V_REG      0x01    // R Shunt voltage measurement data.
#define INA226_BUS_V_REG        0x02    // R Bus voltage measurement data.
#define INA226_Power_REG        0x03    // R Contains the value of the calculated power being delivered to the load.
#define INA226_Current_REG      0x04    // R Contains the value of the calculated current flowing through the shunt resistor.
#define INA226_Cal_REG          0x05    // RW Sets full-scale range and LSB of current and power measurements. Overall system calibration.
#define INA226_MASK_REG         0x06    // RW Alert configuration and Conversion Ready flag.
#define INA226_AlertLimit_REG   0x07    // RW Contains the limit value to compare to the selected Alert function.
#define INA226_MaifacID_REG     0xEF    // R Contains unique manufacturer identification number.
#define INA226_DieID_REG        0xFF    // R Contains the limit value to compare to the selected Alert function.
// 分流电阻值
#define SHUNT_RESISTOR 0.25          // 单位为欧姆
//#define config_value  0x4          // 单位为欧姆

// INA226 配置寄存器的各个字段
typedef enum {
    // Mode 设置（操作模式）
    INA226_MODE_POWER_DOWN        = 0x00,  // 关闭设备
    INA226_MODE_SHUNT_VOLTAGE     = 0x01,  // 仅测量分流电压
    INA226_MODE_BUS_VOLTAGE       = 0x02,  // 仅测量总线电压
    INA226_MODE_SHUNT_BUS_VOLTAGE = 0x03,  // 同时测量分流电压和总线电压
    INA226_MODE_ADCOFF            = 0x04,  // 禁用 ADC（不进行测量）

    // Averaging 设置（平均模式）
    INA226_AVG_1                  = 0x00,  // 1 次采样
    INA226_AVG_4                  = 0x01,  // 4 次采样
    INA226_AVG_16                 = 0x02,  // 16 次采样
    INA226_AVG_64                 = 0x03,  // 64 次采样
    INA226_AVG_128                = 0x04,  // 128 次采样
    INA226_AVG_256                = 0x05,  // 256 次采样
    INA226_AVG_512                = 0x06,  // 512 次采样
    INA226_AVG_1024               = 0x07,  // 1024 次采样

    // Shunt ADC Conversion Time（分流电压采样时间）
    INA226_SHUNT_ADC_140us        = 0x00,  // 140 µs
    INA226_SHUNT_ADC_204us        = 0x01,  // 204 µs
    INA226_SHUNT_ADC_332us        = 0x02,  // 332 µs
    INA226_SHUNT_ADC_588us        = 0x03,  // 588 µs
    INA226_SHUNT_ADC_1100us       = 0x04,  // 1100 µs
    INA226_SHUNT_ADC_2100us       = 0x05,  // 2100 µs
    INA226_SHUNT_ADC_4200us       = 0x06,  // 4200 µs
    INA226_SHUNT_ADC_8300us       = 0x07,  // 8300 µs

    // Bus ADC Conversion Time（总线电压采样时间）
    INA226_BUS_ADC_140us          = 0x00,  // 140 µs
    INA226_BUS_ADC_204us          = 0x01,  // 204 µs
    INA226_BUS_ADC_332us          = 0x02,  // 332 µs
    INA226_BUS_ADC_588us          = 0x03,  // 588 µs
    INA226_BUS_ADC_1100us         = 0x04,  // 1100 µs
    INA226_BUS_ADC_2100us         = 0x05,  // 2100 µs
    INA226_BUS_ADC_4200us         = 0x06,  // 4200 µs
    INA226_BUS_ADC_8300us         = 0x07,  // 8300 µs

    // Shunt Voltage Gain（分流电压增益）
    INA226_SHUNT_GAIN_1x          = 0x00,  // 1x 增益
    INA226_SHUNT_GAIN_2x          = 0x01,  // 2x 增益
    INA226_SHUNT_GAIN_4x          = 0x02,  // 4x 增益
    INA226_SHUNT_GAIN_8x          = 0x03,  // 8x 增益

    // Bus Voltage Range（总线电压范围）
    INA226_BUS_VOLTAGE_RANGE_16V  = 0x00,  // 16V
    INA226_BUS_VOLTAGE_RANGE_32V  = 0x01,  // 32V
    INA226_BUS_VOLTAGE_RANGE_64V  = 0x02,  // 64V
} INA226_Config;

// 配置结构体
typedef struct {
    INA226_Config mode;            // 操作模式
    INA226_Config averaging;       // 平均模式
    INA226_Config shunt_adc_time;  // 分流 ADC 采样时间
    INA226_Config bus_adc_time;    // 总线 ADC 采样时间
    INA226_Config shunt_gain;      // 分流电压增益
    INA226_Config bus_voltage_range; // 总线电压范围
} INA226_Config_Settings;



// INA226 初始化和数据读取函数声明
void INA226_Init(I2C_HandleTypeDef *hi2c);
void INA226_DeInit(I2C_HandleTypeDef *hi2c);
float INA226_ReadBusVoltage(I2C_HandleTypeDef *hi2c);
float INA226_ReadCurrent(I2C_HandleTypeDef *hi2c);
uint16_t INA226_GetConfigValue(INA226_Config_Settings settings);

#endif  // INA226_H

/**
 * 配置寄存器地址 reg=0x00  value=0x4527 =0100 0101 0010 0111(b)
 * 配置寄存器地址 reg=0x00  value=0x4127 =0100 0001 0010 0111(b)--结构体中数据
 * Bit 15       Reset Bit  
 *              Setting this bit to '1' generates a system reset that is the same as power-on reset. Resets all registers to default values; this bit self-clears.
 * Bits 12–14   - - - dafult 100
 * Bits 9–11    Averaging Mode  (D11 D10 D9   AVG Num =4^n —— 1~1024； set：010 —— 16times)值为设置 16 次平均
 *              Determines the number of samples that are collected and averaged. 表 6 shows all the AVG bit settings and related number of averages for each bit setting.
 *              D11 D10 D9      NUMBER OF AVERAGES
 *               0   0  0        1
 *               0   0  1        4
 * set           0   1  0        16
 *               0   1  1        64
 *               1   0  0        128 
 *               1   0  1        256
 *               1   1  0        512
 *               1   1  1        1024
 * Bits 6–8     Bus Voltage Conversion Time         
 *              Sets the conversion time for the bus voltage measurement. 表 7 shows the VBUSCT bit options and related conversion times for each bit setting.
 *              D8 D7 D6     CONVERSION TIME
 *               0  0  0        140us
 *               0  0  1        204us
 *               0  1  0        332us
 *               0  1  1        588us
 * set           1  0  0        1.1ms
 *               1  0  1        2.116ms
 *               1  1  0        4.156ms
 *               1  1  1        8.244ms
 * Bits 3–5     Shunt Voltage Conversion Time
 *              Sets the conversion time for the shunt voltage measurement. 表 8 shows the VSHCT bit options and related conversion times for each bit setting
 *              D5 D4 D3     CONVERSION TIME
 *               0  0  0        140us
 *               0  0  1        204us
 *               0  1  0        332us
 *               0  1  1        588us
 * set           1  0  0        1.1ms
 *               1  0  1        2.116ms
 *               1  1  0        4.156ms
 *               1  1  1        8.244ms
 * Bits 0-2     Operating Mode
 *              D5 D4 D3        MODE
 *               0  0  0     Power-Down (or Shutdown)
 *               0  0  1     Shunt Voltage, Triggered
 *               0  1  0     Bus Voltage, Triggered
 *               0  1  1     Shunt and Bus, Triggered
 *               1  0  0     Power-Down (or Shutdown)
 *               1  0  1     Shunt Voltage, Continuous
 *               1  1  0     Bus Voltage, Continuous
 * default       1  1  1     Shunt and Bus, Continuous
 *
 * Shunt Voltage Register (01h) (Read-Only) Description
 *          BIT#         D15 D14 D13 D12 D11 D10 D9 D8 D7 D6 D5 D4 D3 D2 D1 D0
 *          BIT NAME     SIGN 
 *  The Bus Voltage Register stores the most recent bus voltage reading, VBUS. If averaging is enabled, this register displays the averaged value. 
 *  Full-scale range = 81.92 mV (decimal = 7FFF); LSB: 2.5 μV.
 * Bus Voltage Register (02h) (Read-Only) (1)

 */
 