/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-12-30     BEN       the first version
 */
#ifndef APPLICATIONS_USRPUBLIC_H_
#define APPLICATIONS_USRPUBLIC_H_

#include <stdint.h>
#include <string.h>
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

#define DBG_MOD            // debug flag

#define GET_MAX(A, B)      ((A) > (B) ? (A) : (B))
#define GET_MIN(A, B)      ((A) > (B) ? (B) : (A))
#define STRCMP(a, R, b)    (strcmp((a), (b)) R 0)

#define ABS(a)             ((a)>0?(a):-(a))

#define OFFSET_OF(obj_type,mb)  ((int)&(((obj_type*)0)->mb))

/* 判断数据是否为空
 * flag 指定的空字符  DATA_EMPTY
 */
#define DATA_EMPTY                 0xFF
inline int HexIsEmpty(const unsigned char *buf, int len, unsigned char flag)
{
    int i;

    for (i=0; i<len; i++) {
        if(buf[i] != flag) return 0;
    }

    return 1;
}

/* 打印字符串数组
 * *str 提示信息  *buf 打印字符串  len 字符串长度  row 每行字符串个数 默认为10
 */
inline void DbgPrintDataLst(char* str, char *buf, int len, int row)
{
    int i = 0;

    if (len < 0) return;
    if (row <= 0) row = 10;

    len = GET_MIN(len, 200);

    if (NULL != str) {
        rt_kprintf("%s:", str);
    }

    for (i=0; i<len; i++) {
        if (0 == i%row) rt_kprintf("\n");
        rt_kprintf("%02X ", buf[i]);
    }
    rt_kprintf("\n");
}

// 计算系统ticks差值
inline uint32_t GetDeltaTime(rt_uint32_t tnow, rt_uint32_t last_time)
{
    if (tnow >= last_time) {
       return (tnow - last_time);
    }
    else {
        return (UINT32_MAX - last_time + tnow + 1);
    }
}

#endif /* APPLICATIONS_USRPUBLIC_H_ */
