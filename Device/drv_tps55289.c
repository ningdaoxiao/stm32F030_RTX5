// -*- coding: utf-8 -*-
/**
 * @file    drv_tps55289.c
 * @brief   Source file for TPS55289 driver
 * @author  Thonelyne
 * @date    2025-09-19
 * @version 1.0.0
 */

#include "stm32f0xx.h" // Device header
#include "drv_tps55289.h"

/**TPS55289芯片信息
 * 输入电压 3.0V~30V
 * 输出电压 0.8V~22V 可调 步进10mV
 * 输出电流 0 ~6.35A 可调 步进50mA
 *
 */

/* -------------------------------------------------------------------------- */
/*                      Static / Private Variables & Definitions             */
/* -------------------------------------------------------------------------- */

/* I2C handle 和地址 */
static I2C_HandleTypeDef *htps_i2c = &hi2c1;

/* 超时参数 */
#define TPS55289_MAX_DELAY 20

/* -------------------------------------------------------------------------- */
/*                      Private Helpers                                       */
/* -------------------------------------------------------------------------- */

static tps55289_status_t write_reg(uint8_t reg, uint8_t *pdata, uint16_t size)
{
    if (htps_i2c == NULL)
    {
        return TPS55289_ERROR;
    }
    HAL_StatusTypeDef hal_ret = HAL_I2C_Mem_Write(htps_i2c, (htps55289_addr << 1), reg, I2C_MEMADD_SIZE_8BIT, pdata, size, TPS55289_MAX_DELAY);
    if (hal_ret != HAL_OK)
    {
        return TPS55289_I2C_ERROR;
    }
    return TPS55289_OK;
}

static tps55289_status_t read_reg(uint8_t reg, uint8_t *pdata, uint16_t size)
{
    if (htps_i2c == NULL)
    {
        return TPS55289_ERROR;
    }
    HAL_StatusTypeDef hal_ret = HAL_I2C_Mem_Read(htps_i2c, (htps55289_addr << 1), reg, I2C_MEMADD_SIZE_8BIT, pdata, size, TPS55289_MAX_DELAY);
    if (hal_ret != HAL_OK)
    {
        return TPS55289_I2C_ERROR;
    }
    return TPS55289_OK;
}

/* -------------------------------------------------------------------------- */
/*                     Public API Implementation                              */
/* -------------------------------------------------------------------------- */

tps55289_status_t TPS55289_Init(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == NULL)
    {
        return TPS55289_INVALID_PARAM;
    }
    // htps55289_addr = i2c_addr;

    // 可选：复位芯片到默认状态，如果有硬件复位引脚的话
    //    if (write_reg(TPS55289_REG_OE_DISCHG, 1, 1) != TPS55289_OK)
    //    {
    //        return TPS55289_ERROR;
    //    }
    // 可选：写默认设置，比如输出反馈方式，slew rate，电流限制等
    // 例如，将 OE 设为 0（不使能输出），放电关闭
    uint8_t mod_buf = 0x80;
    uint8_t cdc_reg = 0x07;
    uint8_t intfb_reg = 0x03; // 使用 INTFB 模式    

    if (write_reg(TPS55289_REG_CDC, &cdc_reg, 1) != TPS55289_OK)
    {
        return TPS55289_ERROR;
    }
    // 对 TPS55289_REG_OE_DISCHG 寄存器操作：假设 OE 在某位 (例如 bit0)，DISCHG 在某位 (例如 bit1)
    // buf = (0 << 0) | (0 << 1);
    if (write_reg(TPS55289_REG_OE_DISCHG, &mod_buf, 1) != TPS55289_OK)
    {
        // printf("SET TPS55289_REG_OE_DISCHG ERROR");
        return TPS55289_ERROR;
    }
        if (write_reg(TPS55289_REG_INTFB_EXTFB, &intfb_reg, 1) != TPS55289_OK)
    {
        // printf("SET TPS55289_REG_OE_DISCHG ERROR");
        return TPS55289_ERROR;
    }
    
    // printf("Init TPS55289 OK!");
    return TPS55289_OK;
}

