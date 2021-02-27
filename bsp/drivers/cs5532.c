/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-10     BEN       the first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"

#include <string.h>
#include <stdlib.h>

#include <rtdevice.h> // CMEDIT

#include "cs5532.h"

#define CS5532_SYNC1        0xFF
#define CS5532_SYNC0        0xFE

void cs5532_rst(struct rt_spi_device *pDev)    //串口复位
{
    uint8_t txbuf[16];

    memset(txbuf, CS5532_SYNC1, 15);
    txbuf[15] = CS5532_SYNC0;
    if (16 != rt_spi_transfer(pDev, txbuf, RT_NULL, 16)) {
        rt_kprintf("ERR 00 spi rst\n");
    }
}

int16_t cs5532_wrReg(struct rt_spi_device *pDev, uint8_t reg, uint32_t data)
{
    uint8_t txBuf[5];
    txBuf[0] = reg;
    txBuf[1] = (data >> 24) & 0xFF;
    txBuf[2] = (data >> 16) & 0xFF;
    txBuf[3] = (data >> 8) & 0xFF;
    txBuf[4] = data & 0xFF;
    rt_spi_transfer(pDev, txBuf, RT_NULL, 5);
    return 0;
}

uint32_t cs5532_rdReg(struct rt_spi_device *pDev, uint8_t reg)
{
    uint8_t rxBuf[4] = {0};

    reg |= CMD_RD;
    rt_spi_send_then_recv(pDev, &reg, 1, rxBuf, 4);
    return (rxBuf[0] << 24 | rxBuf[1] << 16 | rxBuf[2] << 8 | rxBuf[3]);
}

uint32_t cs5532_offadjust(struct rt_spi_device *pDev)
{
    uint8_t txbuf;
    uint32_t offval = 0;

    txbuf = CMD_COMD1|CMD_TOFFAD;
    rt_spi_transfer(pDev, &txbuf, RT_NULL, 1);
    rt_thread_mdelay(20); // CMEDIT
    offval = cs5532_rdReg(pDev, CMD_OFFSET_REG);

    return offval;
}

uint32_t cs5532_gainadjust(struct rt_spi_device *pDev)
{
    uint8_t txbuf;
    uint32_t gainval = 0;

    cs5532_rst(pDev);
    txbuf = CMD_COMD1|CMD_TGAINAD;
    rt_spi_transfer(pDev, &txbuf, RT_NULL, 1);
    //rt_thread_mdelay(2); // CMEDIT
    gainval = cs5532_rdReg(pDev, CMD_GAIN_REG);

    return gainval;
}

void cs5532_init(struct rt_spi_device *pDev)
{
    uint32_t date;
    cs5532_wrReg(pDev, CMD_CONFIG_REG, CFG_RS);
    do {
        date = cs5532_rdReg(pDev, CMD_CONFIG_REG);
    }
    while (date & CFG_RV);
    cs5532_wrReg(pDev, CMD_CONFIG_REG, CFG_VRS);

    cs5532_offadjust(pDev);
    cs5532_gainadjust(pDev);
}

void cs5532_csrSet(struct rt_spi_device *pDev, uint8_t setIdx, uint16_t data1, uint16_t data2)
{
    uint8_t cs = 0;
    uint32_t data = 0;
    if (UCMD_CSRSET_1 == setIdx) {
        cs = CMD_CS1;
    }
    else if (UCMD_CSRSET_2 == setIdx) {
        cs = CMD_CS2;
    }
    else if (UCMD_CSRSET_3 == setIdx) {
        cs = CMD_CS3;
    }
    else if (UCMD_CSRSET_4 == setIdx) {
        cs = CMD_CS4;
    }
    else {
        return;
    }

    data = CSR_SET1(data1) | CSR_SET2(data2);
    cs5532_wrReg(pDev, CMD_CSR_REG|cs, data);
}

