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

#define DBG_TAG "taskNet"
#include <usrPublic.h>
extern "C"
{
#include <rtdef.h>
#include <rtdevice.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
//#include <stdlib.h>
}
#include <bsp/includes/taskNet.h>
#include <usrParam.h>

static rt_event_t tc_event = RT_NULL;

#define TC_TCPCLIENT_CLOSE (1 << 0)
#define TC_EXIT_THREAD (1 << 1)
#define TC_SWITCH_TX (1 << 2)
#define TC_SWITCH_RX (1 << 3)

// 以太网接收函数
static void rt_tc_rx_cb(void *buff, rt_size_t len)
{
    char *recv = RT_NULL;

    recv = (char*)malloc(len + 1);
    if (recv == RT_NULL)
        return;
    //char recv[256] = {0};

    memcpy(recv, buff, len);
    *(recv + len) = '\0';

    // 打印接收信息
    rt_kprintf("tc_rx_cb, recv data: %s\n", recv);
    rt_kprintf("tc_rx_cb, recv len: %d\n", strlen(recv));

    // 退出及结束连接处理
    if (STRCMP(recv, ==, "exit"))
    {
        rt_kprintf("receive [exit]\n");
        rt_event_send(tc_event, TC_TCPCLIENT_CLOSE);
    }

    if (STRCMP(recv, ==, "TCP disconnect"))
    {
        rt_kprintf("[TCP disconnect]\n");
        rt_event_send(tc_event, TC_TCPCLIENT_CLOSE);
    }

    // USR CODE BEGIN
    /* 是主机发送的通讯参数变更指令
     * 1. 参数变更
     * 2. 置参数变更标识    g_paramNet.state = 1;
     * 3. 结束 在通讯状态监控线程中处理
     */

    /* 正常报文 解析及处理
     * receive packet
     */
    // USR CODE END

    free(recv);
}

#define BUFF_SIZE (1024)
#define RX_CB_HANDLE(_buff, _len)  \
    do                             \
    {                              \
        if (thiz->rx)              \
            thiz->rx((void*)_buff, _len); \
    } while (0)

#define PRINTF_TEST(_buff, _len, _tag) \
    do                                 \
    {                                  \
        _buff[_len] = '\0';            \
        rt_kprintf(":%s\n", _buff);  \
    } while (0)

#define EXCEPTION_HANDLE(_bytes, _tag, _info_a, _info_b)                  \
    do                                                                    \
    {                                                                     \
        if (_bytes < 0)                                                   \
        {                                                                 \
            rt_kprintf("return: %d\n", _bytes);                \
            goto exit;                                                    \
        }                                                                 \
        if (_bytes == 0)                                                  \
        {                                                                 \
            if (STRCMP(_info_b, ==, "warning"))                           \
                rt_kprintf("return: %d\n", _bytes);            \
            else                                                          \
            {                                                             \
                rt_kprintf("return: %d\n", _bytes);            \
                RX_CB_HANDLE("TCP disconnect", strlen("TCP disconnect")); \
                goto exit;                                                \
            }                                                             \
        }                                                                 \
    } while (0)

#define EXIT_HANDLE(_buff)                                            \
    do                                                                \
    {                                                                 \
        if (STRCMP(_buff, ==, "exit"))                                \
        {                                                             \
            rt_kprintf("exit handle, receive [exit], exit thread\n");   \
            rt_kprintf("exit handle, user clean up resources pleas\n"); \
            goto exit;                                                \
        }                                                             \
    } while (0)

static rt_tcpclient_t *tcpclient_create(void);
static rt_int32_t tcpclient_destory(rt_tcpclient_t **pthiz);
static rt_int32_t socket_init(rt_tcpclient_t *thiz, const char *hostname, rt_uint32_t port, rt_uint8_t tcporudp);
static rt_int32_t socket_deinit(rt_tcpclient_t *thiz);
static rt_int32_t pipe_init(rt_tcpclient_t *thiz);
static rt_int32_t pipe_deinit(rt_tcpclient_t *thiz);
static void select_handle(rt_tcpclient_t *thiz, char *pipe_buff, char *sock_buff);
static rt_int32_t tcpclient_thread_init(rt_tcpclient_t *thiz);
static void tcpclient_thread_entry(void *param);

