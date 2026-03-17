/*
 * @Date: 2024-11-09 15:36:23
 * @LastEditors: Thonelyne chenwg1914@gmail.com
 * @LastEditTime: 2024-11-14 14:06:46
 * @FilePath: \14-Meter\Core\Src\ina226.c
 */
#include "drv_ina226.h"
#include "oled.h"

#define INA226_MAX_DELAY      20

INA226_Config_Settings config_settings = {
    .mode               = INA226_MODE_SHUNT_BUS_VOLTAGE,  // 同时测量分流电压和总线电压
    .averaging          = INA226_AVG_16,                  // 16 次采样
    .shunt_adc_time     = INA226_SHUNT_ADC_8300us,         // 分流 ADC 采样时间
    .bus_adc_time       = INA226_BUS_ADC_8300us,           // 总线 ADC 采样时间
    .shunt_gain         = INA226_SHUNT_GAIN_1x,           // 分流增益 2x
    .bus_voltage_range  = INA226_BUS_VOLTAGE_RANGE_32V    // 总线电压范围 32V
};

INA226_Config_Settings deconfig_settings = {
    .mode               = INA226_MODE_POWER_DOWN,  // 同时测量分流电压和总线电压
//    .averaging          = INA226_AVG_16,                  // 16 次采样
//    .shunt_adc_time     = INA226_SHUNT_ADC_8300us,         // 分流 ADC 采样时间
//    .bus_adc_time       = INA226_BUS_ADC_8300us,           // 总线 ADC 采样时间
//    .shunt_gain         = INA226_SHUNT_GAIN_1x,           // 分流增益 2x
//    .bus_voltage_range  = INA226_BUS_VOLTAGE_RANGE_32V    // 总线电压范围 32V
};
// 初始化INA226传感器
void INA226_Init(I2C_HandleTypeDef *hi2c)
{
//    uint16_t calibration_value = 0x400; // 校准值：见in226.h文件
//    uint16_t config_value = 0x4127;     // 配置值：见in226.h文件
//    uint8_t dataToSend[3];
//    // 按大端字节序准备数据
//    dataToSend[0] = INA226_Cal_REG; // 低字节 0x34
//    dataToSend[1] = (uint8_t)(config_value >> 8);   // 高字节 0x41
//    dataToSend[2] = (uint8_t)(config_value & 0xFF); // 低字节 0x2


    // 获取最终的配置寄存器值
    uint16_t config_value = INA226_GetConfigValue(config_settings);
    HAL_I2C_Mem_Write(&hi2c1, INA226_ADR << 1, INA226_CFG_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t *)&config_value, sizeof(config_value), INA226_MAX_DELAY);

    //    uint8_t data[3];
    //    data[0] = INA226_Cal_REG;
    //    data[1] = (calibration_value >> 8) & 0xFF;
    //    data[2] = calibration_value & 0xFF;
    // HAL_I2C_Mem_Write(hi2c, INA226_ADR << 1, INA226_Cal_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t *)&calibration_value, 2, HAL_MAX_DELAY);
    // HAL_Delay(10);
    // HAL_I2C_Mem_Write(hi2c, INA226_ADR << 1, INA226_CFG_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t *)&config_value, 2, HAL_MAX_DELAY);
    // HAL_I2C_Master_Transmit(hi2c, INA226_ADR << 1, dataToSend, 1, HAL_MAX_DELAY);
    // HAL_I2C_Master_Receive(hi2c, INA226_ADR << 1, data, 2, HAL_MAX_DELAY); // 接收2字节数据
}

// 去初始化INA226传感器
void INA226_DeInit(I2C_HandleTypeDef *hi2c)
{
//    uint16_t config_value = 0x8000;     // 配置值：D15 set1
//		uint8_t cfg_h=0x80;
//		uint8_t cfg_l=0x00;
//    // HAL_I2C_Mem_Write(hi2c, INA226_ADR << 1, INA226_CFG_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t *)&config_value, 2, HAL_MAX_DELAY);
//    HAL_I2C_Master_Transmit(hi2c, INA226_ADR << 1, INA226_CFG_REG, 1, HAL_MAX_DELAY);
//    HAL_I2C_Master_Transmit(hi2c, INA226_ADR << 1, &cfg_h, 1, HAL_MAX_DELAY);
//    HAL_I2C_Master_Transmit(hi2c, INA226_ADR << 1, &cfg_l, 1, HAL_MAX_DELAY);
    
}
// 读取INA226寄存器的16位数据
uint16_t INA226_ReadRegister(I2C_HandleTypeDef *hi2c, uint8_t reg)
{
    uint8_t data[2];
    HAL_I2C_Master_Transmit(hi2c, INA226_ADR << 1, &reg, 1, INA226_MAX_DELAY); // 发送寄存器地址
    HAL_I2C_Master_Receive(hi2c, INA226_ADR << 1, data, 2, INA226_MAX_DELAY); // 接收2字节数据
    return (data[0] << 8) | data[1];                                       // 将两个字节合并为16位数据
}

