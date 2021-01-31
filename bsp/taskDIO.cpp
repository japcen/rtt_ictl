/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-12-30     BEN       the first version
 */

#include <rtthread.h>
#include <bsp/includes/taskDIO.h>
#define DBG_TAG "taskDIO"
#include <usrPublic.h>
extern "C"
{
#include <rtdef.h>
#include <rtdevice.h>
}
#include "board.h"

uint16_t DO_PIN[] = {RT_DO0_Pin, RT_DO1_Pin, RT_DO2_Pin, RT_DO3_Pin};
uint16_t DI_PIN[] = {RT_DI0_Pin, RT_DI1_Pin, RT_DI2_Pin, RT_DI3_Pin};
uint16_t LED_PIN[] = {RT_LED0_Pin, RT_LED1_Pin, RT_LED2_Pin};
uint8_t  LED_INI[] = {BSP_LED_OFF, BSP_LED_OFF, BSP_LED_OFF};

CCtrlGpioDo g_ctrlDO;
CCtrlGpioDi g_ctrlDI;
CCtrlLED g_ctrlLED;

void taskDio_entry(void *parameter)
{
    // init
    g_ctrlDO.init(DO_PIN, RT_NULL, sizeof(DO_PIN)/sizeof(DO_PIN[0]));
    g_ctrlDI.init(DI_PIN, sizeof(DI_PIN)/sizeof(DI_PIN[0]));
    g_ctrlLED.init(LED_PIN, LED_INI, sizeof(LED_PIN)/sizeof(LED_PIN[0]));

    rt_thread_mdelay(500);

    while (1) {
        rt_thread_mdelay(200);
        g_ctrlDO.outputDO(0xFF);
        g_ctrlDI.updataDI();
        g_ctrlLED.outputDO(0xFF, rt_tick_get());
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

void test_do(int argc, char *argv[])
{
    if (3 == argc) {
        g_ctrlDO.setDO(atoi(argv[1]), atoi(argv[2]));
    }
}
MSH_CMD_EXPORT(test_do, dddddooooo led blink state);

#endif
#endif

#ifdef DBG_MOD
#ifdef RT_USING_FINSH

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
        if (!strcmp(argv[2], "blinkon")) {
            g_ctrlLED.blinkon(idx, atoi(argv[3]), atoi(argv[4]));
            noerr = 1;
        }
    }
    else if (3 == argc) {
        if (!strcmp(argv[2], "blinkoff")) {
            g_ctrlLED.blinkoff(idx);
            noerr = 1;
        }
        else if (!strcmp(argv[2], "on")) {
            g_ctrlLED.on(idx);
            noerr = 1;
        }
        else if (!strcmp(argv[2], "off")) {
            g_ctrlLED.off(idx);
            noerr = 1;
        }
        else if (!strcmp(argv[2], "toggle")) {
            g_ctrlLED.toggle(idx);
            noerr = 1;
        }
    }

    if (!noerr) {
        rt_kprintf("Usage:\n");
        rt_kprintf("test_led <idx> blinkon <ontime> <offtime    - set led blink freq (ms)\n");
        rt_kprintf("test_led <idx> blinkoff                     - trun off blink\n");
        rt_kprintf("test_led <idx> on                           - trun on\n");
        rt_kprintf("test_led <idx> off                          - trun off\n");
        rt_kprintf("test_led <idx> toggle                       - toggle\n");
    }
}
MSH_CMD_EXPORT(test_led, set led blink state);

#endif
#endif
