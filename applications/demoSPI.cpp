/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-02-01     BEN       the first version
 */

#include <rtthread.h>
#define DBG_TAG "demoSPI"
#include <usrPublic.h>
#include <bsp/includes/bspSPI.h>

#ifdef DBG_MOD
#ifdef RT_USING_FINSH
// w25q存储测试
void test_w25q(int argc, char *argv[])
{
    sfud_err result;
    rt_uint8_t *read_data;
    rt_uint8_t *write_data;
    rt_uint8_t randvalue = 0;

    #define TEST_W25Q_LEN    32

    W25Q80_SFUD_HDL = rt_sfud_flash_find_by_dev_name(W25Q80_DEV_NAME);
    if (RT_NULL == W25Q80_SFUD_HDL) {
        rt_kprintf("no w25q80 dev!!!\n");
        return;
    }

    result = sfud_erase(W25Q80_SFUD_HDL, 0, TEST_W25Q_LEN);
    if (SFUD_SUCCESS != result) {
        rt_kprintf("w25q80 erase err %d!!!\n", result);
        return;
    }

    // 向flash地址0写入随机数并读取
    write_data = (rt_uint8_t*)rt_malloc(TEST_W25Q_LEN);
    randvalue = (rt_uint8_t)rand();
    rt_memset(write_data, randvalue, TEST_W25Q_LEN);
    sfud_write(W25Q80_SFUD_HDL, 0, TEST_W25Q_LEN, write_data);
    rt_kprintf("write %02X x32 to w25q80, then read...\n", randvalue);

    read_data = (rt_uint8_t*)rt_malloc(TEST_W25Q_LEN);
    sfud_read(W25Q80_SFUD_HDL, 0, TEST_W25Q_LEN, read_data);
    DbgPrintDataLst((char*)"w25q80 read data is", (char*)read_data, TEST_W25Q_LEN, 16);
}
MSH_CMD_EXPORT(test_w25q, Test Fun: test w25q flash);

#endif
#endif

// cs5532范例
// .see bapSPI.cpp/test_cs5532()
