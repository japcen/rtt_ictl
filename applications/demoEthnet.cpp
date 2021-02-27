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
#define DBG_TAG "demoEth"
#include <usrPublic.h>
#include <stdlib.h>
#include <bsp/includes/taskNet.h>

// 以太网状态监控
void taskEth_entry(void *parameter)
{
    rt_thread_mdelay(10000);

    rt_tick_t tick = rt_tick_get();
    rt_tick_t chktick = rt_tick_from_millisecond(600000); // 10mins

    if (RT_FALSE == g_ctrNet.isConnected()) {
        g_ctrNet.connect();
    }

    for (;;) {
        // 通讯参数改变
        if (g_paramNet.state) {
            g_paramNet.state = 0;
            tick = rt_tick_get();
            if (RT_FALSE == g_ctrNet.isConnected()) {
                rt_kprintf("eth connection param changed!!! reconnect...\n");
                g_ctrNet.connect();
            }
        }

        // keepalive:周期检测连接状态 未连接则重新连接 周期10分钟
        if (GetDeltaTime(rt_tick_get(), tick) >= chktick) {
            tick = rt_tick_get();
            if (RT_FALSE == g_ctrNet.isConnected()) {
                rt_kprintf("eth disconnect too long!!! retry...\n");
                g_ctrNet.connect();
            }
        }

        rt_thread_mdelay(10);
    }
}

#define THREAD_NAME_ETH            "tasketh"
#define THREAD_PRIO_TASKETH         20
#define THREAD_SIZE_TASKETH         2048
#define THREAD_TIMESLICE_TASKETH    5
static rt_uint8_t thread_stack_tasketh[THREAD_SIZE_TASKETH];
static struct rt_thread tid_Tasketh;
static int threadbegin_eth(void)
{
    rt_thread_init(&tid_Tasketh,
                   THREAD_NAME_ETH,
                   taskEth_entry,
                   RT_NULL,
                   thread_stack_tasketh,
                   THREAD_SIZE_TASKETH,
                   THREAD_PRIO_TASKETH,
                   THREAD_TIMESLICE_TASKETH);
    rt_thread_startup(&tid_Tasketh);

    return 0;
}
INIT_APP_EXPORT(threadbegin_eth);

#ifdef DBG_MOD
#ifdef RT_USING_FINSH

static int eth_connect(void)
{
    g_ctrNet.connect();
    return 0;
}
MSH_CMD_EXPORT(eth_connect, Test Fun: connect the eth with current param);

static int eth_disconnect(void)
{
    g_ctrNet.disconnect();
    return 0;
}
MSH_CMD_EXPORT(eth_disconnect, Test Fun: disconnect the ethnet);

static int eth_sendmsg(void)
{
    char msg[] = "this is ictl calling\n";
    g_ctrNet.send(msg, sizeof(msg));
    return 0;
}
MSH_CMD_EXPORT(eth_sendmsg, Test Fun: send msg on ethnet);

/**
 * Set ethnet connect param
 */
void eth_set(int argc, char *argv[])
{
    rt_netparam cfg = {0};
    char ip[IP4ADDR_STRLEN_MAX] = {0};
    memcpy(&cfg, &g_paramNet, sizeof(rt_netparam));
    rt_uint8_t rtn = 0; // 1 success 0 error
    rt_uint16_t port = 0;

    if (argc == 3) {
        if (!strcmp(argv[1], "ip")) {
            strcpy(ip, argv[2]);
            rtn = netdev_ip4addr_aton(ip, &cfg.ip_addr);
        }
        else if (!strcmp(argv[1], "gw")) {
            strcpy(ip, argv[2]);
            rtn = netdev_ip4addr_aton(ip, &cfg.gw_addr);
        }
        else if (!strcmp(argv[1], "nm")) {
            strcpy(ip, argv[2]);
            rtn = netdev_ip4addr_aton(ip, &cfg.nm_addr);
        }
        else if (!strcmp(argv[1], "port")) {
            port = atoi(argv[2]);
            if ((port) && (UDP_ENSURE_LOCAL_PORT_RANGE(port))) {
                cfg.localPort = port;
                rtn = 1;
            }
            else {
                rt_kprintf("udp local port must between [%d %d]\n", UDP_LOCAL_PORT_RANGE_START, UDP_LOCAL_PORT_RANGE_END);
            }
        }
        else if (!strcmp(argv[1], "rip")) {
            strcpy(ip, argv[2]);
            rtn = netdev_ip4addr_aton(ip, &cfg.serviceIp);
        }
        else if (!strcmp(argv[1], "rport")) {
            cfg.servicePort = atoi(argv[2]);
            rtn = 1;
        }
        else if (!strcmp(argv[1], "type")) {
            if (!strcmp(argv[2], "tcp")) {
                cfg.TcpOrUdp = 1;
                rtn = 1;
            }
            else if (!strcmp(argv[2], "udp")) {
                cfg.TcpOrUdp = 0;
                rtn = 1;
            }
        }
    }
    else if ((argc == 2) && (!strcmp(argv[1], "show"))) {
        rt_kprintf("ip address  : %s\n", inet_ntoa(cfg.ip_addr));
        rt_kprintf("gw address  : %s\n", inet_ntoa(cfg.gw_addr));
        rt_kprintf("net mask    : %s\n", inet_ntoa(cfg.nm_addr));
        rt_kprintf("local port  : %d\n", cfg.localPort);
        if (1 == cfg.TcpOrUdp) {
            rt_kprintf("connect type: %s\n", "tcp");
        }
        else {
            rt_kprintf("connect type: %s\n", "udp");
        }
        rt_kprintf("remote address  : %s\n", inet_ntoa(cfg.serviceIp));
        rt_kprintf("remote port     : %d\n", cfg.servicePort);
        rtn = 2;
    }
    else {
        rt_kprintf("Usage:\n");
        rt_kprintf("eth_set ip <ipv4>          - set local ip address\n");
        rt_kprintf("eth_set gw <ipv4>          - set local gateway address\n");
        rt_kprintf("eth_set nm <ipv4>          - set local netmask address\n");
        rt_kprintf("eth_set port <%5d-%5d> - set local port for udp connection. 0:system allocates\n", UDP_LOCAL_PORT_RANGE_START, UDP_LOCAL_PORT_RANGE_END);
        rt_kprintf("eth_set rip <ipv4>         - set remote ip address\n");
        rt_kprintf("eth_set rport <ipv4>       - set remote port\n");
        rt_kprintf("eth_set type <tcp/udp>     - set connection type e.g \"tcp\" or \"udp\"\n");
        rt_kprintf("eth_set show               - show current ethernet connection param\n");
        return;
    }

    if (1 == rtn) {
        setNetCfg(&cfg);
    }
    else if (2 == rtn) {
    }
    else {
        rt_kprintf("param error!!!!\n");
    }
}
MSH_CMD_EXPORT(eth_set, Test Fun: set ethnet param);

#endif
#endif
