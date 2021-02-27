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
#include <bsp/includes/bspI2C.h>
#define DBG_TAG "demoI2C"
#include <usrPublic.h>

#ifdef DBG_MOD
#ifdef RT_USING_FINSH
rt_uint8_t ads1115_show = 0;  // 测试:打印ads1115采样值标识
#endif
#endif

// ads1115采样线程
void taski2c1_entry(void *parameter)
{
    rt_uint16_t result = 0;
    float t1 = 0.0f;
    uint8_t cnt = 0;
    uint8_t i = 0;
    rt_err_t rtn = RT_EOK;
    // 轮询采样通道为0-3,1-3,2-3
    #define MAX_ADS1115_CHL 3   // 轮询通道数
    #define MAX_SAMP_CNT    5   // 每通道采样次数
    /* ADS_MUX_xx 通道说明：
     * 0:ECH 1:EC0 2:EC1 3:EC_L
     */
    rt_uint8_t chn[MAX_ADS1115_CHL] = {ADS_MUX_03, ADS_MUX_13, ADS_MUX_23};

    rt_thread_mdelay(5000);

    // 测试 每组通道采集5次
    for (;;) {
        if (0 == cnt) {
            // 通道切换 ads1115采集通道初始化
            rtn = ads111x_config(ADS111x_HDL, ADS_SPD_128SPS|ADS_QUE_DISABLE, ADS_OS_BEGIN|chn[i]|ADS_PGA_4096V|ADS_MOD_CONTINUOUS);
            rt_thread_mdelay(1000);
        }

        if (RT_EOK == rtn) {
            cnt++;
            result = ads111x_readAD(ADS111x_HDL);
            t1 = 4.096f*result/0x7FFF;

            #ifdef DBG_MOD
            #ifdef RT_USING_FINSH
            // 打印采集数据
            if (ads1115_show) {
                if (1 == cnt) {
                    if (chn[i] == ADS_MUX_03) {
                        rt_kprintf("\nads1115 ech-ecl******\n");
                    }
                    else if (chn[i] == ADS_MUX_13) {
                        rt_kprintf("\nads1115 ec0-ecl******\n");
                    }
                    else if (chn[i] == ADS_MUX_23) {
                        rt_kprintf("\nads1115 ec1-ecl******\n");
                    }
                    else {
                        rt_kprintf("\nads1115 %d******\n", i);
                    }
                }
                rt_kprintf(" read:%d vol:%d mv\n", result, (int32_t)(t1*1000 + 0.5f));
            }
            #endif
            #endif
        }

        if ((RT_EOK != rtn) || (cnt >= MAX_SAMP_CNT)) {
            cnt = 0;
            i++;
            i = i%MAX_ADS1115_CHL;
        }

        rt_thread_mdelay(5000);
    }
}

// 上电启动ads1115采样线程
#define THREAD_NAME_I2C1            "taski2c_1"
#define THREAD_PRIO_TASKI2C1        20
#define THREAD_SIZE_TASKI2C1        1024
#define THREAD_TIMESLICE_TASKI2C1   5
static rt_uint8_t thread_stack_taski2c1[THREAD_SIZE_TASKI2C1];
static struct rt_thread tid_Taski2c1;
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
// ads1115采样打印值打印/关闭
void test_ads1115(int argc, char *argv[])
{
    rt_uint8_t noerr = 0;

    if (argc == 2) {
        if (!strcmp(argv[1], "on")) {
            ads1115_show = 1;
            noerr = 1;
        }
        else if (!strcmp(argv[1], "off")) {
            ads1115_show = 0;
            noerr = 1;
        }
        else {
            rt_kprintf("param err...\n");
        }
    }

    if (!noerr) {
        rt_kprintf("Usage:\n");
        rt_kprintf("ads1115 <on/off>       - test ad1115 and print voltage\n");
    }
}
MSH_CMD_EXPORT(test_ads1115, Test Fun: on/off ads1115 value print);

#endif
#endif
