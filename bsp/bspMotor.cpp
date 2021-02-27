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

// 电机使能、方向 引脚对应状态
#define MONTOR_ON         PIN_HIGH//PIN_LOW
#define MONTOR_OFF        PIN_LOW//PIN_HIGH
#define MONTOR_CW         PIN_HIGH
#define MONTOR_ACW        PIN_LOW

// 电机控制句柄
CCtrlMotor g_hdl_motor0;
CCtrlMotor g_hdl_motor1;

// 是否已初始化
uint8_t CCtrlMotor::beValide(void)
{
    if (m_valide) {
        return 1;
    }
    return 0;
}

/* 电机控制器初始化
 * en 使能引脚    dir 方向引脚  channel pwm通道号
 */
void CCtrlMotor::init(uint16_t en, uint16_t dir, int channel)
{
    // 引脚及通道初始化
    m_pinEn = en;
    m_pinDir = dir;
    m_pPwm = g_pwm_dev;
    m_pwmChl = channel;
    rt_pin_mode(m_pinEn, PIN_MODE_OUTPUT);
    rt_pin_write(m_pinEn, MONTOR_OFF);
    rt_pin_mode(m_pinDir, PIN_MODE_OUTPUT);
    rt_pin_write(m_pinDir, MONTOR_CW);
    m_valide = 1;
}
// 使能电机
void CCtrlMotor::turnOn(void)
{
    if (beValide()) {
        rt_pin_write(m_pinEn, MONTOR_ON);
    }
}
// 关停电机
void CCtrlMotor::turnOff(void)
{
    if (beValide()) {
        rt_pin_write(m_pinEn, MONTOR_OFF);
    }
}
// 设置电机转速  speed /ns
void CCtrlMotor::setSpeed(rt_int32_t speed)
{
    rt_uint32_t abs = 0;
    abs = ABS(speed);
    if (beValide()) {
        if (speed > 0) {
            rt_pin_write(m_pinDir, MONTOR_CW);
        }
        else {
            rt_pin_write(m_pinDir, MONTOR_ACW);
        }
        // TODO 速度转换为脉冲周期
        rt_pwm_set(m_pPwm, m_pwmChl, abs, abs/2); // 输出50%方波
        rt_pwm_enable(m_pPwm, m_pwmChl);
    }
}
// 设置电机方向
void CCtrlMotor::setDir(uint8_t dir)
{
    if (beValide()) {
        if (dir) {
            rt_pin_write(m_pinDir, MONTOR_CW);
        }
        else {
            rt_pin_write(m_pinDir, MONTOR_ACW);
        }
    }
}
// 上电初始化电机控制器
static int motor_init(void)
{
    // 初始化电机控制引脚
    // PWM通道采用timer8,通道分别为1、2
    g_hdl_motor0.init(RT_PWM0_EN, RT_PWM0_DIR, 1);
    g_hdl_motor1.init(RT_PWM1_EN, RT_PWM1_DIR, 2);

    return 0;
}
INIT_ENV_EXPORT(motor_init);

// TODO encoder thread
#define THREAD_NAME_MOTOR         "motor count"
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
            pMotor = &g_hdl_motor0;
        }
        else if (1== chl) {
            pMotor = &g_hdl_motor1;
        }
        else {
            goto _exit;
        }

        if (!strcmp(argv[2], "on")) {
            pMotor->turnOn();
        }
        else if (!strcmp(argv[2], "off")) {
            pMotor->turnOff();
        }
        else {
            goto _exit;
        }

        if (argc >= 4) {
            dir = atoi(argv[3]);
            pMotor->setDir(dir);
        }

        if (argc >= 5) {
            speed = atoi(argv[4]);
            pMotor->setSpeed(speed); // ns
        }
    }
    else {
        rt_kprintf("Usage:\n");
        rt_kprintf("test_motor <idx> <on/off> |dir| |speed|      - set motor on/off with speed(ns)\n");
        rt_kprintf("valide idx is [0,1]");
    }
    return;

_exit:
    rt_kprintf("err input\n");
}
MSH_CMD_EXPORT(test_motor, Test Fun: motor control);

#endif
#endif
