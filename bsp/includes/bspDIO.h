/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-12-30     BEN       the first version
 */
#ifndef BSP_INCLUDES_BSPDIO_H_
#define BSP_INCLUDES_BSPDIO_H_

class CCtrlGpioDo
{
protected:
    rt_uint16_t  DO_SET;
    rt_uint16_t* lstPin;
    rt_uint8_t   maxNum;
public:
    void init(rt_uint16_t* pinlst, rt_uint8_t* pInilst, rt_uint8_t num);
    void setDO(rt_uint8_t idx, rt_uint8_t state);
    void immSetDO(rt_uint8_t idx, rt_uint8_t state);
    void toggleDO(rt_uint8_t idx);
    rt_uint8_t getDO(rt_uint8_t idx);
    rt_uint8_t getimmDO(rt_uint8_t idx);
    rt_uint16_t showDO(void);
    void outputDO(rt_uint8_t idx);
public:
    CCtrlGpioDo() {
        lstPin = RT_NULL;
        DO_SET = 0;
        maxNum = 0;
    }
    ~CCtrlGpioDo() {
        if (RT_NULL != lstPin) rt_free(lstPin);
        DO_SET = 0;
        maxNum = 0;
    }
};

class CCtrlGpioDi
{
protected:
    rt_uint16_t  DI_DAT;
    rt_uint16_t* lstPin;
    rt_uint8_t   maxNum;
public:
    void init(rt_uint16_t* pinlst, rt_uint8_t num);
    void setDI(rt_uint8_t idx, rt_uint8_t state);
    rt_uint8_t getDI(rt_uint8_t idx);
    rt_uint8_t showDI(void);
    void updataDI(void);
public:
    CCtrlGpioDi() {
        lstPin = RT_NULL;
        DI_DAT = 0;
        maxNum = 0;
    }
    ~CCtrlGpioDi() {
        if (RT_NULL != lstPin) rt_free(lstPin);
        DI_DAT = 0;
        maxNum = 0;
    }
};

#endif /* BSP_INCLUDES_BSPDIO_H_ */
