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

// do
void CCtrlGpioDo::init(rt_uint16_t* pinlst, rt_uint8_t* pInilst, rt_uint8_t num)
{
    if (RT_NULL != lstPin) {
        maxNum = 0;
        DO_SET = 0;
        rt_free(lstPin);
    }

    lstPin = (rt_uint16_t*)rt_malloc(sizeof(rt_uint16_t)*num);
    if (pinlst != RT_NULL) {
        maxNum = num;
        for (rt_uint8_t i = 0; i<num; i++) {
            lstPin[i] = pinlst[i];
            ////DO_SET |= 1<<i;
            ////immSetDO(i, PIN_HIGH);
            // CMEDIT 在board.c中初始化
            rt_pin_mode(pinlst[i], PIN_MODE_OUTPUT);
            if (RT_NULL != pInilst) { // 按初始值启动
                immSetDO(i, pInilst[i]);
            }
            else { // 未设初始值时默认拉高
                immSetDO(i, PIN_HIGH);
            }
        }
    }
}

void CCtrlGpioDo::setDO(rt_uint8_t idx, rt_uint8_t state)
{
    if (0xFF == idx) {
        if (PIN_LOW == state) {
            DO_SET = 0;
        }
        else {
            DO_SET = 0xFFFF;
        }
    }
    else {
        if (PIN_LOW == state) {
            DO_SET &= ~(1<<idx);
        }
        else {
            DO_SET |= (1<<idx);
        }
    }
}
void CCtrlGpioDo::immSetDO(rt_uint8_t idx, rt_uint8_t state)
{
    if (idx >= maxNum) return;

    if (PIN_LOW == state) {
        DO_SET &= ~(1<<idx);
        rt_pin_write(lstPin[idx], PIN_LOW);
    }
    else {
        DO_SET |= (1<<idx);
        rt_pin_write(lstPin[idx], PIN_HIGH);
    }
}
void CCtrlGpioDo::toggleDO(rt_uint8_t idx)
{
    if (idx >= maxNum) return;

    if (PIN_HIGH == rt_pin_read(lstPin[idx])) {
        DO_SET &= ~(1<<idx);
        rt_pin_write(lstPin[idx], PIN_LOW);
    }
    else {
        DO_SET |= (1<<idx);
        rt_pin_write(lstPin[idx], PIN_HIGH);
    }
}
rt_uint8_t CCtrlGpioDo::getDO(rt_uint8_t idx)
{
    if (DO_SET & (1<<idx)) {
        return PIN_HIGH;
    }
    else {
        return PIN_LOW;
    }
}
rt_uint8_t CCtrlGpioDo::getimmDO(rt_uint8_t idx)
{
    return rt_pin_read(lstPin[idx]);
}
rt_uint16_t CCtrlGpioDo::showDO(void)
{
    return DO_SET;
}
void CCtrlGpioDo::outputDO(rt_uint8_t idx)
{
    rt_uint8_t i = idx;
    rt_uint8_t num = idx + 1;
    if (0xFF == idx) {
        i = 0;
        num = maxNum;
    }

    if (num > maxNum) {
        return;
    }

    for (; i<num; i++) {
        rt_pin_write(lstPin[i], getDO(i));
    }
}

// di
void CCtrlGpioDi::init(rt_uint16_t* pinlst, rt_uint8_t num)
{
    if (RT_NULL != lstPin) {
        maxNum = 0;
        DI_DAT = 0;
        rt_free(lstPin);
    }

    lstPin = (rt_uint16_t*)rt_malloc(sizeof(rt_uint16_t)*num);
    if (lstPin != RT_NULL)
    {
        for (rt_uint8_t i; i<num; i++)
        {
            lstPin[i] = pinlst[i];
            rt_pin_mode(lstPin[i], PIN_MODE_INPUT);
        }
        maxNum = num;
    }
}

void CCtrlGpioDi::setDI(rt_uint8_t idx, rt_uint8_t state)
{
    if (0xFF == idx) {
        if (PIN_LOW == state) {
            DI_DAT = 0;
        }
        else {
            DI_DAT = 0xFF;
        }
    }
    else {
        if (PIN_LOW == state) {
            DI_DAT &= ~(1<<idx);
        }
        else {
            DI_DAT |= (1<<idx);
        }
    }
}
rt_uint8_t CCtrlGpioDi::getDI(rt_uint8_t idx)
{
    if (DI_DAT & (1<<idx)) {
        return PIN_HIGH;
    }
    else {
        return PIN_LOW;
    }
}
rt_uint8_t CCtrlGpioDi::showDI(void)
{
    return DI_DAT;
}

void CCtrlGpioDi::updataDI(void)
{
    for (rt_uint8_t i=0; i<maxNum; i++) {
        setDI(i, rt_pin_read(lstPin[i]));
    }
}

