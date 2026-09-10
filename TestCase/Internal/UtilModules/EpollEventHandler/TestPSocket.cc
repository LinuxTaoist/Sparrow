/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestPSocket.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : PSocket 套接字封装（UDP/TCP/Unix）内部测试
 *  @date       : 2026/09/09
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/09/09 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <ctime>
#include <cstdio>
#include <poll.h>
#include <unistd.h>
#include <atomic>
#include <string>
#include <vector>
#include "PSocket.h"
#include "gtest/gtest.h"

namespace {
std::string MakeSockPath(const std::string& prefix)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    char buf[96] = {};
    std::snprintf(buf, sizeof(buf), "/tmp/%s_%d_%ld.sock", prefix.c_str(), getpid(), ts.tv_nsec % 1000000L);
    return std::string(buf);
}
}

// 涉及 unix socket 文件的测试用 fixture 统一清理，保证断言失败时也不残留
class UtilModules_PSocket_Sock : public ::testing::Test {
protected:
    void TearDown() override {
        for (const auto& p : mSockPaths) {
            unlink(p.c_str());
        }
        mSockPaths.clear();
    }

    std::vector<std::string> mSockPaths;
};

// ---------- SocketCommon ----------
// 测试 IP 地址识别
TEST(UtilModules_PSocket, IsIPAddress)
{
    EXPECT_TRUE(SocketCommon::IsIPAddress("127.0.0.1"));
    EXPECT_TRUE(SocketCommon::IsIPAddress("192.168.1.1"));
    EXPECT_TRUE(SocketCommon::IsIPAddress("::1"));
    EXPECT_TRUE(SocketCommon::IsIPAddress("2001:db8::1"));
    EXPECT_FALSE(SocketCommon::IsIPAddress("localhost"));
    EXPECT_FALSE(SocketCommon::IsIPAddress("not-an-ip"));
    EXPECT_FALSE(SocketCommon::IsIPAddress(""));
}

// 测试主机名解析为 IP
TEST(UtilModules_PSocket, ResolveHostToIP)
{
    // 已经是 IP 则直接返回
    EXPECT_EQ(SocketCommon::ResolveHostToIP("127.0.0.1"), "127.0.0.1");

    // localhost 解析为回环地址
    std::string resolved = SocketCommon::ResolveHostToIP("localhost");
    EXPECT_FALSE(resolved.empty());

    // 无效域名返回空
    EXPECT_EQ(SocketCommon::ResolveHostToIP("invalid.invalid.invalid"), "");
}

// ---------- PUdp ----------
// 测试 UDP 绑定与非法参数校验
TEST(UtilModules_PSocket, UdpBindAndInvalidParams)
{
    PUdp udp([](int32_t, void*) {}, nullptr);
    ASSERT_EQ(udp.AsUdp(0), 0);   // 随机端口绑定成功

    // 无效参数
    EXPECT_EQ(udp.Write(nullptr, 0, "127.0.0.1", 0), -1);
    EXPECT_EQ(udp.Write("", "127.0.0.1", 0), -1);

    std::string addr;
    uint16_t port = 0;
    EXPECT_EQ(udp.Read(nullptr, 0, addr, port), -1);
}

// 测试 UDP 回环收发
TEST(UtilModules_PSocket, UdpLoopback)
{
    PUdp server([](int32_t, void*) {}, nullptr);
    ASSERT_EQ(server.AsUdp(38921), 0);

    PUdp client([](int32_t, void*) {}, nullptr);
    ASSERT_EQ(client.AsUdp(0), 0);

    const std::string payload = "udp-loopback-hello";
    int32_t sent = client.Write(payload, "127.0.0.1", 38921);
    ASSERT_EQ(sent, static_cast<int32_t>(payload.size()));

    struct pollfd pfd;
    pfd.fd = server.GetEvtFd();
    pfd.events = POLLIN;
    pfd.revents = 0;
    int32_t pr = poll(&pfd, 1, 2000);
    ASSERT_GT(pr, 0);

    std::string recv;
    std::string srcAddr;
    uint16_t srcPort = 0;
    int32_t n = server.Read(recv, srcAddr, srcPort);
    EXPECT_EQ(n, static_cast<int32_t>(payload.size()));
    EXPECT_EQ(recv, payload);
    EXPECT_EQ(srcAddr, "127.0.0.1");
}

