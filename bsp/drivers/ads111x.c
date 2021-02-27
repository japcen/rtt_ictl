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
#include <rtdevice.h>
#include "board.h"

#include <string.h>
#include <stdlib.h>

#include "ads111x.h"

#define ADS111x_DATA_REG  0x00        //转换数据寄存器
#define ADS111x_CONF_REG  0x01        //控制字设置寄存器
#define ADS111x_LOTH_REG  0x02        //最低阀值寄存器
#define ADS111x_HITH_REG  0x03        //最高阀值寄存器

ad111x_device_t ads111x_init(const char *i2c_bus_name, uint8_t AddrInput)
{
    ad111x_device_t dev = RT_NULL;

    dev = rt_calloc(1, sizeof(struct ad111x_device));
    if (dev == RT_NULL) {
        return RT_NULL;
    }

    dev->i2c = rt_i2c_bus_device_find(i2c_bus_name);
    if (dev->i2c == RT_NULL) {
        rt_free(dev);
        return RT_NULL;
    }

    dev->AddrInput = AddrInput;

    return dev;
}

void ads111x_deinit(ad111x_device_t dev)
{
    if (RT_NULL != dev) {
        rt_free(dev);
    }
}

/*********************************************************************
*函数名称:  ads111x_config
*描       述： 设置ADS111x包括通道配置，采样时间等等
*参       数： HCMD ：命令字高8位(通道，量程，转换模式)
          LCMD : 命令字低8位(采样率设置 比较模式 有效电平 信号输出锁存)
*返       回； 无
*example:
*                   连续转换模式 MSB=84H LSB=83H
********************************************************************/
rt_err_t ads111x_config(ad111x_device_t dev, rt_uint8_t LCMD, rt_uint8_t HCMD)
{
    rt_uint8_t buf[3];

    if (RT_NULL == dev) {
        return RT_ERROR;
    }

    buf[0] = ADS111x_CONF_REG;
    buf[1] = HCMD;
    buf[2] = LCMD;
    if (3 != rt_i2c_master_send(dev->i2c, dev->AddrInput, RT_I2C_WR, buf, 3)) {
        return RT_ERROR;
    }

    return RT_EOK;
}

rt_err_t ads111x_setThresHold(ad111x_device_t dev, rt_uint16_t L_TH, rt_uint16_t H_TH)        //高低阀门设置
{
    rt_uint8_t buf[3];

    if (RT_NULL == dev) {
        return RT_ERROR;
    }

    buf[0] = ADS111x_LOTH_REG;    // 最低阀值寄存器
    buf[1] = (L_TH>>8);
    buf[2] = L_TH;
    if (3 != rt_i2c_master_send(dev->i2c, dev->AddrInput, RT_I2C_WR, buf, 3)) {
        return RT_ERROR;
    }

    buf[0] = ADS111x_HITH_REG;    // 最高阀值寄存器
    buf[1] = (H_TH>>8);
    buf[2] = H_TH;
    if (3 != rt_i2c_master_send(dev->i2c, dev->AddrInput, RT_I2C_WR, buf, 3)) {
        return RT_ERROR;
    }

    return RT_EOK;
}

// 补码计算
static rt_int16_t two_complement(rt_int16_t n)
{
    const rt_int16_t negative = (n & (1 << 15)) != 0;
    rt_int16_t native_int;

    if (negative)
      native_int = n | ~((1 << 16) - 1);
    else
      native_int = n;
    return native_int;
}

/*******************************************************************
*函数名称:  ads111x_readAD
*描       述： 获取AD转换的值
*参       数： 获取的值为在前面设置的那个通道
*返       回； 无
********************************************************************/
rt_uint16_t ads111x_readAD(ad111x_device_t dev)
{
    rt_uint8_t buf[2];
    rt_uint16_t rtn = 0;

    buf[0] = ADS111x_DATA_REG;
    if (1 != rt_i2c_master_send(dev->i2c, dev->AddrInput, RT_I2C_WR, buf, 1)) {
        return 0;
    }

    buf[0] = 0;
    buf[1] = 0;
    if (2 != rt_i2c_master_send(dev->i2c, dev->AddrInput, RT_I2C_RD, buf, 2)) {
        return 0;
    }

    rtn = two_complement((rt_int16_t)buf[0] << 8 | (rt_int16_t)buf[1]);
    return rtn;
}

#if 0
rt_uint16_t getad(rt_uint8_t LCMD, rt_uint8_t HCMD)
{
    rt_uint16_t value=0;
    ADS1115Config_A(LCMD, HCMD);          //配置通道

    delay_nms(5); // 延时一定时间，防止通道切换互相影响
    value = ReadAD_A();
    return value;
}

rt_uint16_t lvbo(rt_uint8_t LCMD, rt_uint8_t HCMD)        //求30个值的平均值
{
    rt_uint8_t k;
    rt_uint32_t U=0, temp;   //u32 给够叠加空间 或者float、double亦可
    for(k=0; k<30; k++)
    {
        U += getad(LCMD, HCMD);
    }
    temp=U;
    U=0;
    return ((float)temp/30);    //带上小数点
}
#endif
