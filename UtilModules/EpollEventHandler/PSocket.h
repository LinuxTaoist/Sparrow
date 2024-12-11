/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PSocket.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/05/07
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/05/07 | 1.0.0.1   | Xiang.D        | Create file
 *  2024/11/22 | 1.0.0.2   | Xiang.D        | Interface adjustment
 *---------------------------------------------------------------------------------------------------------------------
 *
 */

#ifndef __PSOCKET_H__
#define __PSOCKET_H__

#include <string>
#include <memory>
#include <functional>
#include "IEpollEvent.h"

#define DEFAULT_BUFFER_LIMIT    524288  // 512 * 1024

//---------------------------------------------------------------------------------------------------------------------
// SocketCommon: Socket common functions
//---------------------------------------------------------------------------------------------------------------------
class SocketCommon
{
    static bool IsIPAddress(const std::string& str);
    static std::string ResolveHostToIP(const std::string& host);
};

//---------------------------------------------------------------------------------------------------------------------
// PUdp: UDP
//---------------------------------------------------------------------------------------------------------------------
class PUdp : public IEpollEvent
{
public:
    /**
     * @brief 构建UDP实例
     *
     * 回调函数根据实际需求选择：
     * 第一个构造函数，参数依次为句柄、自定义数据，使用时需在回调中通过句柄自行读取缓存区数据
     * 第二个构造函数，参数依次为返回接收到的读取结果、数据、对端地址、对端端口、自定义数据，可省去读取缓存区数据的步骤
     * 若使用第二个构造函数，可通过读取结果是否为-1，判断是否读取成功，与read返回值一致
     *
     * @param cb 回调函数，当有数据到达时调用
     * @param arg 自定义数据，供回调使用
     */
    PUdp(const std::function<void(int fd, void*)>& cb, void* arg = nullptr)
        : IEpollEvent(-1, EPOLL_TYPE_SOCKET, arg), mCb1(cb), mCb2(nullptr) {}
    PUdp(const std::function<void(ssize_t, std::string, std::string addr, uint16_t port, void*)>& cb, void* arg = nullptr)
        : IEpollEvent(-1, EPOLL_TYPE_SOCKET, arg), mCb1(nullptr), mCb2(cb) {}
    virtual ~PUdp();

    /**
     * @brief 初始化socket
     *
     * @param port 本地端口 默认随机端口
     * @param rcvLen 接收缓冲区大小
     * @param sndLen 发送缓冲区大小
     * @return int32_t
     */
    int32_t AsUdp(uint16_t port = 0, int32_t rcvLen = DEFAULT_BUFFER_LIMIT, int32_t sndLen = DEFAULT_BUFFER_LIMIT);

    /**
     * @brief 发送数据
     *
     * @param bytes string类型数据
     * @param data void*类型数据
     * @param size 数据大小
     * @param addr 对端地址
     * @param port 对端端口
     *
     * @return int32_t
     */
    int32_t Write(const std::string& bytes, const std::string& addr, uint16_t port);
    int32_t Write(const void* data, size_t size, const std::string& addr, uint16_t port);

    /**
     * @brief 接收数据
     *
     * @param bytes string类型数据, 一次读取缓存区全部数据
     * @param data void*类型数据，读取指定长度数据
     * @param size 数据大小
     * @param addr 对端地址
     * @param port 对端端口
     * @return int32_t
     */
    int32_t Read(std::string& bytes, std::string& addr, uint16_t& port);
    int32_t Read(void* data, size_t size, std::string& addr, uint16_t& port);

    void*   EpollEvent(int fd, EpollType eType, void* arg) override;

private:
    std::function<void(int fd, void*)> mCb1;
    std::function<void(ssize_t, std::string, std::string addr, uint16_t port, void*)> mCb2;
};

//---------------------------------------------------------------------------------------------------------------------
// PTcpServer: TCP server
//---------------------------------------------------------------------------------------------------------------------
class PTcpServer : public IEpollEvent
{
public:
    /**
     * @brief 构建TCP server实例
     *
     * @param cb 回调函数，当有数据到达时调用
     * @param arg 自定义数据，供回调使用
     */
    PTcpServer(const std::function<void(int, void*)>& cb, void* arg = nullptr)
        : IEpollEvent(-1, EPOLL_TYPE_SOCKET, arg), mCb(cb) {}
    virtual ~PTcpServer();

