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

struct led_blink_struct{
    //rt_uint8_t  status; // 0 unvalid 1 blink
    rt_uint32_t onTick;
    rt_uint32_t offTick;
    rt_uint32_t lstSysTick;
    rt_uint32_t tick;
    rt_uint8_t  trig;
};
class CCtrlLED : public CCtrlGpioDo
{
private:
    struct led_blink_struct *led;
public:
    void init(rt_uint16_t* pinlst, rt_uint8_t* pInilst, rt_uint8_t num);
    void blinkon(rt_uint8_t idx, rt_uint32_t onms, rt_uint32_t offms);
    void blinkoff(rt_uint8_t idx);
    void on(rt_uint8_t idx);
    void off(rt_uint8_t idx);
    void toggle(rt_uint8_t idx);
    void outputDO(rt_uint8_t idx, rt_uint32_t systick);
public:
    CCtrlLED() {
        led = RT_NULL;
    }
    ~CCtrlLED() {
        if (RT_NULL != led) rt_free(led);
    }
};

#endif /* BSP_INCLUDES_BSPLED_H_ */
