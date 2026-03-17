
// -*- coding: utf-8 -*-
/**
 * @file    drv_rtc.h
 * @author  Your Name (your.email@example.com)
 * @brief   Brief description of this header file
 * @version 1.0.0
 * @date    2025-09-19
 *
 * @copyright
 * Copyright (c) 2025 Your Company. All rights reserved.
 */

#ifndef __DRV_KEYS_H__
#define __DRV_KEYS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */
/*                                  Includes                                  */
/* -------------------------------------------------------------------------- */
#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx_hal.h"
/* -------------------------------------------------------------------------- */
/*                                 Definitions                                */
/* -------------------------------------------------------------------------- */
/**
 * @brief Example macro description
 */
#define KEY_COUNT				4

#define KEY_1					0
#define KEY_2					1
#define KEY_3					2
#define KEY_PWR					3   //key4 is sys key

#define KEY_HOLD				0x01
#define KEY_DOWN				0x02
#define KEY_UP					0x04
#define KEY_SINGLE				0x08
#define KEY_DOUBLE				0x10
#define KEY_LONG				0x20
#define KEY_REPEAT				0x40

/* -------------------------------------------------------------------------- */
/*                                   Typedefs                                 */
/* -------------------------------------------------------------------------- */
/**
 * @brief Example structure description
 */


/* -------------------------------------------------------------------------- */
/*                              Function Prototypes                            */
/* -------------------------------------------------------------------------- */
/**
 * @brief Initialize the example module
 */
uint8_t Key_GetState(uint8_t n);
uint8_t Key_Check(uint8_t n, uint8_t Flag);
void Key_Tick(void);


#ifdef __cplusplus
}
#endif

#endif /* __EXAMPLE_H__ */
