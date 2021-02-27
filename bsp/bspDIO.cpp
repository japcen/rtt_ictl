/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-12-30     BEN       the first version
 */

#include <rtthread.h>
extern "C"
{
#include <rtdevice.h>
}
#include <bsp/includes/bspDIO.h>

// DO
/* 初始化Do序列
 * *pPinList 引脚队列    *pIniList 初始值队列,RT_NULL时为默认拉高
 * num DO数量
 */
void CCtrlGpioDO::init(rt_uint16_t* pPinList, rt_uint8_t* pIniList, rt_uint8_t num)
{
    if (pPinList != RT_NULL) return;
    if (RT_NULL != m_pPins) {
        m_num = 0;
        m_doSet = 0;
        rt_free(m_pPins);
    }

    m_pPins = (rt_uint16_t*)rt_malloc(sizeof(rt_uint16_t)*num);
    if (RT_NULL != m_pPins) {
        m_num = num;
        for (rt_uint8_t i = 0; i<num; i++) {
            m_pPins[i] = pPinList[i];
            rt_pin_mode(pPinList[i], PIN_MODE_OUTPUT);
            if (RT_NULL != pIniList) { // 按初始值启动
                setRtDO(i, pIniList[i]);
            }
            else { // 未设初始值时默认拉高
                setRtDO(i, PIN_HIGH);
            }
        }
    }
}
/* 设置DO引脚状态(缓存)
 * idx 引脚序号[0~max] 0xFF:该序列所有DO
 * state 引脚状态 e.g PIN_LOW/PIN_HIGH
 * .describe DO状态不会立即生效,调用outputDO才实际输出
 */
void CCtrlGpioDO::setDO(rt_uint8_t idx, rt_uint8_t state)
{
    if (0xFF == idx) {
        if (PIN_LOW == state) {
            m_doSet = 0;
        }
        else {
            m_doSet = 0xFFFF;
        }
    }
    else {
        if (idx < m_num) {
            if (PIN_LOW == state) {
                m_doSet &= ~(1<<idx);
            }
            else {
                m_doSet |= (1<<idx);
            }
        }
    }
}
/* 设置DO引脚状态(实时)
 * idx 引脚序号[0~max]
 * state 引脚状态 e.g PIN_LOW/PIN_HIGH
 * .describe DO状态立即生效
 */
void CCtrlGpioDO::setRtDO(rt_uint8_t idx, rt_uint8_t state)
{
    if (idx >= m_num) return;

    if (PIN_LOW == state) {
        m_doSet &= ~(1<<idx);
        rt_pin_write(m_pPins[idx], PIN_LOW);
    }
    else {
        m_doSet |= (1<<idx);
        rt_pin_write(m_pPins[idx], PIN_HIGH);
    }
}
/* DO翻转(实时)
 * idx 引脚序号[0~max]
 */
void CCtrlGpioDO::toggleDO(rt_uint8_t idx)
{
    if (idx >= m_num) return;

    if (PIN_HIGH == rt_pin_read(m_pPins[idx])) {
        m_doSet &= ~(1<<idx);
        rt_pin_write(m_pPins[idx], PIN_LOW);
    }
    else {
        m_doSet |= (1<<idx);
        rt_pin_write(m_pPins[idx], PIN_HIGH);
    }
}
/* 获取DO状态(缓存)
 * idx 引脚序号[0~max]
 * return DO命令状态
 */
rt_uint8_t CCtrlGpioDO::getDO(rt_uint8_t idx)
{
    if (m_doSet & (1<<idx)) {
        return PIN_HIGH;
    }
    else {
        return PIN_LOW;
    }
}
/* 获取DO实际状态(实时)
 * idx 引脚序号[0~max]
 * return DO实际状态
 */
rt_uint8_t CCtrlGpioDO::getRtDO(rt_uint8_t idx)
{
    return rt_pin_read(m_pPins[idx]);
}
/* 获取DO系列全部引脚状态(缓存)
 * return
 */
rt_uint16_t CCtrlGpioDO::showDO(void)
{
    return m_doSet;
}
/* 输出DO状态
 * idx 引脚序号[0~max] 0xFF:该序列所有DO
 * .discribe 使DO缓存命令生效 DO动作
 */
void CCtrlGpioDO::outputDO(rt_uint8_t idx)
{
    rt_uint8_t i = idx;
    rt_uint8_t num = idx + 1;
    if (0xFF == idx) {
        i = 0;
        num = m_num;
    }

    if (num > m_num) {
        return;
    }

    for (; i<num; i++) {
        rt_pin_write(m_pPins[i], getDO(i));
    }
}

// di
/* 初始化Do序列
 * *pPinList 引脚队列    num DO数量
 */
void CCtrlGpioDI::init(rt_uint16_t* pinlst, rt_uint8_t num)
{
    if (RT_NULL != m_pPins) {
        m_num = 0;
        m_diDat = 0;
        rt_free(m_pPins);
    }

    m_pPins = (rt_uint16_t*)rt_malloc(sizeof(rt_uint16_t)*num);
    if (m_pPins != RT_NULL)
    {
        m_num = num;
        for (rt_uint8_t i=0; i<num; i++)
        {
            m_pPins[i] = pinlst[i];
            rt_pin_mode(m_pPins[i], PIN_MODE_INPUT);
        }
        updataDI();
    }
}
/* 设置DI数据状态
 * idx 引脚序号[0~max] 0xFF:该序列所有DO
 * state 引脚状态 e.g PIN_LOW/PIN_HIGH
 */
void CCtrlGpioDI::setDI(rt_uint8_t idx, rt_uint8_t state)
{
    if (0xFF == idx) {
        if (PIN_LOW == state) {
            m_diDat = 0;
        }
        else {
            m_diDat = 0xFF;
        }
    }
    else {
        if (PIN_LOW == state) {
            m_diDat &= ~(1<<idx);
        }
        else {
            m_diDat |= (1<<idx);
        }
    }
}
/* 获取DI实际状态
 * idx 引脚序号[0~max]
 * return DI实际状态
 */
rt_uint8_t CCtrlGpioDI::getRtDI(rt_uint8_t idx)
{
    if (idx >= m_num) return PIN_LOW;

    setDI(idx, rt_pin_read(m_pPins[idx]));
    if (m_diDat & (1<<idx)) {
        return PIN_HIGH;
    }
    else {
        return PIN_LOW;
    }
}
/* 获取DI系列全部引脚实际状态
 * return
 */
rt_uint8_t CCtrlGpioDI::showDI(void)
{
    updataDI();
    return m_diDat;
}
/* 更新DI引脚状态
 * return
 */
void CCtrlGpioDI::updataDI(void)
{
    for (rt_uint8_t i=0; i<m_num; i++) {
        setDI(i, rt_pin_read(m_pPins[i]));
    }
}