static rt_tcpclient_t *tcpclient_create(void)
{
    rt_tcpclient_t *thiz = RT_NULL;

    thiz = (rt_tcpclient_t*)rt_malloc(sizeof(rt_tcpclient_t));
    if (thiz == RT_NULL)
    {
        rt_kprintf("tcpclient alloc, malloc error\n");
        return RT_NULL;
    }

    thiz->sock_fd = -1;
    thiz->pipe_read_fd = -1;
    thiz->pipe_write_fd = -1;
    memset(thiz->pipe_name, 0, sizeof(thiz->pipe_name));
    thiz->rx = RT_NULL;

    return thiz;
}

static rt_int32_t tcpclient_destory(rt_tcpclient_t **pthiz)
{
    int res = 0;

    if ((pthiz == RT_NULL) || ((*pthiz) == RT_NULL))
    {
        rt_kprintf("tcpclient del, param is NULL, delete failed\n");
        return -1;
    }

    if ((*pthiz)->sock_fd != -1)
        socket_deinit(*pthiz);

    if ((*pthiz)->pipe_read_fd != -1)
    {
        res = close((*pthiz)->pipe_read_fd);
        RT_ASSERT(res == 0);
        (*pthiz)->pipe_read_fd = -1;
    }

    if ((*pthiz)->pipe_write_fd != -1)
    {
        res = close((*pthiz)->pipe_write_fd);
        RT_ASSERT(res == 0);
        (*pthiz)->pipe_write_fd = -1;
    }

    free(*pthiz);
    *pthiz = RT_NULL; // FIXME err?

    rt_kprintf("tcpclient del, delete succeed\n");

    return 0;
}

static rt_int32_t socket_init(rt_tcpclient_t *thiz, const char *url, rt_uint32_t port, rt_uint8_t tcporudp)
{
    struct sockaddr_in dst_addr;
    struct hostent *hostname;
    rt_int32_t res = 0;
    const char *str = "socket create succeed";

    if (thiz == RT_NULL)
        return -1;

    if (tcporudp) {
        thiz->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    }
    else {
        thiz->sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    }
    if (thiz->sock_fd == -1)
    {
        rt_kprintf("socket init, socket create failed\n");
        return -1;
    }

    hostname = gethostbyname(url);

    dst_addr.sin_family = AF_INET;
    dst_addr.sin_port = htons(port);
    dst_addr.sin_addr = *((struct in_addr *)hostname->h_addr);
    memset(&(dst_addr.sin_zero), 0, sizeof(dst_addr.sin_zero));

    res = connect(thiz->sock_fd, (struct sockaddr *)&dst_addr, sizeof(struct sockaddr));
    if (res == -1)
    {
        rt_kprintf("socket init, socket connect failed\n");
        return -1;
    }

    rt_kprintf("socket init, TCP connected succeed\n");

    send(thiz->sock_fd, str, strlen(str), 0);

    return 0;
}

static rt_int32_t socket_deinit(rt_tcpclient_t *thiz)
{
    int res = 0;

    if (thiz == RT_NULL)
    {
        rt_kprintf("socket deinit, param is NULL, socket deinit failed\n");
        return -1;
    }

    res = closesocket(thiz->sock_fd);
    RT_ASSERT(res == 0);

    thiz->sock_fd = -1;

    rt_kprintf("socket deinit, socket close succeed\n");

    return 0;
}

