/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-12-31     BEN       the first version
 */
#ifndef BSP_INCLUDES_BSPSPI_H_
#define BSP_INCLUDES_BSPSPI_H_

extern "C"
{
#include "spi_flash_sfud.h"
#include <bsp/drivers/cs5532.h>
}

#define W25Q80_DEV_NAME       "W25Q80DV"
extern sfud_flash_t           g_dev_w25q80;
#define W25Q80_SFUD_HDL       g_dev_w25q80

#endif /* BSP_INCLUDES_TASKSPI_H_ */
