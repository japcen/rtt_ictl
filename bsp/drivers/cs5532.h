/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-10     BEN       the first version
 */
#ifndef BSP_DRIVERS_CS5532_H_
#define BSP_DRIVERS_CS5532_H_

#define UCMD_CSRSET_1            0x00
#define UCMD_CSRSET_2            0x01
#define UCMD_CSRSET_3            0x02
#define UCMD_CSRSET_4            0x03

#define UCMD_CONVER_SINGLE              0x00
#define UCMD_CONVER_CONTINUE            0x01

/* Command Register */
//#define CMD_COMD                 0x00         // others
#define CMD_CS1                  0x00
#define CMD_CS2                  0x01
#define CMD_CS3                  0x20
#define CMD_CS4                  0x30
#define CMD_RD                   0x08          // read date  .else write
#define CMD_OFFSET_REG           0x01
#define CMD_GAIN_REG             0x02
#define CMD_CONFIG_REG           0x03
#define CMD_CSR_REG              0x05

#define CMD_COMD1                0x80          // conversion calibration command
                                               // .else others
#define CMD_MC                   0x40
#define CMD_CSPR1                0x00          // CSPR 1~8
#define CMD_CSPR2                0x08
#define CMD_CSPR3                0x10
#define CMD_CSPR4                0x18
#define CMD_CSPR5                0x20
#define CMD_CSPR6                0x28
#define CMD_CSPR7                0x30
#define CMD_CSPR8                0x38
#define CMD_NORSW                0x00
#define CMD_SOFFAD               0x01
#define CMD_SGAINAD              0x02
#define CMD_TOFFAD               0x05
#define CMD_TGAINAD              0x06

/* Configuration Register */
#define CFG_PSS                0x80000000    // sleep mode  .else standby mode
#define CFG_PDW                0x40000000    // power save mode  .else normal mode
#define CFG_RS                 0x20000000    // reset cycle
#define CFG_RV                 0x10000000    // system was reset
#define CFG_VRS                0x02000000    // 1~2.5V  .else 2.5~Va

// CMD_CSR_REG
#define CSR_SET1(set)          ((uint32_t)(set)<<16)
#define CSR_SET2(set)          ((uint32_t)(set))

#define CSR_CS1                0x0000
#define CSR_CS2                0x4000
#define CSR_CS3                0x8000    // unvalid
#define CSR_CS4                0xC000    // unvalid

#define CSR_GAIN1A             0x0000
#define CSR_GAIN2A             0x0800
#define CSR_GAIN4A             0x1000
#define CSR_GAIN8A             0x1800
#define CSR_GAIN16A            0x2000
#define CSR_GAIN32A            0x2800
#define CSR_GAIN64A            0x3000

#define CSR_WR100              0x0000    // FRS = 1
#define CSR_WR50               0x0080
#define CSR_WR25               0x0100
#define CSR_WR12p5             0x0180
#define CSR_WR6p25             0x0200
#define CSR_WR3200             0x0400
#define CSR_WR1600             0x0480
#define CSR_WR800              0x0500
#define CSR_WR400              0x0580
#define CSR_WR200              0x0600

#define CSR_BIPOLAR            0x0000    // 双极性
#define CSR_UNIPOLAR           0x0040    // 单极性

void cs5532_test_regrd(const char *devName);
void cs5532_test_singlerd(const char *devName);
void cs5532_test_conrd(void/*const char *devName*/);

#endif /* BSP_DRIVERS_CS5532_H_ */
