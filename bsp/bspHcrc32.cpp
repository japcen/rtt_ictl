/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-02-26     BEN       the first version
 */

#include <bsp/includes/bspHcrc32.h>
#define DBG_TAG "bspHcrc32"
#include <usrPublic.h>

// 软件模拟硬件CRC32计算
uint32_t stm_softCRC32(uint32_t *ptr, uint32_t len)
{
    uint32_t    xbit;
    uint32_t    data;
    uint32_t    crc32 = 0xFFFFFFFF;
    uint32_t    dwPolynomial = 0x04c11db7;

    while (len--) {
        xbit = 1 << 31;

        data = *ptr++;
        for (int bits = 0; bits < 32; bits++) {
            if (crc32 & 0x80000000) {
                crc32 <<= 1;
                crc32 ^= dwPolynomial;
            }
            else
                crc32 <<= 1;
            if (data & xbit)
                crc32 ^= dwPolynomial;

            xbit >>= 1;
        }
    }
    return crc32;
}

// 硬件CRC32计算
uint32_t stm_hardCRC32(CRC_HandleTypeDef *hcrc, uint32_t pBuffer[], uint32_t BufferLength)
{
    return HAL_CRC_Calculate(hcrc, pBuffer, BufferLength);
}