static rt_int32_t pipe_init(rt_tcpclient_t *thiz)
{
    char dev_name[32];
    static int pipeno = 0;
    rt_pipe_t *pipe = RT_NULL;

    if (thiz == RT_NULL)
    {
        rt_kprintf("pipe init, param is NULL\n");
        return -1;
    }

    snprintf(thiz->pipe_name, sizeof(thiz->pipe_name), "pipe%d", pipeno++);

    pipe = rt_pipe_create(thiz->pipe_name, PIPE_BUFSZ);
    if (pipe == RT_NULL)
    {
        rt_kprintf("pipe create, pipe create failed\n");
        return -1;
    }

    snprintf(dev_name, sizeof(dev_name), "/dev/%s", thiz->pipe_name);
    thiz->pipe_read_fd = open(dev_name, O_RDONLY, 0);
    if (thiz->pipe_read_fd < 0)
        goto fail_read;

    thiz->pipe_write_fd = open(dev_name, O_WRONLY, 0);
    if (thiz->pipe_write_fd < 0)
        goto fail_write;

    rt_kprintf("pipe init, pipe init succeed\n");
    return 0;

fail_write:
    close(thiz->pipe_read_fd);
fail_read:
    return -1;
}

static rt_int32_t pipe_deinit(rt_tcpclient_t *thiz)
{
    int res = 0;

    if (thiz == RT_NULL)
    {
        rt_kprintf("pipe deinit, param is NULL, pipe deinit failed\n");
        return -1;
    }

    res = close(thiz->pipe_read_fd);
    RT_ASSERT(res == 0);
    thiz->pipe_read_fd = -1;

    res = close(thiz->pipe_write_fd);
    RT_ASSERT(res == 0);
    thiz->pipe_write_fd = -1;

    rt_pipe_delete(thiz->pipe_name);

    rt_kprintf("pipe deinit, pipe close succeed\n");
    return 0;
}

static rt_int32_t tcpclient_thread_init(rt_tcpclient_t *thiz)
{
    rt_thread_t tcpclient_tid = RT_NULL;

    tcpclient_tid = rt_thread_create(ETHNET_BUS_NAME, tcpclient_thread_entry, thiz, 4096, 12, 10);
    if (tcpclient_tid == RT_NULL)
    {
        rt_kprintf("tcpclient thread, thread create failed\n");
        return -1;
    }

    rt_thread_startup(tcpclient_tid);

    rt_kprintf("tcpclient thread, thread init succeed\n");
    return 0;
}

static void select_handle(rt_tcpclient_t *thiz, char *pipe_buff, char *sock_buff)
{
    fd_set fds;
    rt_int32_t max_fd = 0, res = 0;
    timeval timeout;

    max_fd = GET_MAX(thiz->sock_fd, thiz->pipe_read_fd) + 1;
    FD_ZERO(&fds);
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    while (1)
    {
        FD_SET(thiz->sock_fd, &fds);
        FD_SET(thiz->pipe_read_fd, &fds);

        res = select(max_fd, &fds, RT_NULL, RT_NULL, &timeout);

        if (0 == res) {           // timeout
            if (g_ctrNet.end) {      // 主动断开
                EXCEPTION_HANDLE(0, "term end handle", "error", "TCP disconnected");
                //goto exit;
            }
            else {
                //rt_thread_mdelay(1);
                continue;
            }
        }
        else if (res < 0) {
            /* exception handling: exit */
            EXCEPTION_HANDLE(0, "select handle", "error", "timeout");
        }

        /* exception handling: exit */
        //EXCEPTION_HANDLE(res, "select handle", "error", "timeout");

        /* socket is ready */
        if (FD_ISSET(thiz->sock_fd, &fds))
        {
            res = recv(thiz->sock_fd, sock_buff, BUFF_SIZE, 0);

            /* exception handling: exit */
            EXCEPTION_HANDLE(res, "socket recv handle", "error", "TCP disconnected");

            /* have received data, clear the end */
            sock_buff[res] = '\0';

            RX_CB_HANDLE(sock_buff, res);

            EXIT_HANDLE(sock_buff);
        }

        /* pipe is read */
        if (FD_ISSET(thiz->pipe_read_fd, &fds))
        {
            /* read pipe */
            res = read(thiz->pipe_read_fd, pipe_buff, BUFF_SIZE);

            /* exception handling: exit */
            EXCEPTION_HANDLE(res, "pipe recv handle", "error", "");

            /* have received data, clear the end */
            pipe_buff[res] = '\0';

            /* write socket */
            send(thiz->sock_fd, pipe_buff, res, 0);

            /* exception handling: warning */
            EXCEPTION_HANDLE(res, "socket write handle", "error", "warning");

            EXIT_HANDLE(pipe_buff);
        }
    }
exit:
    rt_kprintf("socket exit!!!\r\n");
    free(pipe_buff);
    free(sock_buff);
}