    /**
     * @brief 初始化socket
     *
     * @add
     * @param port
     * @param backlog
     * @param addr
     * @return int32_t
     */
    int32_t AsTcpServer(uint16_t port, int32_t backlog, const std::string& addr = "");
    void*   EpollEvent(int fd, EpollType eType, void* arg) override;

private:
    std::function<void(int, void*)> mCb;
};

//---------------------------------------------------------------------------------------------------------------------
// PTcpClient: TCP client
//---------------------------------------------------------------------------------------------------------------------
class PTcpClient : public IEpollEvent
{
public:
    /**
     * @brief 使用指定句柄构建客户端对象，常用于服务端与已连接的客户端通信
     *
     * 回调函数根据实际需求选择：
     * 第一个构造函数，回调参数依次为句柄、自定义数据，使用时需在回调中通过句柄自行读取缓存区数据
     * 第二个构造函数，回调参数依次为读取结果、数据内容、自定义数据，可省去读取缓存区数据的步骤
     * 若使用第二个构造函数，可通过读取结果是否为-1，判断是否读取成功，与read返回值一致
     *
     * @param fd 已经连接的套接字文件描述符
     * @param cb 回调函数，当有数据到达时调用
     * @param arg 传递给回调函数的参数
     */
    PTcpClient(int fd, const std::function<void(int, void*)>& cb, void* arg = nullptr);
    PTcpClient(int fd, const std::function<void(ssize_t, std::string, void*)>& cb, void* arg = nullptr);

    /**
     * @brief 用于创建一个新的客户端对象，初始化一个新的套接字
     *
     * 回调函数的选择参考上述构造注释
     *
     * @param cb 回调函数，当有数据到达时调用
     * @param arg 传递给回调函数的参数
     */
    PTcpClient(const std::function<void(int, void*)>& cb, void* arg = nullptr);
    PTcpClient(const std::function<void(ssize_t, std::string, void*)>& cb, void* arg = nullptr);
    virtual ~PTcpClient();

    int32_t AsTcpClient(bool con = false, const std::string& srvAddr = "", uint16_t srvPort = 0,
                        int32_t rcvLen = DEFAULT_BUFFER_LIMIT, int32_t sndLen = DEFAULT_BUFFER_LIMIT);

    void*   EpollEvent(int fd, EpollType eType, void* arg) override;

private:
    std::function<void(int, void*)> mCb1;
    std::function<void(ssize_t, std::string, void*)> mCb2;
};

//---------------------------------------------------------------------------------------------------------------------
// PUnixDgram: Unix dgram
//---------------------------------------------------------------------------------------------------------------------
class PUnixDgram : public IEpollEvent
{
public:
    /**
     * @brief 构建Unix dgram实例
     *
     * 回调函数根据实际需求选择：
     * 第一个构造函数，参数依次为句柄、自定义数据，使用时需在回调中通过句柄自行读取缓存区数据
     * 第二个构造函数，参数依次为返回接收到的读取结果、数据、对端地址、对端端口、自定义数据，可省去读取缓存区数据的步骤
     * 若使用第二个构造函数，可通过读取结果是否为-1，判断是否读取成功，与read返回值一致
     *
     * @param cb 回调函数，当有数据到达时调用
     * @param arg 自定义数据，供回调使用
     */
    PUnixDgram(const std::function<void(int, void*)>& cb, void* arg = nullptr)
        : IEpollEvent(-1, EPOLL_TYPE_SOCKET, arg), mCb1(cb), mCb2(nullptr) {}
    PUnixDgram(const std::function<void(ssize_t, std::string, std::string, void*)>& cb, void* arg = nullptr)
        : IEpollEvent(-1, EPOLL_TYPE_SOCKET, arg), mCb1(nullptr), mCb2(cb) {}
    virtual ~PUnixDgram();