void cs5532_convert(struct rt_spi_device *pDev, rt_uint8_t conver)
{
    uint8_t txBuf = 0;
    rt_thread_mdelay(1);
    if (UCMD_CONVER_SINGLE == conver) {
        txBuf = CMD_COMD1|CMD_CSPR1;
        rt_spi_transfer(pDev, &txBuf, RT_NULL, 1);
    }
    else if (UCMD_CONVER_CONTINUE == conver) {
        txBuf = CMD_COMD1|CMD_MC|CMD_CSPR1;
        rt_spi_transfer(pDev, &txBuf, RT_NULL, 1);
    }
}

static int two_complement(int n)
{
    const int negative = (n & (1 << 23)) != 0;
    int native_int;

    if (negative)
      native_int = n | ~((1 << 24) - 1);
    else
      native_int = n;
    return native_int;
}

int32_t cs5532_readAdc(struct rt_spi_device *pDev)
{
    uint8_t txBuf[5] = {0x00};//{0xFF};
    uint8_t rxBuf[5] = {0};
    int32_t data = 0;
    rt_spi_transfer(pDev, txBuf, rxBuf, 5);
    rt_kprintf("rcv: %02X %02X %02X %02X %02X\n",
            rxBuf[0], rxBuf[1], rxBuf[2], rxBuf[3], rxBuf[4]);
    data = ((int32_t)rxBuf[1]<<16) + ((int32_t)rxBuf[2]<<8) + rxBuf[3];
    data = two_complement(data);
    return data;
}

int32_t cs5532_readAdc2(struct rt_spi_device *pDev)
{
    uint8_t txBuf[5] = {0};
    uint8_t rxBuf[5] = {0};
    int32_t data = 0;

    rt_spi_transfer(pDev, txBuf, rxBuf, 5);
    rt_kprintf("--%02X %02X %02X %02X %02X\n",
            rxBuf[0], rxBuf[1], rxBuf[2], rxBuf[3], rxBuf[4]);
    data = ((int32_t)rxBuf[1]<<16) + ((int32_t)rxBuf[2]<<8) + rxBuf[3];
    data = two_complement(data);
    return data;
}

void cs5532_test_regrd(const char *devName)
{
    uint32_t date, cnt = 0;
    struct rt_spi_device *pDev = RT_NULL;
    pDev = (struct rt_spi_device *)rt_device_find(devName);
    if (RT_NULL == pDev) {
        return;
    }

    rt_kprintf("++++test cs5532 %s reg ...\n", devName);
    cs5532_rst(pDev);

    cs5532_wrReg(pDev, CMD_CONFIG_REG, CFG_RS);
    do {
        date = cs5532_rdReg(pDev, CMD_CONFIG_REG);
    }
    while (date & CFG_RV);

    rt_kprintf("--read default reg date...\n", cnt);
    date = cs5532_rdReg(pDev, CMD_CONFIG_REG);
    rt_kprintf("  config reg is 0x%08X\n", date);
    date = cs5532_rdReg(pDev, CMD_OFFSET_REG);
    rt_kprintf("  offset reg is 0x%08X\n", date);
    date = cs5532_rdReg(pDev, CMD_GAIN_REG);
    rt_kprintf("  gain reg is   0x%08X\n", date);
    date = cs5532_rdReg(pDev, CMD_CSR_REG);
    rt_kprintf("  channel reg is 0x%08X\n", date);

    rt_kprintf("--set config reg 0x02000000...\n");
    cs5532_wrReg(pDev, CMD_CONFIG_REG, CFG_VRS);
    date = cs5532_rdReg(pDev, CMD_CONFIG_REG);
    rt_kprintf("  read data is 0x%08X\n", date);

    rt_kprintf("--set csr reg 0x094000C0...\n");
    cs5532_csrSet(pDev, UCMD_CSRSET_1,
                  CSR_CS1|CSR_GAIN2A|CSR_WR25|CSR_UNIPOLAR, CSR_CS1|CSR_GAIN1A|CSR_WR50|CSR_UNIPOLAR);
    date = cs5532_rdReg(pDev, CMD_CSR_REG|CMD_CSPR1);
    rt_kprintf("  read data is 0x%08X\n", date);

    rt_kprintf("++++test cs5532 %s reg end\n\n", devName);
}

