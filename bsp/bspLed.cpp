/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-13     BEN       the first version
 */

#include <bsp/includes/bspLed.h>

void CCtrlLED::blinkon(rt_uint8_t idx, rt_uint32_t onms, rt_uint32_t offms)
{
    if (idx >= maxNum) return;

    led[idx].onTick = rt_tick_from_millisecond(onms);
    led[idx].offTick = rt_tick_from_millisecond(offms);
    led[idx].lstSysTick = rt_tick_get();
    led[idx].tick = 0;
    led[idx].trig = 1;
}
void CCtrlLED::blinkoff(rt_uint8_t idx)
{
    rt_uint8_t turnoff = 0;

    if (idx >= maxNum) return;

    if ((led[idx].onTick) && (led[idx].offTick)) {
        turnoff = 1;
    }

    led[idx].onTick = 0;
    led[idx].offTick = 0;
    led[idx].lstSysTick = 0;
    led[idx].tick = 0;
    led[idx].trig = 0;

    if (turnoff) {
        immSetDO(idx, BSP_LED_OFF);
    }
}
void CCtrlLED::on(rt_uint8_t idx)
{
    if (idx >= maxNum) return;

    blinkoff(idx);
    immSetDO(idx, BSP_LED_ON);
}
void CCtrlLED::off(rt_uint8_t idx)
{
    if (idx >= maxNum) return;

    blinkoff(idx);
    immSetDO(idx, BSP_LED_OFF);
}
void CCtrlLED::toggle(rt_uint8_t idx)
{
    if (idx >= maxNum) return;

    blinkoff(idx);
    toggleDO(idx);
}

void CCtrlLED::outputDO(rt_uint8_t idx, rt_uint32_t systick)
{
    rt_uint8_t i = idx;
    rt_uint8_t num = idx + 1;
    rt_uint8_t status = 0;
    rt_uint8_t nextStatus = 0;

    if (0xFF == idx) {
        i = 0;
        num = maxNum;
    }

    for (; i<num; i++) {
        if ((!led[i].onTick) && (!led[i].offTick)) {
            CCtrlGpioDo::outputDO(i);
        }

        if (systick < led[i].lstSysTick) {
            led[i].lstSysTick = systick;
            led[i].tick = 0;
            continue;
        }

        led[i].tick += (systick - led[i].lstSysTick);
        status = getimmDO(i);
        nextStatus = status;
        if (BSP_LED_ON == status) { // on->off
            if (0 != led[i].offTick) {
                if (led[i].trig) {
                    nextStatus = BSP_LED_OFF;
                }
                else if (led[i].tick > led[i].onTick) {
                    nextStatus = BSP_LED_OFF;
                }
            }
        }
        else { // off->on
            if (0 != led[i].onTick) {
                if (led[i].trig) {
                    nextStatus = BSP_LED_ON;
                }
                else if (led[i].tick > led[i].offTick) {
                    nextStatus = BSP_LED_ON;
                }
            }
        }

        if (nextStatus != status) {
            immSetDO(i, nextStatus);
            led[i].tick = 0;
        }
        led[i].lstSysTick = systick;
        led[i].trig = 0;
    }
}

void CCtrlLED::init(rt_uint16_t* pinlst, rt_uint8_t* pInilst, rt_uint8_t num)
{
    if (RT_NULL != led) {
        rt_free(led);
    }

    led = (led_blink_struct*)rt_malloc(sizeof(led_blink_struct)*num);
    if (RT_NULL != led) {
        rt_memset(led, 0x00, (sizeof(led_blink_struct)*num));

        CCtrlGpioDo::init(pinlst, pInilst, num);
    }
}
