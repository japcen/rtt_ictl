/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-02-15     BEN       the first version
 */

#include <rtthread.h>
extern "C"
{
#include <bsp/includes/bspEncoder.h>
}
#define DBG_TAG "demoEnc"
#include <usrPublic.h>

#ifdef DBG_MOD
#ifdef RT_USING_FINSH
rt_uint8_t encoder_show = 0;  // 测试:打印encoder采样值标识
#endif
#endif

// encoder采样线程
void taskEnc_entry(void *parameter)
{
    for (;;) {
        rt_thread_mdelay(200);
        getEncoder();
        if (encoder_show) {
            rt_thread_mdelay(2800);
            rt_kprintf("encoder:(%d)%d (%d)%d\r\n", g_ctrlEnc[0].dir, g_ctrlEnc[0].val, g_ctrlEnc[1].dir, g_ctrlEnc[1].val);
        }
    }
}

// 上电启动encoder5采样线程
#define THREAD_NAME_ENC             "taskEnc"
#define THREAD_PRIO_TASKENC         20
#define THREAD_SIZE_TASKENC         1024
#define THREAD_TIMESLICE_TASKENC    5
static rt_uint8_t thread_stack_taskenc[THREAD_SIZE_TASKENC];
static struct rt_thread tid_Taskenc;
static int threadbegin_enc(void)
{
    rt_thread_init(&tid_Taskenc,
                   THREAD_NAME_ENC,
                   taskEnc_entry,
                   RT_NULL,
                   thread_stack_taskenc,
                   THREAD_SIZE_TASKENC,
                   THREAD_PRIO_TASKENC,
                   THREAD_TIMESLICE_TASKENC);
    rt_thread_startup(&tid_Taskenc);

    return 0;
}
INIT_APP_EXPORT(threadbegin_enc);

#ifdef DBG_MOD
#ifdef RT_USING_FINSH
// encoder5采样打印值打印/关闭
void test_encoder(int argc, char *argv[])
{
    rt_uint8_t noerr = 0;

    if (argc == 2) {
        if (!strcmp(argv[1], "on")) {
            encoder_show = 1;
            noerr = 1;
        }
        else if (!strcmp(argv[1], "off")) {
            encoder_show = 0;
            noerr = 1;
        }
        else {
            rt_kprintf("param err...\n");
        }
    }

    if (!noerr) {
        rt_kprintf("Usage:\n");
        rt_kprintf("test_encoder <on/off>       - test encoder and print voltage\n");
    }
}
MSH_CMD_EXPORT(test_encoder, Test Fun: on/off encoder value print);

#endif
#endif

