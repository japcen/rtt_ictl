/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-12-31     BEN       the first version
 */

#include <rtthread.h>
#define DBG_TAG __FILE__
#include <usrPublic.h>
extern "C"
{
#include <bsp/includes/bspSPI.h>
#include <drv_spi.h>
}
#include "board.h"

// 上电初始化w25q
#define W25Q80_SPI_BUS_NAME              "spi1"
#define W25Q80_SPI_DEV_NAME              "spi_w25q"
sfud_flash_t g_dev_w25q80                = RT_NULL;
static int rt_hw_spi_flash_init(void)
{
    /* 往总线 spi1 上挂载一个从设备 */
    rt_hw_spi_device_attach(W25Q80_SPI_BUS_NAME, W25Q80_SPI_DEV_NAME, SPI1_CS_PORT, SPI1_CS_PIN);

    /* 使用 SFUD 探测 spi10 从设备，并将 spi10 连接的 flash 初始化为块设备，名称 W25Q80DV */
    if (RT_NULL == rt_sfud_flash_probe("W25Q80DV", W25Q80_SPI_DEV_NAME))
    {
        return -RT_ERROR;
    };

    return RT_EOK;
}
INIT_COMPONENT_EXPORT(rt_hw_spi_flash_init);


// 上电初始化cs5532设备
struct rt_spi_device *g_dev_cs0;
struct rt_spi_device *g_dev_cs1;
struct rt_spi_device *g_dev_cs2;
#define CS5532_DEV0_HDL                     g_dev_cs0
#define CS5532_DEV1_HDL                     g_dev_cs1
#define CS5532_DEV2_HDL                     g_dev_cs2

#define CS5532_SPI_BUS_NAME                 "spi3"
#define CS5532_SPI_DEV0_NAME                "spi_cs0"
#define CS5532_SPI_DEV1_NAME                "spi_cs1"
#define CS5532_SPI_DEV2_NAME                "spi_cs2"
static int rt_hw_spi_ai_init(void)
{
    struct rt_spi_configuration cfg;

    rt_hw_spi_device_attach(CS5532_SPI_BUS_NAME, CS5532_SPI_DEV0_NAME, SPI3_CS0_Port, SPI3_CS0_Pin);
    rt_hw_spi_device_attach(CS5532_SPI_BUS_NAME, CS5532_SPI_DEV1_NAME, SPI3_CS1_Port, SPI3_CS1_Pin);
    rt_hw_spi_device_attach(CS5532_SPI_BUS_NAME, CS5532_SPI_DEV2_NAME, SPI3_CS2_Port, SPI3_CS2_Pin);

    cfg.data_width = 8;
    cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_MSB;
    cfg.max_hz = 3200;                           /* 3200 */

    CS5532_DEV0_HDL = (struct rt_spi_device *)rt_device_find(CS5532_SPI_DEV0_NAME);
    if (RT_NULL != CS5532_DEV0_HDL) {
        rt_spi_configure(CS5532_DEV0_HDL, &cfg);
    }
    CS5532_DEV1_HDL = (struct rt_spi_device *)rt_device_find(CS5532_SPI_DEV1_NAME);
    if (RT_NULL != CS5532_DEV1_HDL) {
        rt_spi_configure(CS5532_DEV1_HDL, &cfg);
    }
    CS5532_DEV2_HDL = (struct rt_spi_device *)rt_device_find(CS5532_SPI_DEV2_NAME);
    if (RT_NULL != CS5532_DEV2_HDL) {
        rt_spi_configure(CS5532_DEV2_HDL, &cfg);
    }

    return RT_EOK;
}
INIT_APP_EXPORT(rt_hw_spi_ai_init);

#ifdef DBG_MOD
#ifdef RT_USING_FINSH

static struct rt_thread tid_testCs5532;
static rt_uint8_t thread_stack_test[1024];
void test5532_entry(void *parameter)
{
    cs5532_test_conrd();
}

void test_cs5532(int argc, char *argv[])
{
    char devName[RT_NAME_MAX] = {0};

    if (argc > 2) {
        switch (atoi(argv[2])) {
        case 0:
            rt_strncpy(devName, CS5532_SPI_DEV0_NAME, RT_NAME_MAX);
            break;
        case 1:
            rt_strncpy(devName, CS5532_SPI_DEV1_NAME, RT_NAME_MAX);
            break;
        case 2:
            rt_strncpy(devName, CS5532_SPI_DEV2_NAME, RT_NAME_MAX);
            break;
        default:
            rt_kprintf("cmd err:cs5532 channel must be [0~2]\n");
            return;
        }
    }

    if (argc == 2) {

    }
    if (argc == 3) {
        if (!strcmp(argv[1], "reg")) {
            cs5532_test_regrd(devName);
        }
        else if (!strcmp(argv[1], "sig")) {
            cs5532_test_singlerd(devName);
        }
        else if (!strcmp(argv[1], "con")) {
            rt_thread_t testid5532 = RT_NULL;
            if (!strcmp(argv[2], "on")) {
                testid5532 = rt_thread_find((char*)"test5532");
                if (RT_NULL != testid5532) {
                    rt_thread_detach(testid5532);
                    rt_thread_mdelay(5);
                }
                rt_thread_init(&tid_testCs5532,
                               "test5532",
                               test5532_entry,
                               RT_NULL,
                               thread_stack_test,
                               1024,
                               20,
                               5);
                rt_thread_startup(&tid_testCs5532);
            }
            else if (!strcmp(argv[2], "off")) {
                testid5532 = rt_thread_find((char*)"test5532");
                if (RT_NULL != testid5532) {
                    rt_thread_detach(testid5532);
                }
            }
        }
    }
    else {
        rt_kprintf("Usage:\n");
        rt_kprintf("test_cs5532 reg <idx>            - test cs5532 reg write and read\n");
        rt_kprintf("test_cs5532 sig <idx>            - test cs5532 single convert\n");
        rt_kprintf("test_cs5532 con <on/off>         - test cs5532 all channels continue convert\n");
    }
}
MSH_CMD_EXPORT(test_cs5532, Test Fun: test cs5532);

#endif
#endif

