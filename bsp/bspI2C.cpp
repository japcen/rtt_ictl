#include <bsp/includes/bspI2C.h>
/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-12-31     BEN       the first version
 */

// 启动时初始化ds1115设备
ad111x_device_t g_dev_ads111x      = RT_NULL;
#define ADS111x_I2C_BUS_NAME       "i2c1"
static int taski2c1_ini(void)
{
    if (RT_NULL != ADS111x_HDL)
    {
        ads111x_deinit(ADS111x_HDL);
    }
    ADS111x_HDL = ads111x_init(ADS111x_I2C_BUS_NAME, ADS111x_ADD);

    return 0;
}
INIT_DEVICE_EXPORT(taski2c1_ini);


// 启动时初始化at24c设备
at24cxx_device_t g_dev_at24cxx      = RT_NULL;
#define AT24cxx_I2C_BUS_NAME        "i2c3"
static int taski2c3_ini(void)
{
    if (RT_NULL != AT24cxx_HDL)
    {
        at24cxx_deinit(AT24cxx_HDL);
    }
    AT24cxx_HDL = at24cxx_init(AT24cxx_I2C_BUS_NAME, AT24CXX_ADDR);

    return 0;
}
INIT_DEVICE_EXPORT(taski2c3_ini);
