// -*- coding: utf-8 -*-
/**
 * @file    drv_led.c
 * @author  Thonelyne
 * @brief   Implementation of the example module
 * @version 1.0.0
 * @date    2025-11-22
 *
 * @copyright
 * Copyright (c) 2025 Your Company. All rights reserved.
 */

/* -------------------------------------------------------------------------- */
/*                                  Includes                                  */
/* -------------------------------------------------------------------------- */
#include "drv_leds.h"
#include "stm32f0xx.h"                  // Device header
#include "main.h"
//#include <stdio.h>
#include "tim.h"
/* -------------------------------------------------------------------------- */
/*                             Private Definitions                             */
/* -------------------------------------------------------------------------- */
//#define EXAMPLE_INTERNAL_CONST   (42)
#define BREATHE_TIME 4000
uint16_t breathe_val = 0;
int8_t breathe_dir = 1;   // 1 = 变亮，-1 = 变暗

/* -------------------------------------------------------------------------- */
/*                             Private Global Vars                             */
/* -------------------------------------------------------------------------- */
//static int example_internal_state = 0;

/* -------------------------------------------------------------------------- */
/*                          Private Function Prototypes                        */
/* -------------------------------------------------------------------------- */
//static void example_internal_helper(void);

/* -------------------------------------------------------------------------- */
/*                           Public Function Definitions                       */
/* -------------------------------------------------------------------------- */
void LED_Init(void)
{
    
}
/* -------------------------------------------------------------------------- */
/*                           Private Function Definitions                       */
/* -------------------------------------------------------------------------- */
void Breathe_Task(void)
{
    breathe_val += breathe_dir;

    if (breathe_val >= BREATHE_TIME-1)
    {
        breathe_val = BREATHE_TIME-1;
        breathe_dir = -1;
    }
    else if (breathe_val == 0)
    {
        breathe_dir = 1;
    }

    __HAL_TIM_SET_COMPARE(&htim14, TIM_CHANNEL_1, breathe_val*1000/BREATHE_TIME);
}