tps55289_status_t TPS55289_SetVoltage(uint16_t ref_value)
{
    // REF 寄存器是 16 位（MSB + LSB）: 低字节先写，再写高字节
    uint8_t lsb = (uint8_t)(ref_value & 0xFFU);
    uint8_t msb = (uint8_t)((ref_value >> 8) & 0xFFU);

    if (write_reg(TPS55289_REG_REF_LSB, &lsb, 1) != TPS55289_OK)
    {

        return TPS55289_ERROR;
    }
    if (write_reg(TPS55289_REG_REF_MSB, &msb, 1) != TPS55289_OK)
    {
        return TPS55289_ERROR;
    }

    return TPS55289_OK;
}
/**
 * @brief  目标电压 → 自动换算 TPS55289 REF 寄存器值
 * @param  vout  目标输出电压（单位：mV）
 * @return  REF 寄存器结构体
 *  ** 5V-->ref=0x01A4
    ** 6V-->ref=0x0204
    ** 7V-->ref=0x0268
    ** 8V-->ref=0x02CC
    ** 9V-->ref=0x0332
    **10V-->ref=0x0396
    **11V-->ref=0x0400
 */
void TPS55289_SetVoltage_Calc(uint16_t vout_mV)
{
//    uint32_t vref_uV;
//    int32_t N;
//    // 1. 反推 VREF
//    vref_uV = vout_mV * INTFB_TABLE[3]; 
//    // 2. 计算 11-bit DAC 值    
//    N = (int32_t)((vref_uV - TPS_REF_MIN_MV*1000 ) / TPS_REF_STEP_uV + 0.5f); // 四舍五入
//    // 3. 限幅
//    if (N < 0)     N = 0;
//    if (N > 2047)  N = 2047;
    int32_t N;

    // 1. 限定输出范围（20V 档）
    if (vout_mV < 800)
        vout_mV = 800;

    if (vout_mV > 20000)
        vout_mV = 20000;

    // 2. 按 TPS55289 官方 20V 档公式计算 REF
    // VOUT = 800 + 10 * REF
    N = (vout_mV - 800) / 10;   // +5 是整数四舍五入

    // 3. 限幅（11bit DAC）
    if (N < 0)     N = 0;
    if (N > 2047)  N = 2047;

    // 4. 写入芯片
    TPS55289_SetVoltage(N);    
}

tps55289_status_t TPS55289_SetCurrentLimit(tps55289_iout_limit_t limit_enum)
{
    uint8_t val;

    switch (limit_enum)
    {
    case TPS55289_IOUT_LIMIT_MIN:
        val = 0x00; // 根据手册映射
        break;
    case TPS55289_IOUT_LIMIT_MID:
        val = 0x80; // 示例值
        break;
    case TPS55289_IOUT_LIMIT_MAX:
        val = 0xFF; // 示例值
        break;
    default:
        return TPS55289_INVALID_PARAM;
    }

    if (write_reg(TPS55289_REG_IOUT_LIMIT, &val, 1) != TPS55289_OK)
    {
        return TPS55289_ERROR;
    }
    return TPS55289_OK;
}

tps55289_status_t TPS55289_Enable(bool enable)
{
    uint8_t buf;
    // 读当前 OE_DISCHG 寄存器
    if (read_reg(TPS55289_REG_OE_DISCHG, &buf, 1) != TPS55289_OK)
    {
        return TPS55289_ERROR;
    }

    if (enable)
    {
        buf |= (1 << 0); // 假设 OE 是位 0
    }
    else
    {
        buf &= ~(1 << 0);
    }

    if (write_reg(TPS55289_REG_OE_DISCHG, &buf, 1) != TPS55289_OK)
    {
        return TPS55289_ERROR;
    }

    return TPS55289_OK;
}

