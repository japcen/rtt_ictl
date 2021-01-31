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
    uint16_t en_pin;                                 // 使能引脚
    uint16_t dir_pin;                                // 方向引脚
    int      ch;                                     // pwm通道号
    struct rt_device_pwm *pPwm;                      // pwm控制
    uint8_t valid;                                   // 初始化标识
public:
    void init(uint16_t en, uint16_t dir, int ch);    // 电机控制器初始化
    void turnon(void);                               // 启动
    void turnoff(void);                              // 关停
    void speed(rt_int32_t speed);                    // 设置转速
    void dir(uint8_t dir);                           // 设置方向
    uint8_t inited(void);                            // 控制器是否已初始化
public:
    CCtrlMotor() {
        valid = 0;
    }
    ~CCtrlMotor() {
    }
};



#endif /* BSP_INCLUDES_BSPMOTOR_H_ */
