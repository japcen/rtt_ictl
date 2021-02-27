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

/* 初始化LED序列
 * *pPinList 引脚队列    *pIniList 初始值队列,RT_NULL时为默认拉高
 * num LED数量
 */
void CCtrlLED::init(rt_uint16_t* pPinList, rt_uint8_t* pIniList, rt_uint8_t num)
{
    if (RT_NULL != m_pLed) {
        rt_free(m_pLed);
    }

    m_pLed = (led_blink_struct*)rt_malloc(sizeof(led_blink_struct)*num);
    if (RT_NULL != m_pLed) {
        rt_memset(m_pLed, 0x00, (sizeof(led_blink_struct)*num));

        CCtrlGpioDO::init(pPinList, pIniList, num);
    }
}
/* 开启LED闪烁
 * onms 亮时长(ms)    offms 灭时长(ms)
 * .discribe 需要轮询outputDO
 */
void CCtrlLED::blinkOn(rt_uint8_t idx, rt_uint32_t onms, rt_uint32_t offms)
{
    if (idx >= m_num) return;

    m_pLed[idx].onTick = rt_tick_from_millisecond(onms);
    m_pLed[idx].offTick = rt_tick_from_millisecond(offms);
    m_pLed[idx].lstSysTick = rt_tick_get();
    m_pLed[idx].tick = 0;
    m_pLed[idx].trig = 1;
}
/* 关闭LED闪烁
 */
void CCtrlLED::blinkOff(rt_uint8_t idx)
{
    rt_uint8_t turnoff = 0;

    if (idx >= m_num) return;

    if ((m_pLed[idx].onTick) && (m_pLed[idx].offTick)) {
        turnoff = 1;
    }

    m_pLed[idx].onTick = 0;
    m_pLed[idx].offTick = 0;
    m_pLed[idx].lstSysTick = 0;
    m_pLed[idx].tick = 0;
    m_pLed[idx].trig = 0;

    if (turnoff) {
        setRtDO(idx, BSP_LED_OFF);
    }
}
/* LED常亮
 */
void CCtrlLED::on(rt_uint8_t idx)
{
    if (idx >= m_num) return;

    blinkOff(idx);
    setRtDO(idx, BSP_LED_ON);
}
/* LED常灭
 */
void CCtrlLED::off(rt_uint8_t idx)
{
    if (idx >= m_num) return;

    blinkOff(idx);
    setRtDO(idx, BSP_LED_OFF);
}
/* LED翻转
 */
void CCtrlLED::toggle(rt_uint8_t idx)
{
    if (idx >= m_num) return;

    blinkOff(idx);
    toggleDO(idx);
}
/* LED状态生效
 */
void CCtrlLED::outputDO(rt_uint8_t idx, rt_uint32_t systick)
{
    rt_uint8_t i = idx;
    rt_uint8_t num = idx + 1;
    rt_uint8_t status = 0;
    rt_uint8_t nextStatus = 0;

    if (0xFF == idx) {
        i = 0;
        num = m_num;
    }

    for (; i<num; i++) {
        if ((!m_pLed[i].onTick) && (!m_pLed[i].offTick)) {
            CCtrlGpioDO::outputDO(i);
        }

        if (systick < m_pLed[i].lstSysTick) {
            m_pLed[i].lstSysTick = systick;
            m_pLed[i].tick = 0;
            continue;
        }

        m_pLed[i].tick += (systick - m_pLed[i].lstSysTick);
        status = getRtDO(i);
        nextStatus = status;
        if (BSP_LED_ON == status) { // on->off
            if (0 != m_pLed[i].offTick) {
                if (m_pLed[i].trig) {
                    nextStatus = BSP_LED_OFF;
                }
                else if (m_pLed[i].tick > m_pLed[i].onTick) {
                    nextStatus = BSP_LED_OFF;
                }
            }
        }
        else { // off->on
            if (0 != m_pLed[i].onTick) {
                if (m_pLed[i].trig) {
                    nextStatus = BSP_LED_ON;
                }
                else if (m_pLed[i].tick > m_pLed[i].offTick) {
                    nextStatus = BSP_LED_ON;
                }
            }
        }

        if (nextStatus != status) {
            setRtDO(i, nextStatus);
            m_pLed[i].tick = 0;
        }
        m_pLed[i].lstSysTick = systick;
        m_pLed[i].trig = 0;
    }
}
