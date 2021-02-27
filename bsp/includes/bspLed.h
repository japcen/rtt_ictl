/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-13     BEN       the first version
 */
#ifndef BSP_INCLUDES_BSPLED_H_
#define BSP_INCLUDES_BSPLED_H_

#include <rtthread.h>
extern "C"
{
#include <rtdevice.h>
}
#include <bsp/includes/bspDIO.h>

#define BSP_LED_ON          PIN_LOW
#define BSP_LED_OFF         PIN_HIGH

// Led闪烁控制结构
struct led_blink_struct{
    rt_uint32_t onTick;        // 亮时长
    rt_uint32_t offTick;       // 灭时长
    rt_uint32_t lstSysTick;    // 上一次时长累积
    rt_uint32_t tick;          // 时长累积
    rt_uint8_t  trig;          // 首次触发标识
};
// Led序列控制类
class CCtrlLED : public CCtrlGpioDO
{
private:
    struct led_blink_struct *m_pLed;
public:
    void init(rt_uint16_t* pPinList, rt_uint8_t* pIniList, rt_uint8_t num);
    void blinkOn(rt_uint8_t idx, rt_uint32_t onms, rt_uint32_t offms);
    void blinkOff(rt_uint8_t idx);
    void on(rt_uint8_t idx);
    void off(rt_uint8_t idx);
    void toggle(rt_uint8_t idx);
    void outputDO(rt_uint8_t idx, rt_uint32_t systick);
public:
    CCtrlLED() {
        m_pLed = RT_NULL;
    }
    ~CCtrlLED() {
        if (RT_NULL != m_pLed) rt_free(m_pLed);
    }
};

#endif /* BSP_INCLUDES_BSPLED_H_ */
