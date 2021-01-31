/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-29     BEN       the first version
 */
#ifndef BSP_INCLUDES_BSPNETCOMPARAM_H_
#define BSP_INCLUDES_BSPNETCOMPARAM_H_

#include <arpa/inet.h>
#include <netdev.h>

// ͨѶ����Ĭ��ֵ
#define DEF_IP_ADDR        RT_LWIP_IPADDR
#define DEF_GW_ADDR        RT_LWIP_GWADDR
#define DEF_SER_IP_ADDR    "192.168.8.121"
#define DEF_LOCAL_PORT     0
#define DEF_SER_PORT       9008
#define DEF_NM_ADDR        RT_LWIP_MSKADDR
#define DEF_TCPORUDP       0 // UDP

// ͨѶ���Ӳ���
// FIXME same as udp.c
#if 0
struct rt_netparam
{
    ip_addr_t ip_addr;         // �ն�ip��ַ
    ip_addr_t gw_addr;         // ���ص�ַ
    ip_addr_t nm_addr;         // ����
    rt_uint8_t TcpOrUdp;       // �������� 1 tcp, 0 udp
    rt_uint16_t localPort;     // ���ض˿� 0-���ն����з���

    ip_addr_t serviceIp;       // �����ip��ַ
    rt_uint16_t servicePort;   // ����˶˿�
};
extern struct rt_netparam g_paramNet;
#endif

#endif /* BSP_INCLUDES_BSPNETCOMPARAM_H_ */
