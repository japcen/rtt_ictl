/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-02-15     BEN       the first version
 */
#ifndef BSP_INCLUDES_BSPENCODER_H_
#define BSP_INCLUDES_BSPENCODER_H_

#include <rtthread.h>

#define MAX_ENCODER    2
struct ctrlEncoder
{
    rt_uint8_t dir;
    rt_int32_t val;
    rt_int8_t  circle;
};
extern struct ctrlEncoder g_ctrlEnc[MAX_ENCODER];

extern void getEncoder(void);

#endif /* BSP_INCLUDES_BSPENCODER_H_ */
