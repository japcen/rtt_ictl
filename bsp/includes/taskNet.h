/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-12-30     BEN       the first version
 */
#ifndef BSP_INCLUDES_TASKNET_H_
#define BSP_INCLUDES_TASKNET_H_

#include <rtthread.h>
//extern "C"
//{
//#include <arpa/inet.h>
//#include <netdev.h>
#include <bsp/includes/bspNetComParam.h>
//}

#ifndef UDP_LOCAL_PORT_RANGE_START
/* From http://www.iana.org/assignments/port-numbers:
   "The Dynamic and/or Private Ports are those from 49152 through 65535" */
#define UDP_LOCAL_PORT_RANGE_START  0xc000
#define UDP_LOCAL_PORT_RANGE_END    0xffff
#define UDP_ENSURE_LOCAL_PORT_RANGE(port) ((rt_uint16_t)(((port) & ~UDP_LOCAL_PORT_RANGE_START) + UDP_LOCAL_PORT_RANGE_START))
#endif

typedef struct rt_tcpclient rt_tcpclient_t;
typedef void (*rx_cb_t)(void *buff, rt_size_t len);

struct rt_tcpclient
{
    int sock_fd;
    int pipe_read_fd;
    int pipe_write_fd;
    char pipe_name[8];
    rx_cb_t rx;
};

// 通讯控制对象
class CCtrlNet
{
private:
    rt_tcpclient_t *socketHdl;
    //rt_event_t      tc_event;
    rt_uint8_t      lock;
    void lockit(void);
    void unlock(void);
public:
    rt_uint8_t      end;            // 主动结束标识
    rt_err_t connect(void);         // 开启连接
    rt_err_t disconnect(void);      // 关闭连接
    rt_bool_t isConnected(void);    // 是否处于连接状态
    rt_int32_t send(const void *buff, rt_size_t len);    // 发送报文
public:
    CCtrlNet() {
        socketHdl = RT_NULL;
        end = 0;
        lock = 0;
    }
    ~CCtrlNet() {
    }
};
extern CCtrlNet g_ctrNet;

#define ETHNET_IF_NAME           "e0"
#define ETHNET_BUS_NAME          (char*)"tcpc"
//#define ETHNET_HDL               g_dev_ethnet;

typedef void (*rx_cb_t)(void *buff, rt_size_t len);

rt_tcpclient_t *rt_tcpclient_start(const char *hostname, rt_uint32_t port);
void rt_tcpclient_close(rt_tcpclient_t *thiz);
rt_int32_t rt_tcpclient_attach_rx_cb(rt_tcpclient_t *thiz, rx_cb_t cb);
rt_int32_t rt_tcpclient_send(rt_tcpclient_t *thiz, const void *buff, rt_size_t len);

//rt_uint16_t getLocalPort(void);
rt_err_t setCurNetCfg(struct netdev *pDev);
rt_err_t setNetCfg(rt_netparam *netCfg);
rt_err_t setDefNetCfg(void);

#endif /* BSP_INCLUDES_TASKNET_H_ */