    /**
     * @brief 初始化socket
     *
     * @param srcPath 本地地址
     * @param rcvLen 接收缓存区大小
     * @param sndLen 发送缓存区大小
     * @return int32_t
     */
    int32_t AsUnixDgram(const std::string& srcPath, int32_t rcvLen = DEFAULT_BUFFER_LIMIT, int32_t sndLen = DEFAULT_BUFFER_LIMIT);

    /**
     * @brief 发送数据
     *
     * @param bytes string类型数据
     * @param data void*类型数据
     * @param size 数据大小
     * @param dstPath 对端地址
     *
     * @return int32_t
     */
    int32_t Write(const std::string& bytes, const std::string& dstPath);
    int32_t Write(const void* data, size_t size, const std::string& dstPath);

    /**
     * @brief 接收数据
     *
     * @param bytes string类型数据, 一次读取缓存区全部数据
     * @param data void*类型数据，读取指定长度数据
     * @param size 数据大小
     * @param dstPath 对端地址
     * @return int32_t
     */
    int32_t Read(std::string& bytes, std::string& dstPath);
    int32_t Read(void* data, size_t size, std::string& dstPath);

    void*   EpollEvent(int fd, EpollType eType, void* arg) override;

private:
    std::function<void(int, void*)> mCb1;
    std::function<void(ssize_t, std::string, std::string, void*)> mCb2;
};

//---------------------------------------------------------------------------------------------------------------------
// PUnixStreamServer: Unix stream server
//---------------------------------------------------------------------------------------------------------------------
class PUnixStreamServer : public IEpollEvent
{
public:
    PUnixStreamServer(const std::function<void(int, void*)>& cb, void* arg = nullptr)
        : IEpollEvent(-1, EPOLL_TYPE_SOCKET, arg), mCb(cb) {}
    virtual ~PUnixStreamServer();

    /**
     * @brief 初始化socket
     *
     * @param port
     * @param backlog
     * @return int32_t
     */
    int32_t AsUnixStreamServer(const std::string& srvPath, int32_t backlog);
    void*   EpollEvent(int fd, EpollType eType, void* arg) override;

private:
    std::function<void(int, void*)> mCb;
};

//---------------------------------------------------------------------------------------------------------------------
// PUnixStreamClient : Unix stream client
//---------------------------------------------------------------------------------------------------------------------
class PUnixStreamClient : public IEpollEvent
{
public:
    /**
     * @brief 使用指定句柄构建客户端对象，常用于服务端与已连接的客户端通信
     *
     * 回调函数根据实际需求选择：
     * 第一个构造函数，回调参数依次为句柄、自定义数据，使用时需在回调中通过句柄自行读取缓存区数据
     * 第二个构造函数，回调参数依次为读取结果、数据内容、自定义数据，可省去读取缓存区数据的步骤
     * 若使用第二个构造函数，可通过读取结果是否为-1，判断是否读取成功，与read返回值一致
     *
     * @param fd 已经连接的套接字文件描述符
     * @param cb 回调函数，当有数据到达时调用
     * @param arg 传递给回调函数的参数
     */
    PUnixStreamClient(int fd, const std::function<void(int, void*)>& cb = nullptr, void* arg = nullptr);
    PUnixStreamClient(int fd, const std::function<void(ssize_t, std::string, void*)>& cb, void* arg = nullptr);

    /**
     * @brief 用于创建一个新的客户端对象，初始化一个新的套接字
     *
     * 回调函数的选择参考上述构造注释
     *
     * @param cb 回调函数，当有数据到达时调用
     * @param arg 传递给回调函数的参数
     */
    PUnixStreamClient(const std::function<void(int, void*)>& cb, void* arg = nullptr);
    PUnixStreamClient(const std::function<void(ssize_t, std::string, void*)>& cb, void* arg = nullptr);
    virtual ~PUnixStreamClient();

    int32_t AsUnixStreamClient(bool con = false, const std::string& srvPath = "", const std::string& cliPath = "");
    void*   EpollEvent(int fd, EpollType eType, void* arg) override;

private:
    std::function<void(int, void*)> mCb1;
    std::function<void(ssize_t, std::string, void*)> mCb2;
};

#endif // __PSOCKET_H__