/**
 * @brief 读取总线电压，分辨率/LSB=1.25mV
 * Based on the fixed 1.25-mV LSB for the Bus Voltage Register that a full-scale register results in a 40.96 V value.
 * Do not apply more than 36 V of actual voltage to the input pins.
 * I2C_HandleTypeDef 器件挂到哪一路i2c总线
 */
float INA226_ReadBusVoltage(I2C_HandleTypeDef *hi2c)
{
    uint16_t raw_voltage = INA226_ReadRegister(hi2c, INA226_BUS_V_REG);
    return raw_voltage * 1.25; // 转换为电压值（单位：mV）
    // return raw_voltage ; // 转换为电压值（单位：V）
}

/**
 * @brief 读取采样电阻流过电流
 * 采样电阻(500mR/2)和Current_LSB(10uA)共同决定
 * Current_LSB=2.5uV/bit 
 * Current_LSB = Maximum Expected Current(Imax)/2^15,default:8.192A 
 * set Current_LSB=10uA/bit,Imax=2^15*10uA=327.670mA
 * CAL = 0.00512/(Current_LSB × RSHUNT) = 0.00512/(10uA/bit * 0.5Ω) = 1024(dec)=0x400
 * current = current_reg_value x current_lsb
 *  */
float INA226_ReadCurrent(I2C_HandleTypeDef *hi2c)
{
    // uint16_t raw_current = INA226_ReadRegister(hi2c, INA226_Current_REG);
    /*直接的读取分流电阻电压寄存器INA226_SHUNT_V_REG 0x01*/
    uint16_t raw_current = INA226_ReadRegister(hi2c, INA226_SHUNT_V_REG);
    // return (raw_current & 0x7FFF)*5; // 根据校准值转换为电流（单位：uA）
    //return raw_current *5; // 根据校准值转换为电流（单位：uA）
    if (raw_current & 0x8000)
    {
        // 如果最高位为 1
        // 两补码转换：反转所有位并加1
        raw_current = ~raw_current + 1;
        return raw_current ;
        // return raw_current * 5;
    }
    else
    {
        // 如果最高位为 0
        return raw_current; // 根据校准值转换为电流（单位：uA）
        // return raw_current * 5; // 根据校准值转换为电流（单位：uA）
    }
}

/**
 * @brief 读取INA226寄存器�?�并在oled显示
 *
 */
void INA226_Scan(void)
{
//  uint8_t i = 0;
//  uint16_t reg_data;
//  OLED_ShowString(0, 0, "INA226 scan...", 12);
  HAL_Delay(100);
//  OLED_Clear();
  for (uint8_t reg = 0x00; reg < 0x08; reg++)
  {
//    OLED_ShowString(0, i, "0x", 12);
//    OLED_ShowNum(17, i, reg, 2, 12);
//    reg_data = INA226_ReadRegister(&hi2c1, reg);
      INA226_ReadRegister(&hi2c1, reg);
//    OLED_ShowNum(33, i, reg_data, 6, 12);
//    i++;
    HAL_Delay(1000);
  }
}

uint16_t INA226_GetConfigValue(INA226_Config_Settings settings)
{
    uint16_t config = 0;

    // 设置模式
    config |= (settings.mode << 15);

    // 设置平均模式
    config |= (settings.averaging << 12);

    // 设置分流电压 ADC 采样时间
    config |= (settings.shunt_adc_time << 9);

    // 设置总线电压 ADC 采样时间
    config |= (settings.bus_adc_time << 6);

    // 设置分流电压增益
    config |= (settings.shunt_gain << 3);

    // 设置总线电压范围
    config |= settings.bus_voltage_range;

    return config;
}

/**
 * @brief 使用OLED显示电流曲线
 * 最大30帧刷新
 * 对数坐标显示最低10uA分辨路，10uA~100uA~1mA~100mA
 *       ^
 * 100mA |
 *  10mA |
 *   1mA |
 * 100uA |
 *  10uA |
 *       |------------------------------------>
 * 
 */

void drv_ina226_oled_display_task(uint8_t current_select_5mR)
{
    float current = INA226_ReadCurrent(&hi2c1);
    if(current_select_5mR == 0)
    {
        //采样电阻50R,lsb 2.5uV = 50R * 0.05uA
        current = current * 0.05;
    }
    else
    {
        //采样电阻5mR 2.5uV = 0.005R * 500uA
        current = current * 500;
    }
       
    
    //HAL_Delay(30);
    float voltage = INA226_ReadBusVoltage(&hi2c1);
    OLED_ShowNum(79, 0, voltage, 5, 12);
    OLED_ShowNum(79, 1, current, 5, 12);
    /** to be define
     * @brief 按键功能
     * 按键可以选择电流挡位 164mA 10A
     *
     */
//    if (read_count < 0xFFFF)
//    {
//      /* code */
//      OLED_ShowNum(73, 4, read_count, 6, 12);
//    }
//    else
//    {
//      /* code */
//      read_count = 0;
//      OLED_ShowString(0, 5, "Read Count Reset!", 12);
//      
//    }
//    read_count += 1;
}
//end ina226.c