tps55289_status_t TPS55289_Discharge(bool discharge)
{
    uint8_t buf;
    if (read_reg(TPS55289_REG_OE_DISCHG, &buf, 1) != TPS55289_OK)
    {
        return TPS55289_ERROR;
    }

    if (discharge)
    {
        buf |= (1 << 1); // 假设 DISCHG 是位 1
    }
    else
    {
        buf &= ~(1 << 1);
    }

    if (write_reg(TPS55289_REG_OE_DISCHG, &buf, 1) != TPS55289_OK)
    {
        return TPS55289_ERROR;
    }

    return TPS55289_OK;
}

tps55289_status_t TPS55289_GetVoltage(uint16_t *ref_value)
{
    if (ref_value == NULL)
    {
        return TPS55289_INVALID_PARAM;
    }

    uint8_t lsb, msb;
    if (read_reg(TPS55289_REG_REF_LSB, &lsb, 1) != TPS55289_OK)
    {
        return TPS55289_ERROR;
    }
    if (read_reg(TPS55289_REG_REF_MSB, &msb, 1) != TPS55289_OK)
    {
        return TPS55289_ERROR;
    }

    *ref_value = ((uint16_t)msb << 8) | (uint16_t)lsb;
    return TPS55289_OK;
}

tps55289_status_t TPS55289_WriteReg(uint8_t reg, uint8_t *pdata, uint16_t size)
{
    return write_reg(reg, pdata, size);
}

tps55289_status_t TPS55289_ReadReg(uint8_t reg, uint8_t *pdata, uint16_t size)
{
    return read_reg(reg, pdata, size);
}

#if TPS55289_SHELL == 1
/* ========== 命令实现 ========== */

// 初始化（可根据需要配置默认寄存器）
int cmd_tps_init(void)
{
    printf("TPS55289 init done\r\n");
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 tps_init, cmd_tps_init, "Init TPS55289");

// 打开输出（EN 引脚控制或寄存器使能）
// int cmd_tps_on(void)
//{
//    // 假设控制 EN 在寄存器 0x01 bit0
//    uint8_t val = TPS55289_ReadReg(0x01);
//    TPS55289_WriteReg(0x01, val | 0x01);
//    printf("TPS55289 output ON\r\n");
//    return 0;
//}
// SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//    tps_on, cmd_tps_on, "Enable TPS55289 output");

// int cmd_tps_off(void)
//{
//     uint8_t val = TPS55289_ReadReg(0x01);
//     TPS55289_WriteReg(0x01, val & ~0x01);
//     printf("TPS55289 output OFF\r\n");
//     return 0;
// }
// SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//     tps_off, cmd_tps_off, "Disable TPS55289 output");

// 设置输出电压
int cmd_tps_set_v(uint16_t mv)
{
    // VOUT = (0.8V + DAC*10mV)，假设映射关系：DAC = (Vout-800)/10
    uint8_t dac = (mv - 800) / 10;
    TPS55289_WriteReg(0x02, &dac, sizeof(dac)); // 假设寄存器0x02为电压设定
    printf("Set Vout = %dmV\r\n", mv);
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 tps_set_v, cmd_tps_set_v, "Set TPS55289 output voltage (mV)");

// 设置限流
int cmd_tps_set_i(uint16_t ma)
{
    // 假设 I_LIMIT = ma/100 映射到寄存器0x03
    uint8_t lim = ma / 100;
    //    TPS55289_WriteReg(0x03, lim,sizeof(lim));
    printf("Set I_limit = %dmA\r\n", ma);
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 tps_set_i, cmd_tps_set_i, "Set TPS55289 current limit (mA)");

// 读写寄存器
int cmd_tps_r(uint8_t reg)
{
    uint8_t i = 0;
    // uint8_t val = TPS55289_ReadReg(reg,i,1);
    // printf("Reg[0x%02X] = 0x%02X\r\n", reg, val);
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 tps_r, cmd_tps_r, "Read TPS55289 register");

// int cmd_tps_w(uint8_t reg, uint8_t val)
//{
//     TPS55289_WriteReg(reg, &val,sizeof(val));
//     printf("Write Reg[0x%02X] = 0x%02X\r\n", reg, val);
//     return 0;
// }
// SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//     tps_w, cmd_tps_w, "Write TPS55289 register");
#endif
