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

// DO序列控制类
class CCtrlGpioDO
{
protected:
    rt_uint16_t  m_doSet;
    rt_uint16_t* m_pPins;
    rt_uint8_t   m_num;
public:
    void init(rt_uint16_t* pPinList, rt_uint8_t* pIniList, rt_uint8_t num);
    void setDO(rt_uint8_t idx, rt_uint8_t state);
    void setRtDO(rt_uint8_t idx, rt_uint8_t state);
    void toggleDO(rt_uint8_t idx);
    rt_uint8_t getDO(rt_uint8_t idx);
    rt_uint8_t getRtDO(rt_uint8_t idx);
    rt_uint16_t showDO(void);
    void outputDO(rt_uint8_t idx);
public:
    CCtrlGpioDO() {
        m_pPins = RT_NULL;
        m_doSet = 0;
        m_num = 0;
    }
    ~CCtrlGpioDO() {
        if (RT_NULL != m_pPins) rt_free(m_pPins);
        m_doSet = 0;
        m_num = 0;
    }
};

// DI序列控制类
class CCtrlGpioDI
{
protected:
    rt_uint16_t  m_diDat;
    rt_uint16_t* m_pPins;
    rt_uint8_t   m_num;
    void setDI(rt_uint8_t idx, rt_uint8_t state);
public:
    void init(rt_uint16_t* pPinList, rt_uint8_t num);
    rt_uint8_t getRtDI(rt_uint8_t idx);
    rt_uint8_t showDI(void);
    void updataDI(void);
public:
    CCtrlGpioDI() {
        m_pPins = RT_NULL;
        m_diDat = 0;
        m_num = 0;
    }
    ~CCtrlGpioDI() {
        if (RT_NULL != m_pPins) rt_free(m_pPins);
        m_diDat = 0;
        m_num = 0;
    }
};

#endif /* BSP_INCLUDES_BSPDIO_H_ */
