/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-12     BEN       the first version
 */

#include <rtthread.h>
#define DBG_TAG "param"
#include <usrPublic.h>
#include <version.h>

//extern "C"
//{
#include <rtdef.h>
//}
#include <bsp/includes/taskNet.h>
#include <bsp/includes/bspI2C.h>

// software version
#define PARAM_SOFTVERSION_BASE      0       // 软件版本信息保存基地址
#define CLEAN_DB_FLAG               0x01    // 内存结构版本标识
// 读取软件版本等信息
static int loadSoftVersion(void)
{
    rt_err_t rtn = RT_EOK;
    uint8_t buf[8] = {0};
    uint8_t version[8] = {CLEAN_DB_FLAG, VERSION_MAJOR, VERSION_MINOR, VERSION_PROJECT, VERSION_STAGE,
                          VERSION_YEAR, VERSION_MONTH, VERSION_DAY};

    rtn = at24cxx_read(AT24cxx_HDL, PARAM_SOFTVERSION_BASE, buf, 8);
    if (RT_EOK == rtn) {
        if (memcmp(buf, version, 8)) {
            at24cxx_page_write(AT24cxx_HDL, PARAM_SOFTVERSION_BASE, version, 8);
        }

        if (buf[0] != CLEAN_DB_FLAG) {
            // USER CODE BEGIN
            // 标识位不同时,清除FLASH数据.防止参数结构变更导致程序错误.
            // USER CODE END
        }
    }
    else {
        at24cxx_page_write(AT24cxx_HDL, PARAM_SOFTVERSION_BASE, version, 8);
    }

    rt_kprintf("\n******\n");
    rt_kprintf(" ICTL      soft version:v%d.%d.%d.%d date:%d-%d-%d\n",
            version[1], version[2], version[3], version[4], version[5], version[6], version[7]);
    rt_kprintf("******\n");
    return RT_EOK;
}
INIT_ENV_EXPORT(loadSoftVersion);

// at24 64/page
#define PARAM_NET_CFG_BASE         64        // 通讯参数保存基地址
// 读取默认通讯参数
static void loadDefNetParam(void)
{
    netdev_ip4addr_aton(DEF_IP_ADDR, &g_paramNet.ip_addr);
    netdev_ip4addr_aton(DEF_GW_ADDR, &g_paramNet.gw_addr);
    netdev_ip4addr_aton(DEF_NM_ADDR, &g_paramNet.nm_addr);
    netdev_ip4addr_aton(DEF_SER_IP_ADDR, &g_paramNet.serviceIp);
    g_paramNet.localPort = DEF_LOCAL_PORT;
    g_paramNet.servicePort = DEF_SER_PORT;
    g_paramNet.TcpOrUdp = DEF_TCPORUDP;
}
// 保存通讯参数至flash
rt_err_t writeNetParam(void)
{
    rt_err_t rtn = RT_EOK;
    if (RT_NULL != AT24cxx_HDL) {
        rtn = at24cxx_page_write(AT24cxx_HDL, PARAM_NET_CFG_BASE, (uint8_t*)&g_paramNet, sizeof(rt_netparam));
    }
    else {
        rt_kprintf("no at24c bus , err write net config!!!!!\r\n");
        rtn = RT_EEMPTY;
    }

    return rtn;
}
// 由flash读取通讯参数
rt_err_t readNetParam(void)
{
    rt_err_t rtn = RT_EOK;
    if (RT_NULL != AT24cxx_HDL) {
        rtn = at24cxx_read(AT24cxx_HDL, PARAM_NET_CFG_BASE, (uint8_t*)&g_paramNet, sizeof(rt_netparam));
        if (
            (RT_EOK != rtn) ||
            (HexIsEmpty((unsigned char*)&g_paramNet, sizeof(rt_netparam), DATA_EMPTY)) ||
            (HexIsEmpty((unsigned char*)&g_paramNet, sizeof(rt_netparam), 0x00))
           ) {
            loadDefNetParam();
            writeNetParam();
        }
    }
    else {
        rt_kprintf("no at24c bus , err ini net config!!!!!\r\n");
        rtn = RT_EEMPTY;
    }

    return rtn;
}
// 上电通讯参数初始化
static int initNetParam(void)
{
    loadDefNetParam();        // 加载默认参数
    readNetParam();           // 读取flash中的参数 成功则替换默认参数
    setCurNetCfg(RT_NULL);    // 参数立即生效
    return 0;
}
INIT_ENV_EXPORT(initNetParam);
