/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-02-26     BEN       the first version
 */
#ifndef BSP_INCLUDES_BSPHCRC32_H_
#define BSP_BSPHCRC32_H_

#include <rtthread.h>
extern "C"
{
#include <board.h>
}

uint32_t stm_softCRC32(uint32_t *ptr, uint32_t len);
uint32_t stm_hardCRC32(CRC_HandleTypeDef *hcrc, uint32_t pBuffer[], uint32_t BufferLength);

#endif /* BSP_INCLUDES_BSPHCRC32_H_ */
