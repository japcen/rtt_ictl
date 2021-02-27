/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-12-30     BEN       the first version
 */
#ifndef BSP_DRIVERS_ADS111X_H_
#define BSP_DRIVERS_ADS111X_H_

#include "board.h"

struct ad111x_device
{
    struct rt_i2c_bus_device *i2c;
    //rt_mutex_t lock;
    uint8_t AddrInput;
};
typedef struct ad111x_device *ad111x_device_t;
extern ad111x_device_t g_dev_ads111x;

//#define ADS111x_I2C_BUS_NAME     "i2c1"
//#define AD111x_HDL               g_dev_ad111x

#define ADS111x_ADD              (0x90 >> 1)             // 设备地址

/* 配置寄存器 MSB
 *
 */
// 工作模式 b15
#define ADS_OS_BEGIN  0x80

// 多路复用 b14-12
#define ADS_MUX_01    0x00 // defaut
#define ADS_MUX_03    0x10
#define ADS_MUX_13    0x20
#define ADS_MUX_23    0x30
#define ADS_MUX_0G    0x40
#define ADS_MUX_1G    0x50
#define ADS_MUX_2G    0x60
#define ADS_MUX_3G    0x70

// 增益放大 b11-9
#define ADS_PGA_6144V  0x00
#define ADS_PGA_4096V  0x02
#define ADS_PGA_2048V  0x04 // defaut
#define ADS_PGA_1024V  0x06
#define ADS_PGA_0512V  0x08
#define ADS_PGA_0256V  0x0E

// 转换模式 b8
#define ADS_MOD_CONTINUOUS  0x00
#define ADS_MOD_SINGLESHOT  0x01  // defaut

/* 配置寄存器 LSB
 *
 */
// 速率 b7-5
#define ADS_SPD_8SPS       0x00
#define ADS_SPD_16SPS      0x20
#define ADS_SPD_32SPS      0x40
#define ADS_SPD_64SPS      0x60
#define ADS_SPD_128SPS     0x80 // defaut
#define ADS_SPD_250SPS     0xA0
#define ADS_SPD_475SPS     0xC0
#define ADS_SPD_860SPS     0xE0

// 比较器模式 b4
#define ADS_CMOD_TRADITION 0x00 // defaut
#define ADS_CMOD_WINDOW    0x10

// 比较器极性 b3
#define ADS_CPOL_L         0x00 // defaut
#define ADS_CPOL_H         0x08

// 锁存比较器 b2
#define ADS_CLAT_L         0x00 // defaut
#define ADS_CLAT_H         0x04

// 比较器队列 b1-0
#define ADS_QUE_ONE        0x00
#define ADS_QUE_TWO        0x01
#define ADS_QUE_FOUR       0x02
#define ADS_QUE_DISABLE    0x03 // defaut

// Function
extern ad111x_device_t ads111x_init(const char *i2c_bus_name, uint8_t AddrInput);
extern void ads111x_deinit(ad111x_device_t dev);
extern rt_err_t ads111x_config(ad111x_device_t dev, rt_uint8_t LCMD, rt_uint8_t HCMD);
extern void I2CWriteByte_A(uint8_t DATA);
extern rt_err_t ads111x_setThresHold(ad111x_device_t dev, rt_uint16_t L_TH, rt_uint16_t H_TH);
extern rt_uint16_t ads111x_readAD(ad111x_device_t dev);

#endif /* BSP_DRIVERS_ADS111X_H_ */