static void tcpclient_thread_entry(void *param)
{
    rt_tcpclient_t *temp = (rt_tcpclient_t*)param;
    char *pipe_buff = RT_NULL, *sock_buff = RT_NULL;

    pipe_buff = (char *)rt_malloc(BUFF_SIZE);
    if (pipe_buff == RT_NULL)
    {
        LOG_I("thread entry, malloc error\n");
        LOG_I("thread entry, exit\n");
        return;
    }

    sock_buff = (char *)rt_malloc(BUFF_SIZE);
    if (sock_buff == RT_NULL)
    {
        free(pipe_buff);
        LOG_I("thread entry, malloc error\n");
        LOG_I("thread entry, exit\n");
        return;
    }

    memset(sock_buff, 0, BUFF_SIZE);
    memset(pipe_buff, 0, BUFF_SIZE);

    select_handle(temp, pipe_buff, sock_buff);
}
static void rt_tc_thread1_entry(void *param)
{
    //rt_tcpclient_t *temp = (rt_tcpclient_t*)param;
    //const char *str = "this is thread1\r\n";
    rt_uint32_t e = 0;

    while (1)
    {
        if (rt_event_recv(tc_event, TC_TCPCLIENT_CLOSE,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          0, &e) == RT_EOK)
        {
            rt_event_send(tc_event, TC_EXIT_THREAD);
            rt_tcpclient_close((rt_tcpclient_t *)param);
            return;
        }
        rt_thread_mdelay(100);

        // USER CODE BEGIN
        //rt_tcpclient_send(temp, str, strlen(str));
        // USER CODE END
    }
}

static void rt_tc_thread2_entry(void *param)
{
    //rt_tcpclient_t *temp = (rt_tcpclient_t*)param;
    //const char *str = "this is thread2\r\n";
    rt_uint32_t e = 0;

    while (1)
    {
        if (rt_event_recv(tc_event, TC_EXIT_THREAD,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          0, &e) == RT_EOK)
        {
            rt_event_delete(tc_event);
            g_ctrNet.end = 0;
            return;
        }

        rt_thread_mdelay(100);

        // USER CODE BEGIN
        //rt_tcpclient_send(temp, str, strlen(str));
        // USER CODE END
    }
}

rt_tcpclient_t *rt_tcpclient_start(const char *hostname, rt_uint32_t port, rt_uint8_t tcporudp)
{
    rt_tcpclient_t *thiz = RT_NULL;

    thiz = tcpclient_create();
    if (thiz == RT_NULL)
        return RT_NULL;

    if (socket_init(thiz, hostname, port, tcporudp) != 0)
        goto quit;

    if (pipe_init(thiz) != 0)
        goto quit;

    if (tcpclient_thread_init(thiz) != 0)
        goto quit;

    rt_kprintf("tcpcient start, tcpclient start succeed\n");
    return thiz;

quit:
    tcpclient_destory(&thiz);
    return RT_NULL;
}

void rt_tcpclient_close(rt_tcpclient_t *thiz)
{
    if (thiz == RT_NULL)
    {
        rt_kprintf("tcpclient deinit, param is NULL, tcpclient deinit failed\n");
        return;
    }

    if (socket_deinit(thiz) != 0)
        return;

    if (pipe_deinit(thiz) != 0)
        return;

    if (tcpclient_destory(&thiz) != 0)
        return;

    rt_kprintf("tcpclient deinit, tcpclient deinit succeed\n");
}

