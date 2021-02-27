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
#define DBG_TAG "demoDIO"
#include <usrPublic.h>
//extern "C"
//{
#include <rtdef.h>
#include <rtdevice.h>
//}
#include "board.h"
#include <bsp/includes/bspDIO.h>
#include <bsp/includes/bspLed.h>

// DO序列引脚配置
uint16_t DO_PIN[] = {RT_DO0_Pin, RT_DO1_Pin, RT_DO2_Pin, RT_DO3_Pin};
// DI序列引脚配置
uint16_t DI_PIN[] = {RT_DI0_Pin, RT_DI1_Pin, RT_DI2_Pin, RT_DI3_Pin};
// LED序列引脚 初始状态 配置
uint16_t LED_PIN[] = {RT_LED0_Pin, RT_LED1_Pin, RT_LED2_Pin};
uint8_t  LED_INI[] = {BSP_LED_OFF, BSP_LED_OFF, BSP_LED_OFF};

// DO DI LED 控制变量
CCtrlGpioDO g_hdl_do;
CCtrlGpioDI g_hdl_di;
CCtrlLED g_hdl_led;

// DO DI LED状态轮询线程
void taskDio_entry(void *parameter)
{
    // 设备初始化
    g_hdl_do.init(DO_PIN, RT_NULL, sizeof(DO_PIN)/sizeof(DO_PIN[0]));
    g_hdl_di.init(DI_PIN, sizeof(DI_PIN)/sizeof(DI_PIN[0]));
    g_hdl_led.init(LED_PIN, LED_INI, sizeof(LED_PIN)/sizeof(LED_PIN[0]));

    rt_thread_mdelay(500);

    // 按200ms周期轮询
    while (1) {
        rt_thread_mdelay(200);
        g_hdl_do.outputDO(0xFF);
        g_hdl_di.updataDI();
        g_hdl_led.outputDO(0xFF, rt_tick_get());
    }
}

#define THREAD_NAME_DIO             "taskdio"
#define THREAD_PRIO_TASKIDIO        20
#define THREAD_SIZE_TASKIDIO        1024
#define THREAD_TIMESLICE_TASKDIO    5
static rt_uint8_t thread_stack_taskdio[THREAD_SIZE_TASKIDIO];
static struct rt_thread tid_Taskdio;
static int threadbegin_dio(void)
{
    rt_thread_init(&tid_Taskdio,
                   THREAD_NAME_DIO,
                   taskDio_entry,
                   RT_NULL,
                   thread_stack_taskdio,
                   THREAD_SIZE_TASKIDIO,
                   THREAD_PRIO_TASKIDIO,
                   THREAD_TIMESLICE_TASKDIO);
    rt_thread_startup(&tid_Taskdio);

    return 0;
}
INIT_APP_EXPORT(threadbegin_dio);

#ifdef DBG_MOD
#ifdef RT_USING_FINSH
// DO序列测试函数
void test_do(int argc, char *argv[])
{
    if (3 == argc) {
        g_hdl_do.setRtDO(atoi(argv[1]), atoi(argv[2]));
    }
    else {
        rt_kprintf("Usage:\n");
        rt_kprintf("test_do <idx> <state>    - set do state. idx:[0~3], state 0:off 1:on\n");
    }
}
MSH_CMD_EXPORT(test_do, Test Fun: test do list);

#endif
#endif

#ifdef DBG_MOD
#ifdef RT_USING_FINSH
// LED序列测试
void test_led(int argc, char *argv[])
{
    rt_uint8_t noerr = 0;
    rt_uint32_t idx = 0;

    if (argc >= 2) {
        idx = atoi(argv[1]);
        if (idx > 2) {
            rt_kprintf("cmd error:led idx is [0~2]\n");
            return;
        }
    }

    if (5 == argc) {
        // 闪烁测试
        if (!strcmp(argv[2], "blinkon")) {
            g_hdl_led.blinkOn(idx, atoi(argv[3]), atoi(argv[4]));
            noerr = 1;
        }
    }
    else if (3 == argc) {
        // 关闭闪烁
        if (!strcmp(argv[2], "blinkoff")) {
            g_hdl_led.blinkOff(idx);
            noerr = 1;
        }
        // 常亮
        else if (!strcmp(argv[2], "on")) {
            g_hdl_led.on(idx);
            noerr = 1;
        }
        // 常灭
        else if (!strcmp(argv[2], "off")) {
            g_hdl_led.off(idx);
            noerr = 1;
        }
        // 翻转
        else if (!strcmp(argv[2], "toggle")) {
            g_hdl_led.toggle(idx);
            noerr = 1;
        }
    }

    if (!noerr) {
        rt_kprintf("Usage:\n");
        rt_kprintf("test_led <idx> blinkon <ontime> <offtime>   - set led blink freq (ms)\n");
        rt_kprintf("test_led <idx> blinkoff                     - trun off blink\n");
        rt_kprintf("test_led <idx> on                           - trun on\n");
        rt_kprintf("test_led <idx> off                          - trun off\n");
        rt_kprintf("test_led <idx> toggle                       - toggle\n");
    }
}
MSH_CMD_EXPORT(test_led, Test Fun: test led control);

#endif
#endif
