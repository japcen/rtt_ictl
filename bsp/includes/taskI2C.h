/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-12-31     BEN       the first version
 */
#ifndef BSP_INCLUDES_TASKI2C_H_
#define BSP_INCLUDES_TASKI2C_H_

extern "C"
{
#include <bsp/drivers/ads111x.h>
#include <packages/at24cxx-latest/at24cxx.h>
}

extern at24cxx_device_t g_dev_at24cxx;
#define AT24cxx_HDL                  g_dev_at24cxx

#endif /* BSP_INCLUDES_TASKI2C_H_ */
