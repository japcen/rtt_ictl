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
#include <bsp/includes/taskI2C.h>
#define DBG_TAG "taskI2C"
#include <usrPublic.h>

#define THREAD_NAME_I2C1            "taski2c_1"
#define THREAD_PRIO_TASKI2C1        20
#define THREAD_SIZE_TASKI2C1        1024
#define THREAD_TIMESLICE_TASKI2C1   5
static rt_uint8_t thread_stack_taski2c1[THREAD_SIZE_TASKI2C1];
static struct rt_thread tid_Taski2c1;

// ad1115 i2c demo
// 启动时初始化i2c_1总线
static int taski2c1_ini(void)
{
    if (RT_NULL != AD111x_HDL)
    {
        ads111x_deinit(AD111x_HDL);
    }
    AD111x_HDL = ads111x_init(ADS111x_I2C_BUS_NAME, ADS111x_ADD);

    return 0;
}
INIT_DEVICE_EXPORT(taski2c1_ini);

#ifdef DBG_MOD
#ifdef RT_USING_FINSH
rt_uint8_t test_ad1115 = {0};
#endif
#endif

// i2c总线轮询线程
void taski2c1_entry(void *parameter)
{
    rt_uint16_t result = 0;
    float t1 = 0.0f;
    uint8_t cnt = 0;
    uint8_t i = 0;
    rt_err_t rtn = RT_EOK;
    // 轮询采样通道为0-3,1-3,2-3
    #define MAX_AD1115_CHL 3   // 轮询通道数
    #define MAX_SAMP_CNT   5   // 每通道采样次数
    rt_uint8_t chn[MAX_AD1115_CHL] = {ADS_MUX_03, ADS_MUX_13, ADS_MUX_23};

    rt_thread_mdelay(5000);

    // 测试 每组通道采集5次
    for (;;) {
        if (0 == cnt) {
            // 通道切换 ads1115采集通道初始化
            rtn = ads111x_config(AD111x_HDL, ADS_SPD_128SPS|ADS_QUE_DISABLE, ADS_OS_BEGIN|chn[i]|ADS_PGA_4096V|ADS_MOD_CONTINUOUS);
            rt_thread_mdelay(1000);
        }

        if (RT_EOK == rtn) {
            cnt++;
            result = ads111x_readAD(AD111x_HDL);
            t1 = 4.096f*result/0x7FFF;

            #ifdef DBG_MOD
            #ifdef RT_USING_FINSH
            if (test_ad1115) {
                rt_kprintf("ad1115 ch(%d): %d ", i, result);
                rt_kprintf("vol: %d mv\n", (int32_t)(t1*1000 + 0.5f));
            }
            #endif
            #endif
        }

        if ((RT_EOK != rtn) || (cnt >= MAX_SAMP_CNT)) {
            cnt = 0;
            i++;
            i = i%MAX_AD1115_CHL;
        }

        rt_thread_mdelay(5000);
    }
}

static int threadbegin_i2c_1(void)
{
    rt_thread_init(&tid_Taski2c1,
                   THREAD_NAME_I2C1,
                   taski2c1_entry,
                   RT_NULL,
                   thread_stack_taski2c1,
                   THREAD_SIZE_TASKI2C1,
                   THREAD_PRIO_TASKI2C1,
                   THREAD_TIMESLICE_TASKI2C1);
    rt_thread_startup(&tid_Taski2c1);

    return 0;
}
INIT_APP_EXPORT(threadbegin_i2c_1);

#ifdef DBG_MOD
#ifdef RT_USING_FINSH
void ads1115(int argc, char *argv[])
{
    if (argc == 2) {
        if (!strcmp(argv[1], "on")) {
            test_ad1115 = 1;
        }
        else if (!strcmp(argv[1], "off")) {
            test_ad1115 = 0;
        }
        else {
            rt_kprintf("param err...\n");
        }
    }
    else {
        rt_kprintf("Usage:\n");
        rt_kprintf("ads1115 <on/off>       - test ad1115 and print voltage. idx:[0,1]\n");
    }
}
MSH_CMD_EXPORT(ads1115, Test Fun: test ad1115);

#endif
#endif


// at24 i2c3 demo
at24cxx_device_t g_dev_at24cxx      = RT_NULL;
#define AT24cxx_I2C_BUS_NAME        "i2c3"
#if 0
#define THREAD_NAME_I2C3            "taski2c_3"
#define THREAD_PRIO_TASKI2C3        20
#define THREAD_SIZE_TASKI2C3        1024
#define THREAD_TIMESLICE_TASKI2C3   5
static rt_uint8_t thread_stack_taski2c3[THREAD_SIZE_TASKI2C3];
static struct rt_thread tid_Taski2c3;

void taski2c3_entry(void *parameter)
{
    if (RT_NULL != AT24cxx_HDL)
    {
        at24cxx_deinit(AT24cxx_HDL);
    }
    AT24cxx_HDL = at24cxx_init(AT24cxx_I2C_BUS_NAME, AT24CXX_ADDR);
}

static int threadbegin_i2c_3(void)
{
    rt_thread_init(&tid_Taski2c3,
                   THREAD_NAME_I2C3,
                   taski2c3_entry,
                   RT_NULL,
                   thread_stack_taski2c3,
                   THREAD_SIZE_TASKI2C3,
                   THREAD_PRIO_TASKI2C3,
                   THREAD_TIMESLICE_TASKI2C3);
    rt_thread_startup(&tid_Taski2c3);

    return 0;
}
//INIT_APP_EXPORT(threadbegin_i2c_3);
#endif

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