rt_int32_t rt_tcpclient_send(rt_tcpclient_t *thiz, const void *buff, rt_size_t len)
{
    rt_size_t bytes = 0;

    if (thiz == RT_NULL)
    {
        rt_kprintf("tcpclient send, param is NULL\n");
        return -1;
    }

    if (buff == RT_NULL)
    {
        rt_kprintf("tcpclient send, buff is NULL\n");
        return -1;
    }

    bytes = write(thiz->pipe_write_fd, buff, len);
    return bytes;
}

rt_int32_t rt_tcpclient_attach_rx_cb(rt_tcpclient_t *thiz, rx_cb_t cb)
{
    if (thiz == RT_NULL)
    {
        return -1;
    }

    thiz->rx = cb;
    return 0;
}


CCtrlNet g_ctrNet;
void CCtrlNet::lockit(void)
{
    while (lock) {
        rt_thread_mdelay(2);
    }

    lock = 1;
}
void CCtrlNet::unlock(void)
{
    lock = 0;
}

rt_err_t CCtrlNet::connect(void)
{
    rt_thread_t tid_net = RT_NULL;
    char strServiceIP[IP4ADDR_STRLEN_MAX] = {0};

    lockit();
    if (RT_NULL != socketHdl) {
        unlock();
        disconnect();
        lockit();
    }

    this->end = 0;

    netdev_ip4addr_ntoa_r(&g_paramNet.serviceIp, strServiceIP, IP4ADDR_STRLEN_MAX);
    /* �������� ip ��ַ & �����������Ķ˿ں� */
    socketHdl = rt_tcpclient_start(strServiceIP, g_paramNet.servicePort, g_paramNet.TcpOrUdp);
    if (socketHdl == RT_NULL)
    {
        LOG_I("tcpclient thread, param is NULL, exit");
        goto msk_err;
    }

    /* ע����ջص����� */
    rt_tcpclient_attach_rx_cb(socketHdl, rt_tc_rx_cb);

    tc_event = rt_event_create("tcev", RT_IPC_FLAG_FIFO);
    if (tc_event == RT_NULL)
    {
        LOG_I("tcpclient event, event create failed");
        goto msk_err;
    }

    tid_net = rt_thread_create("tcth1", rt_tc_thread1_entry, socketHdl, 1024, 10, 10);
    if (tid_net == RT_NULL)
    {
        LOG_I("tcpclient thread1, thread1 init failed");
        goto msk_err;
    }
    rt_thread_startup(tid_net);

    tid_net = rt_thread_create("tcth2", rt_tc_thread2_entry, socketHdl, 1024, 10, 10);
    if (tid_net == RT_NULL)
    {
        LOG_I("tcpclient thread2, thread2 init failed");
        goto msk_err;
    }
    rt_thread_startup(tid_net);

    LOG_I("ethnet connection succeed");

    unlock();
    return RT_EOK;

msk_err:
    unlock();
    return RT_ERROR;
}
rt_err_t CCtrlNet::disconnect(void)
{
    rt_thread_t ttid = RT_NULL;

    //if (RT_NULL == socketHdl) return RT_EOK;

    lockit();

    ttid = rt_thread_find(ETHNET_BUS_NAME);
    if (RT_NULL != ttid) {
        end = 1;
        //rt_thread_delete(ttid);
        while (end) {
            rt_thread_mdelay(2);
        }
    }
#if 0
    ttid = rt_thread_find((char*)"tcth1");
    if (RT_NULL != ttid) {
        rt_thread_delete(ttid);
    }
    ttid = rt_thread_find((char*)"tcth2");
    if (RT_NULL != ttid) {
        rt_thread_delete(ttid);
    }

    rt_event_delete(tc_event);
    rt_tcpclient_close((rt_tcpclient_t *)socketHdl);
    //socketHdl = RT_NULL;
#endif

    unlock();
    return RT_EOK;
}
rt_bool_t CCtrlNet::isConnected(void)
{
    if (RT_NULL == rt_thread_find(ETHNET_BUS_NAME)) {
        return RT_FALSE;
    }
    else {
        return RT_TRUE;
    }
    //if (RT_NULL == socketHdl) return RT_FALSE;
    //return RT_TRUE;
}