void cs5532_test_singlerd(const char *devName)
{
    int32_t   date  = 0;
    rt_int8_t cnt   = 0;
    float     fval  = 0.0f;
    struct rt_spi_device *pDev = RT_NULL;
    pDev = (struct rt_spi_device *)rt_device_find(devName);
    if (RT_NULL == pDev) {
        return;
    }

    rt_kprintf("++++test %s single convert...\n", devName);

    cs5532_rst(pDev);
    cs5532_init(pDev);
    cs5532_csrSet(pDev, UCMD_CSRSET_1, CSR_BIPOLAR, CSR_BIPOLAR);

    while (cnt < 20) {
        cs5532_convert(pDev, UCMD_CONVER_SINGLE);
        rt_thread_mdelay(1000);
        date = cs5532_readAdc(pDev);
        rt_kprintf("  %d adc is %d\n", cnt, date);
        fval = (float)date*5/3/0x7FFFFF;
        rt_kprintf("  vol is %d uv\n", (int32_t)(fval*1000000 + 0.5f));
        cnt++;
    }
    rt_kprintf("++++test %s single convert end\n\n", devName);
}

void cs5532_test_conrd(void)
{
    int32_t   date = 0;
    float     fval = 0.0f;
    struct rt_spi_device *pDev0, *pDev1, *pDev2 = RT_NULL;
    pDev0 = (struct rt_spi_device *)rt_device_find("spi_cs0");
    pDev1 = (struct rt_spi_device *)rt_device_find("spi_cs1");
    pDev2 = (struct rt_spi_device *)rt_device_find("spi_cs2");
    if ((RT_NULL == pDev0) || (RT_NULL == pDev1) || (RT_NULL == pDev2)) {
        return;
    }

    rt_kprintf("++++test cs5532 continue convert...\n");
    cs5532_rst(pDev0);
    cs5532_init(pDev0);
    cs5532_csrSet(pDev0, UCMD_CSRSET_1, CSR_WR1600|CSR_BIPOLAR, CSR_WR1600|CSR_BIPOLAR);
    cs5532_convert(pDev0, UCMD_CONVER_CONTINUE);

    cs5532_rst(pDev1);
    cs5532_init(pDev1);
    cs5532_csrSet(pDev1, UCMD_CSRSET_1, CSR_WR1600|CSR_BIPOLAR, CSR_WR1600|CSR_BIPOLAR);
    cs5532_convert(pDev1, UCMD_CONVER_CONTINUE);

    cs5532_rst(pDev2);
    cs5532_init(pDev2);
    cs5532_csrSet(pDev2, UCMD_CSRSET_1, CSR_WR1600|CSR_BIPOLAR, CSR_WR1600|CSR_BIPOLAR);
    cs5532_convert(pDev2, UCMD_CONVER_CONTINUE);

    while (1)
    {
        rt_thread_mdelay(1000);

        date = cs5532_readAdc(pDev0);
        fval = (float)date*5/3/0x7FFFFF;
        rt_kprintf("chl(0) is %d uv\n", (int32_t)(fval*1000000 + 0.5f));

        date = cs5532_readAdc(pDev1);
        fval = (float)date*5/3/0x7FFFFF;
        rt_kprintf("chl(1) is %d uv\n", (int32_t)(fval*1000000 + 0.5f));

        date = cs5532_readAdc(pDev2);
        fval = (float)date*5/3/0x7FFFFF;
        rt_kprintf("chl(2) is %d uv\n", (int32_t)(fval*1000000 + 0.5f));
        rt_kprintf("\n");
    }
}