// ---------- PTcpServer / PTcpClient ----------
// 测试 TCP 服务端绑定与客户端构造
TEST(UtilModules_PSocket, TcpServerBindAndClientConstruct)
{
    PTcpServer server([](int32_t, void*) {}, nullptr);
    ASSERT_EQ(server.AsTcpServer(0, 5), 0);   // 随机端口

    PTcpClient client([](int32_t, void*) {}, nullptr);
    ASSERT_EQ(client.AsTcpClient(false), 0);   // 不连接，仅设置选项
}

// 测试 TCP 客户端连接被拒绝
TEST(UtilModules_PSocket, TcpClientConnectRefused)
{
    PTcpClient client([](int32_t, void*) {}, nullptr);
    // 连接到未监听的端口，应失败
    EXPECT_EQ(client.AsTcpClient(true, "127.0.0.1", 1, DEFAULT_BUFFER_LIMIT, DEFAULT_BUFFER_LIMIT, 500), -1);
}

// 测试 TCP 服务端接受连接并客户端连接成功
TEST_F(UtilModules_PSocket_Sock, TcpServerAcceptAndClientConnect)
{
    const uint16_t port = 38922;
    std::atomic<int32_t> acceptCount(0);
    PTcpServer server([&](int32_t cliFd, void*) {
        acceptCount.fetch_add(1);
        close(cliFd);
    });
    ASSERT_EQ(server.AsTcpServer(port, 5), 0);

    PTcpClient client([](int32_t, void*) {}, nullptr);
    ASSERT_EQ(client.AsTcpClient(true, "127.0.0.1", port), 0);

    server.EpollEvent(server.GetEvtFd(), EPOLL_TYPE_SOCKET, nullptr);
    EXPECT_GE(acceptCount.load(), 1);
}

// 测试 UDP 事件回调（mCb1 分支）
TEST(UtilModules_PSocket, UdpEpollEventCb1)
{
    int32_t hitFd = -1;
    PUdp udp([&](int32_t fd, void*) { hitFd = fd; }, nullptr);
    ASSERT_EQ(udp.AsUdp(0), 0);

    udp.EpollEvent(udp.GetEvtFd(), EPOLL_TYPE_SOCKET, nullptr);
    EXPECT_EQ(hitFd, udp.GetEvtFd());
}

// 测试 UDP 事件回调（mCb2 分支）
TEST(UtilModules_PSocket, UdpEpollEventCb2)
{
    PUdp udp([](ssize_t, std::string, std::string, uint16_t, void*) {}, nullptr);
    ASSERT_EQ(udp.AsUdp(0), 0);

    udp.EpollEvent(udp.GetEvtFd(), EPOLL_TYPE_SOCKET, nullptr);
}

// 测试 TCP 客户端事件回调（mCb1 分支）
TEST(UtilModules_PSocket, TcpClientEpollEventCb1)
{
    int32_t hitFd = -1;
    PTcpClient client([&](int32_t fd, void*) { hitFd = fd; }, nullptr);
    ASSERT_EQ(client.AsTcpClient(false), 0);

    client.EpollEvent(client.GetEvtFd(), EPOLL_TYPE_SOCKET, nullptr);
    EXPECT_EQ(hitFd, client.GetEvtFd());
}

// 测试 TCP 客户端事件回调（mCb2 分支）
TEST(UtilModules_PSocket, TcpClientEpollEventCb2)
{
    ssize_t recvSize = -1;
    PTcpClient client([&](ssize_t size, std::string, void*) { recvSize = size; }, nullptr);
    ASSERT_EQ(client.AsTcpClient(false), 0);

    client.EpollEvent(client.GetEvtFd(), EPOLL_TYPE_SOCKET, nullptr);
    // 无数据时读取返回 -1（EAGAIN）
    EXPECT_LT(recvSize, 0);
}

// 测试 Unix 数据报事件回调（mCb1 分支）
TEST_F(UtilModules_PSocket_Sock, UnixDgramEpollEventCb1)
{
    const std::string srvPath = MakeSockPath("spr_udgram_cb_srv");
    mSockPaths.push_back(srvPath);

    int32_t hitFd = -1;
    PUnixDgram udp([&](int32_t fd, void*) { hitFd = fd; }, nullptr);
    ASSERT_EQ(udp.AsUnixDgram(srvPath), 0);

    udp.EpollEvent(udp.GetEvtFd(), EPOLL_TYPE_SOCKET, nullptr);
    EXPECT_EQ(hitFd, udp.GetEvtFd());
}

