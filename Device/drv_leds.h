
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

#ifndef __DRV_LEDS_H__
#define __DRV_LEDS_H__

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
//#define EXAMPLE_MACRO   (100)

/* -------------------------------------------------------------------------- */
/*                                   Typedefs                                 */
/* -------------------------------------------------------------------------- */
/**
 * @brief Example structure description
 */
//typedef struct
//{
//    int id;             /**< ID of the example */
//    char name[32];      /**< Name of the example */
//} example_t;

/* -------------------------------------------------------------------------- */
/*                              Function Prototypes                            */
/* -------------------------------------------------------------------------- */
/**
 * @brief Initialize the example module
 */


void Breathe_Task(void);

#ifdef __cplusplus
}
#endif

#endif /* __EXAMPLE_H__ */
