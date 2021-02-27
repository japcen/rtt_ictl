#include <bsp/includes/bspPwm.h>
/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-02     BEN       the first version
 */

#define DBG_TAG "bspPwm"
#include <usrPublic.h>
extern "C"
{
//#include <board.h>
#include <rtdevice.h>
}

#define PWM_NAME                   "pwm1"
struct rt_device_pwm *g_pwm_dev    = RT_NULL;

#if 0
#define THREAD_NAME_PWM            "taskpwm"
#define THREAD_PRIO_TASKPWM         20
#define THREAD_SIZE_TASKPWM         1024
#define THREAD_TIMESLICE_TASKPWM    5
static rt_uint8_t thread_stack_taskpwm[THREAD_SIZE_TASKPWM];
static struct rt_thread tid_Taskpwm;
void taskpwm_entry(void *parameter)
{
    g_pwm_dev = (struct rt_device_pwm*)rt_device_find(PWM_NAME);

    // TODO
}

static int threadbegin_pwm(void)
{
    rt_thread_init(&tid_Taskpwm,
                   THREAD_NAME_PWM,
                   taskpwm_entry,
                   RT_NULL,
                   thread_stack_taskpwm,
                   THREAD_SIZE_TASKPWM,
                   THREAD_PRIO_TASKPWM,
                   THREAD_TIMESLICE_TASKPWM);
    rt_thread_startup(&tid_Taskpwm);

    return 0;
}
INIT_ENV_EXPORT(threadbegin_pwm);
#endif
// pwm设备初始化
static int pwm_ini(void)
{
    g_pwm_dev = (struct rt_device_pwm*)rt_device_find(PWM_NAME);
    return 0;
}
INIT_DEVICE_EXPORT(pwm_ini);

#ifdef DBG_MOD
#ifdef RT_USING_FINSH
// 控制板PWM引脚输出测试
void test_pwm(int argc, char *argv[])
{
    rt_uint8_t chl = 0;
    rt_uint8_t noerr = 0;

    if (argc >= 4) {
        chl = atoi(argv[1]);
        g_pwm_dev = (struct rt_device_pwm*)rt_device_find(PWM_NAME);
        if (RT_NULL != g_pwm_dev) {
            rt_pwm_disable(g_pwm_dev, chl);
            rt_kprintf("test:send pwm chl(%d)... period:%d plus:%d\n", chl, atoi(argv[2]), atoi(argv[3]));
            rt_pwm_set(g_pwm_dev, chl, atoi(argv[2]), atoi(argv[3]));
            rt_pwm_enable(g_pwm_dev, chl);
            noerr = 1;
        }
    }

    if (!noerr) {
        rt_kprintf("Usage:\n");
        rt_kprintf("test_pwm <idx> <period> <plus>      - set pwm freq\n");
        rt_kprintf("         idx:[1or2], period plus: xms");
    }
}
MSH_CMD_EXPORT(test_pwm, Test Fun: pwm test);

#endif
#endif