// 测试 Unix 流式客户端事件回调（mCb1 分支）
TEST_F(UtilModules_PSocket_Sock, UnixStreamClientEpollEventCb1)
{
    const std::string cliPath = MakeSockPath("spr_ustream_cb_cli");
    mSockPaths.push_back(cliPath);

    int32_t hitFd = -1;
    PUnixStreamClient client([&](int32_t fd, void*) { hitFd = fd; }, nullptr);
    ASSERT_EQ(client.AsUnixStreamClient(false, "", cliPath), 0);

    client.EpollEvent(client.GetEvtFd(), EPOLL_TYPE_SOCKET, nullptr);
    EXPECT_EQ(hitFd, client.GetEvtFd());
}

// ---------- PUnixDgram ----------
// 测试 Unix 数据报套接字回环收发
TEST_F(UtilModules_PSocket_Sock, UnixDgramLoopback)
{
    const std::string srvPath = MakeSockPath("spr_udgram_srv");
    const std::string cliPath = MakeSockPath("spr_udgram_cli");
    mSockPaths.push_back(srvPath);
    mSockPaths.push_back(cliPath);

    PUnixDgram server([](int32_t, void*) {}, nullptr);
    ASSERT_EQ(server.AsUnixDgram(srvPath), 0);

    PUnixDgram client([](int32_t, void*) {}, nullptr);
    ASSERT_EQ(client.AsUnixDgram(cliPath), 0);

    const std::string payload = "unix-dgram-hello";
    int32_t sent = client.Write(payload, srvPath);
    ASSERT_EQ(sent, static_cast<int32_t>(payload.size()));

    struct pollfd pfd;
    pfd.fd = server.GetEvtFd();
    pfd.events = POLLIN;
    pfd.revents = 0;
    int32_t pr = poll(&pfd, 1, 2000);
    ASSERT_GT(pr, 0);

    std::string recv;
    std::string srcPath;
    char buf[2048] = {};
    int32_t n = server.Read(buf, sizeof(buf), srcPath);
    EXPECT_EQ(n, static_cast<int32_t>(payload.size()));
    EXPECT_EQ(std::string(buf, n), payload);
    EXPECT_EQ(srcPath, cliPath);
}

// 测试 Unix 数据报套接字非法参数校验
TEST_F(UtilModules_PSocket_Sock, UnixDgramInvalidParams)
{
    const std::string srvPath = MakeSockPath("spr_udgram_inv");
    mSockPaths.push_back(srvPath);

    PUnixDgram udp([](int32_t, void*) {}, nullptr);
    ASSERT_EQ(udp.AsUnixDgram(srvPath), 0);

    EXPECT_EQ(udp.Write(nullptr, 0, srvPath), -1);

    std::string path;
    EXPECT_EQ(udp.Read(nullptr, 0, path), -1);
}

// ---------- PUnixStream ----------
// 测试 Unix 流式套接字服务端绑定与客户端构造
TEST_F(UtilModules_PSocket_Sock, UnixStreamServerBindAndClientConstruct)
{
    const std::string srvPath = MakeSockPath("spr_ustream_srv");
    const std::string cliPath = MakeSockPath("spr_ustream_cli");
    mSockPaths.push_back(srvPath);
    mSockPaths.push_back(cliPath);

    PUnixStreamServer server([](int32_t, void*) {}, nullptr);
    ASSERT_EQ(server.AsUnixStreamServer(srvPath, 5), 0);

    PUnixStreamClient client([](int32_t, void*) {}, nullptr);
    ASSERT_EQ(client.AsUnixStreamClient(false, srvPath, cliPath), 0);
}

// 测试 Unix 流式套接字服务端接受连接并客户端连接成功
TEST_F(UtilModules_PSocket_Sock, UnixStreamServerAcceptAndClientConnect)
{
    const std::string srvPath = MakeSockPath("spr_ustream_accept_srv");
    const std::string cliPath = MakeSockPath("spr_ustream_accept_cli");
    mSockPaths.push_back(srvPath);
    mSockPaths.push_back(cliPath);

    std::atomic<int32_t> acceptCount(0);
    PUnixStreamServer server([&](int32_t cliFd, void*) {
        acceptCount.fetch_add(1);
        close(cliFd);
    });
    ASSERT_EQ(server.AsUnixStreamServer(srvPath, 5), 0);

    PUnixStreamClient client([](int32_t, void*) {}, nullptr);
    ASSERT_EQ(client.AsUnixStreamClient(true, srvPath, cliPath), 0);

    server.EpollEvent(server.GetEvtFd(), EPOLL_TYPE_SOCKET, nullptr);
    EXPECT_GE(acceptCount.load(), 1);
}
