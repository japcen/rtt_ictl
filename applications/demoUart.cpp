/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-23     BEN       the first version
 */

#include <rtthread.h>
#define DBG_TAG "uartExample"
#include <usrPublic.h>
#include <stdlib.h>

#ifdef DBG_MOD
#ifdef RT_USING_FINSH
rt_uint8_t test_uart2_on = 0;
#endif
#endif

void taskUart_entry(void *parameter)
{
#ifdef DBG_MOD
#ifdef RT_USING_FINSH
    char str[] = "this is uart2 calling !!!\r\n";
#endif
#endif
    char rxBuf[256] = {0};
    rt_size_t rLen = 0;
    rt_device_t serial = RT_NULL;

    serial = rt_device_find("uart2");
    if (RT_EOK != rt_device_open(serial, RT_DEVICE_FLAG_INT_RX)) {
        rt_kprintf("dev uart2 open fail\n");
        return;
    }

    for (;;) {
        rLen = rt_device_read(serial, 0, rxBuf, 256);
        if (rLen > 0) {
            DbgPrintDataLst((char*)"uart recv:", rxBuf, rLen, 0);
        }
        rt_thread_mdelay(100);

#ifdef DBG_MOD
#ifdef RT_USING_FINSH
        // test uart send message
        if (test_uart2_on) {
            rt_device_write(serial, 0, str, (sizeof(str) - 1));
            DbgPrintDataLst((char*)"uart send:", str, (sizeof(str) - 1), 0);
            rt_thread_mdelay(4900);
        }
#endif
#endif
    }
}

#define THREAD_NAME_UART            "taskuart"
#define THREAD_PRIO_TASKUART        20
#define THREAD_SIZE_TASKUART        1024
#define THREAD_TIMESLICE_TASKUART   5
static rt_uint8_t thread_stack_taskuart[THREAD_SIZE_TASKUART];
static struct rt_thread tid_Taskuart;
static int threadbegin_uart(void)
{
    rt_thread_init(&tid_Taskuart,
                   THREAD_NAME_UART,
                   taskUart_entry,
                   RT_NULL,
                   thread_stack_taskuart,
                   THREAD_SIZE_TASKUART,
                   THREAD_PRIO_TASKUART,
                   THREAD_TIMESLICE_TASKUART);
    rt_thread_startup(&tid_Taskuart);

    return 0;
}
INIT_APP_EXPORT(threadbegin_uart);

#ifdef DBG_MOD
#ifdef RT_USING_FINSH
void uart_sendmsg(int argc, char *argv[])
{
    if (test_uart2_on) test_uart2_on = 0;
    else test_uart2_on = 1;
}
MSH_CMD_EXPORT(uart_sendmsg, Test Fun: send msg on uart2);

#endif
#endif
