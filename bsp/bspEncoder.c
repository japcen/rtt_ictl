/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-02-15     BEN       the first version
 */

#include <rtthread.h>
#include <board.h>
#include <bsp/includes/bspEncoder.h>

struct ctrlEncoder g_ctrlEnc[MAX_ENCODER];

void getEncoder(void)
{
    g_ctrlEnc[0].dir = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim4);
    g_ctrlEnc[0].val = __HAL_TIM_GET_COUNTER(&htim4) + g_ctrlEnc[0].circle*MAX_TIMVAL;

    g_ctrlEnc[1].dir = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim1);
    g_ctrlEnc[1].val = __HAL_TIM_GET_COUNTER(&htim1) + g_ctrlEnc[1].circle*MAX_TIMVAL;
}
