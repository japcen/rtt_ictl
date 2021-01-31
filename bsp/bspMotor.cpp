/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-28     BEN       the first version
 */

#include <rtthread.h>
#define DBG_TAG "bspMontor"
#include <usrPublic.h>
extern "C"
{
#include <board.h>
#include <rtdevice.h>
}
#include <bsp/includes/bspMotor.h>

#define MONTOR_ON         PIN_LOW
#define MONTOR_OFF        PIN_HIGH
#define MONTOR_CW         PIN_HIGH
#define MONTOR_ACW        PIN_LOW

CCtrlMotor g_montor0;
CCtrlMotor g_montor1;

uint8_t CCtrlMotor::inited(void)
{
    if (valid) {
        return 1;
    }
    return 0;
}

void CCtrlMotor::init(uint16_t en, uint16_t dir, int channel)
{
    en_pin = en;
    dir_pin = dir;
    pPwm = g_pwm_dev;
    ch = channel;
    rt_pin_mode(en_pin, PIN_MODE_OUTPUT);
    rt_pin_write(en_pin, MONTOR_OFF);
    rt_pin_mode(dir_pin, PIN_MODE_OUTPUT);
    rt_pin_write(dir_pin, MONTOR_CW);
    valid = 1;
}
void CCtrlMotor::turnon(void)
{
    if (inited()) {
        rt_pin_write(en_pin, MONTOR_ON);
    }
}
void CCtrlMotor::turnoff(void)
{
    if (inited()) {
        rt_pin_write(en_pin, MONTOR_OFF);
    }
}
// speed /ns
void CCtrlMotor::speed(rt_int32_t speed)
{
    rt_uint32_t abs = 0;
    abs = ABS(speed);
    if (inited()) {
        if (speed > 0) {
            rt_pin_write(dir_pin, MONTOR_CW);
        }
        else {
            rt_pin_write(dir_pin, MONTOR_ACW);
        }
        // TODO 速度转换为脉冲周期
        rt_pwm_set(pPwm, ch, abs, abs/2); // 输出50%方波
        rt_pwm_enable(pPwm, ch);
    }
}
void CCtrlMotor::dir(uint8_t dir)
{
    if (inited()) {
        if (dir) {
            rt_pin_write(dir_pin, MONTOR_CW);
        }
        else {
            rt_pin_write(dir_pin, MONTOR_ACW);
        }
    }
}
// 上电初始化电机控制器
static int motor_init(void)
{
    g_montor0.init(RT_PWM0_EN, RT_PWM0_DIR, 1); // 使用tim8通道1
    g_montor1.init(RT_PWM1_EN, RT_PWM1_DIR, 2); // 使用tim8通道2

    return 0;
}
INIT_ENV_EXPORT(motor_init);

// TODO encoder thread
#define THREAD_NAME_MOTOR         "mont count"
#define THREAD_PRIO_MOTOR          20
#define THREAD_SIZE_MOTOR          1024
#define THREAD_TIMESLICE_MOTOR     5
static rt_uint8_t thread_stack_motor[THREAD_SIZE_MOTOR];
static struct rt_thread tid_Taskmotor;

void taskmotor_entry(void *parameter)
{
    for (;;) {
        rt_thread_mdelay(2000);
    }
}

static int threadbegin_motor_count(void)
{
    rt_thread_init(&tid_Taskmotor,
                   THREAD_NAME_MOTOR,
                   taskmotor_entry,
                   RT_NULL,
                   thread_stack_motor,
                   THREAD_SIZE_MOTOR,
                   THREAD_PRIO_MOTOR,
                   THREAD_TIMESLICE_MOTOR);
    rt_thread_startup(&tid_Taskmotor);

    return 0;
}
INIT_APP_EXPORT(threadbegin_motor_count);


#ifdef DBG_MOD
#ifdef RT_USING_FINSH

void test_motor(int argc, char *argv[])
{
    rt_uint8_t chl = 0;
    rt_int32_t speed = 0;
    uint8_t    dir = 0;
    CCtrlMotor *pMotor = RT_NULL;

    if (argc >= 3) {
        chl = atoi(argv[1]);
        if (0 == chl) {
            pMotor = &g_montor0;
        }
        else if (1== chl) {
            pMotor = &g_montor1;
        }
        else {
            goto _exit;
        }

        if (!strcmp(argv[2], "on")) {
            pMotor->turnon();
        }
        else if (!strcmp(argv[2], "off")) {
            pMotor->turnoff();
        }
        else {
            goto _exit;
        }

        if (argc >= 4) {
            dir = atoi(argv[3]);
            pMotor->dir(dir);
        }

        if (argc >= 5) {
            speed = atoi(argv[4]);
            pMotor->speed(speed); // ns
        }
    }
    else {
        rt_kprintf("Usage:\n");
        rt_kprintf("test_motor <idx> <on/off> |dir| |speed|      - set motor on/off with speed(ns)\n");
        rt_kprintf("useful idx is [0,1]");
    }
    return;

_exit:
    rt_kprintf("err input\n");
}
MSH_CMD_EXPORT(test_motor, Test Fun: motor control);

#endif
#endif
