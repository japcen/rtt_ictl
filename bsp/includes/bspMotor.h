/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-28     BEN       the first version
 */
#ifndef BSP_INCLUDES_BSPMOTOR_H_
#define BSP_INCLUDES_BSPMOTOR_H_

#include <bsp/includes/bspDIO.h>
#include <bsp/includes/bspPwm.h>

class CCtrlMotor
{
public:
    uint16_t m_pinEn;                                 // 使能引脚
    uint16_t m_pinDir;                                // 方向引脚
    int      m_pwmChl;                                // pwm通道号
    struct rt_device_pwm *m_pPwm;                     // pwm控制
    uint8_t  m_valide;                                // 初始化标识
public:
    void init(uint16_t en, uint16_t dir, int ch);    // 电机控制器初始化
    void turnOn(void);                               // 使能电机
    void turnOff(void);                              // 失能电机
    void setSpeed(rt_int32_t speed);                 // 设置转速
    void setDir(uint8_t dir);                        // 设置方向
    uint8_t beValide(void);                          // 控制器是否已初始化
public:
    CCtrlMotor() {
        m_valide = 0;
    }
    ~CCtrlMotor() {
    }
};

extern CCtrlMotor g_hdl_motor0;
extern CCtrlMotor g_hdl_motor1;

#endif /* BSP_INCLUDES_BSPMOTOR_H_ */