rt_int32_t CCtrlNet::send(const void *buff, rt_size_t len)
{
    rt_size_t bytes = 0;
    lockit();
    bytes = rt_tcpclient_send(socketHdl, buff, len);
    unlock();
    return bytes;
}

rt_netparam g_paramNet;

// 保存当前通讯参数至flash
extern void writeNetParam();
void saveNetCfg(void)
{
    writeNetParam();
}
// 通讯参数生效
// *pDev 参数结构, RT_NULL时表示按当前参数生效
rt_err_t setCurNetCfg(struct netdev *pDev)
{
    rt_bool_t reConnect = RT_FALSE;
    struct netdev *netdev = RT_NULL;

    if (RT_NULL == pDev) {
        rt_slist_t *node = RT_NULL;
        for (node = &(netdev_list->list); node; node = rt_slist_next(node)) {
            netdev = rt_list_entry(node, struct netdev, list);

            if(strncmp(ETHNET_IF_NAME, netdev->name, sizeof(netdev->name)) == 0) {
                break;
            }
        }
        if (netdev == RT_NULL ) {
            rt_kprintf("network interface: e0 not found!\r\n");
            return RT_ERROR;
        }
    }
    else {
        netdev = pDev;
    }

    if (RT_TRUE == g_ctrNet.isConnected()) {
        // 关闭当前连接 按新参数重新连接
        g_ctrNet.disconnect();
        reConnect = RT_TRUE;
    }

    // 参数生效
    netdev_set_ipaddr(netdev, &g_paramNet.ip_addr);
    netdev_set_gw(netdev,  &g_paramNet.gw_addr);
    netdev_set_netmask(netdev,  &g_paramNet.nm_addr);

    rt_thread_mdelay(10);

    if (RT_TRUE == reConnect) {
        // 重连
        g_ctrNet.connect();
    }

    return RT_EOK;
}

#if 0
// 获取本地端口号
rt_uint16_t getLocalPort(void)
{
    return g_paramNet.localPort;
}
#endif

rt_err_t setNetCfg(rt_netparam *netCfg)
{
    rt_netparam bakCfg;

    memcpy(&bakCfg, &g_paramNet, sizeof(rt_netparam));
    memcpy(&g_paramNet, netCfg, sizeof(rt_netparam));

    if (memcmp(&bakCfg, &g_paramNet, sizeof(rt_netparam))) {
        LOG_I("net config changed !!!\n");
        saveNetCfg();

        struct netdev *netdev = RT_NULL;
        rt_slist_t *node = RT_NULL;
        for (node = &(netdev_list->list); node; node = rt_slist_next(node)) {
            netdev = rt_list_entry(node, struct netdev, list);

            if(strncmp(ETHNET_IF_NAME, netdev->name, sizeof(netdev->name)) == 0) {
                break;
            }
        }
        if (netdev == RT_NULL ) {
            rt_kprintf("network interface: e0 not found!\r\n");
            return RT_ERROR;
        }

        // 网口参数立即生效
        setCurNetCfg(netdev);
    }

    return RT_EOK;
}
rt_err_t setDefNetCfg(void)
{
    rt_netparam defCfg;

    netdev_ip4addr_aton(DEF_IP_ADDR, &defCfg.ip_addr);
    netdev_ip4addr_aton(DEF_GW_ADDR, &defCfg.gw_addr);
    netdev_ip4addr_aton(DEF_NM_ADDR, &defCfg.nm_addr);
    netdev_ip4addr_aton(DEF_SER_IP_ADDR, &defCfg.serviceIp);
    defCfg.servicePort = DEF_SER_PORT;
    defCfg.TcpOrUdp = DEF_TCPORUDP;
    setNetCfg(&defCfg);

    return RT_EOK;
}
