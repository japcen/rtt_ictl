/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-12-31     BEN       the first version
 */
#ifndef BSP_INCLUDES_BSPI2C_H_
#define BSP_INCLUDES_TASKI2C_H_

extern "C"
{
#include <bsp/drivers/ads111x.h>
#include <at24cxx.h>
}

extern ad111x_device_t     g_dev_ads111x;
#define ADS111x_HDL        g_dev_ads111x
extern at24cxx_device_t    g_dev_at24cxx;
#define AT24cxx_HDL        g_dev_at24cxx

#endif /* BSP_INCLUDES_BSPI2C_H_ */
