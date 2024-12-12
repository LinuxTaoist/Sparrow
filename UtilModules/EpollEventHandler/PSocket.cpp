/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PSocket.cpp
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
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "PSocket.h"

using namespace std;

#define SPR_LOGD(fmt, args...) printf("%4d PSocket D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%4d PSocket E: " fmt, __LINE__, ##args)

//---------------------------------------------------------------------------------------------------------------------
// SocketCommon: Socket common functions
//---------------------------------------------------------------------------------------------------------------------
bool SocketCommon::IsIPAddress(const std::string& str)
{
    struct sockaddr_in sa;
    struct sockaddr_in6 sa6;
    return (inet_pton(AF_INET, str.c_str(), &(sa.sin_addr)) == 1 ||
            inet_pton(AF_INET6, str.c_str(), &(sa6.sin6_addr)) == 1);
}

std::string SocketCommon::ResolveHostToIP(const std::string& host)
{
    if (IsIPAddress(host)) {
        return host;
    }

    int status = 0;
    char ipstr[INET6_ADDRSTRLEN] = {0};
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;     // 不限制 IPv4 或 IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP
    if ((status = getaddrinfo(host.c_str(), nullptr, &hints, &res)) != 0) {
        SPR_LOGE("getaddrinfo %s failed! (%s)\n", host.c_str(), strerror(errno));
        return "";
    }

    void *addr;
    if (res->ai_family == AF_INET) { // IPv4
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
        addr = &(ipv4->sin_addr);
    } else {                        // IPv6
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)res->ai_addr;
        addr = &(ipv6->sin6_addr);
    }

    inet_ntop(res->ai_family, addr, ipstr, sizeof(ipstr));
    freeaddrinfo(res);
    return std::string(ipstr);
}


//---------------------------------------------------------------------------------------------------------------------
// PUdp: UDP with PSOCKET_UDP
//---------------------------------------------------------------------------------------------------------------------
PUdp::~PUdp()
{
    Close();
}

int32_t PUdp::AsUdp(uint16_t port, int32_t rcvLen, int32_t sndLen)
{
    mEvtFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (mEvtFd == -1) {
        SPR_LOGE("socket failed! (%s)\n", strerror(errno));
        return -1;
    }

    int flags = fcntl(mEvtFd, F_GETFL, 0);
    fcntl(mEvtFd, F_SETFL, flags | O_NONBLOCK);

    if (setsockopt(mEvtFd, SOL_SOCKET, SO_RCVBUF, &rcvLen, sizeof(rcvLen)) < 0) {
        SPR_LOGE("setsockopt %d failed! (%s)\n", mEvtFd, strerror(errno));
        Close();
        return -1;
    }

    if (setsockopt(mEvtFd, SOL_SOCKET, SO_SNDBUF, &sndLen, sizeof(sndLen)) < 0) {
        SPR_LOGE("setsockopt %d failed! (%s)\n", mEvtFd, strerror(errno));
        Close();
        return -1;
    }

    struct sockaddr_in myAddr;
    bzero(&myAddr, sizeof(myAddr));
    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myAddr.sin_port = htons(port);
    int32_t ret = bind(mEvtFd, (struct sockaddr*)&myAddr, sizeof(myAddr));
    if (ret == -1) {
        Close();
        SPR_LOGE("bind failed! (%s)\n", strerror(errno));
        return ret;
    }

    AddToPoll();
    return ret;
}

int32_t PUdp::Write(const std::string& bytes, const std::string& addr, uint16_t port)
{
    return Write(bytes.c_str(), bytes.size(), addr, port);
}

int32_t PUdp::Write(const void* data, size_t size, const std::string& addr, uint16_t port)
{
    if (!data || size <= 0) {
        SPR_LOGE("Invalid params! data = %p, size = %zu\n", data, size);
        return -1;
    }

    struct sockaddr_in dstAddr;
    bzero(&dstAddr, sizeof(struct sockaddr_in));
    dstAddr.sin_family = AF_INET;
    dstAddr.sin_addr.s_addr = inet_addr(addr.c_str());
    dstAddr.sin_port = htons(port);
    int32_t ret = sendto(mEvtFd, data, size, 0, (struct sockaddr*)&dstAddr, sizeof(dstAddr));
    if (ret == -1) {
        SPR_LOGE("sendto failed! (%s)\n", strerror(errno));
    }

    return ret;
}

int32_t PUdp::Read(std::string& bytes, std::string& addr, uint16_t& port)
{
    char buf[2048] = {0};
    std::string tmpAddr;
    uint16_t tmpPort;
    int32_t bytesRead = Read(buf, sizeof(buf), tmpAddr, tmpPort);
    if (bytesRead > 0) {
        bytes.assign(buf, bytesRead);
        addr = tmpAddr;
        port = tmpPort;
    }

    return bytesRead;
}

int32_t PUdp::Read(void* data, size_t size, std::string& addr, uint16_t& port)
{
    if (!data || size <= 0) {
        SPR_LOGE("Invalid params! data = %p, size = %zu\n", data, size);
        return -1;
    }

    struct sockaddr_in dstAddr;
    socklen_t addrSize = sizeof(dstAddr);
    int32_t ret = recvfrom(mEvtFd, data, size, 0, (struct sockaddr*)&dstAddr, &addrSize);
    if (ret == -1) {
        SPR_LOGE("recvfrom failed! (%s)\n", strerror(errno));
    }

    addr = inet_ntoa(dstAddr.sin_addr);
    port = ntohs(dstAddr.sin_port);
    return ret;
}

void* PUdp::EpollEvent(int fd, EpollType eType, void* arg)
{
    arg = arg ? arg : this;
    if (mCb1) {
        mCb1(fd, arg);
    }

    if (mCb2) {
        std::string dstAddr;
        uint16_t dstPort;
        std::string bytes;
        ssize_t ret = Read(bytes, dstAddr, dstPort);
        mCb2(ret, bytes, dstAddr, dstPort, arg);
    }

    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------------
// PTcpServer: TCP server
//---------------------------------------------------------------------------------------------------------------------
PTcpServer::~PTcpServer()
{
    Close();
}

int32_t PTcpServer::AsTcpServer(uint16_t port, int32_t backlog)
{
    mEvtFd = socket(AF_INET, SOCK_STREAM, 0);
    if (mEvtFd == -1) {
        SPR_LOGE("socket failed! (%s)\n", strerror(errno));
        return -1;
    }

    int flags = fcntl(mEvtFd, F_GETFL, 0);
    fcntl(mEvtFd, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in myAddr;
    bzero(&myAddr, sizeof(myAddr));
    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myAddr.sin_port = htons(port);
    int ret = bind(mEvtFd, (struct sockaddr *)&myAddr, sizeof(myAddr));
    if (ret < 0) {
        SPR_LOGE("bind failed! (%s)\n", strerror(errno));
        Close();
        return ret;
    }

    ret = listen(mEvtFd, backlog);
    if (ret < 0) {
        SPR_LOGE("bind failed! (%s)\n", strerror(errno));
        Close();
        return ret;
    }

    AddToPoll();
    return ret;
}

void* PTcpServer::EpollEvent(int fd, EpollType eType, void* arg)
{
    struct sockaddr_in client;
    socklen_t len = (socklen_t)sizeof(client);
    int cliFd = accept(fd, (struct sockaddr *)&client, &len);
    if (cliFd < 0) {
        SPR_LOGE("accept failed! (%s)\n", strerror(errno));
        return nullptr;
    }

    arg = arg ? arg : this;
    if (mCb) {
        mCb(cliFd, arg);
    }

    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------------
// PTcpClient: TCP client
//---------------------------------------------------------------------------------------------------------------------
PTcpClient::PTcpClient(int fd, const std::function<void(int, void*)>& cb, void* arg)
    : IEpollEvent(fd, EPOLL_TYPE_SOCKET, arg), mCb1(cb), mCb2(nullptr)
{
    int op = 1;
    if (setsockopt(mEvtFd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) < 0) {
        SPR_LOGE("setsockopt %d failed! (%s)\n", mEvtFd, strerror(errno));
        Close();
        SetReady(false);
    }
}

PTcpClient::PTcpClient(int fd, const std::function<void(ssize_t, std::string, void*)>& cb, void* arg)
    : IEpollEvent(fd, EPOLL_TYPE_SOCKET, arg), mCb1(nullptr), mCb2(cb)
{
    int op = 1;
    if (setsockopt(mEvtFd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) < 0) {
        SPR_LOGE("setsockopt %d failed! (%s)\n", mEvtFd, strerror(errno));
        Close();
        SetReady(false);
    }
}

PTcpClient::PTcpClient(const std::function<void(int, void*)>& cb, void* arg)
    : IEpollEvent(-1, EPOLL_TYPE_SOCKET, arg), mCb1(cb), mCb2(nullptr)
{
    mEvtFd = socket(AF_INET, SOCK_STREAM, 0);
    if (mEvtFd == -1) {
        SPR_LOGE("socket failed! (%s)\n", strerror(errno));
        SetReady(false);
    }

    int op = 1;
    if (setsockopt(mEvtFd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) < 0) {
        SPR_LOGE("setsockopt %d failed! (%s)\n", mEvtFd, strerror(errno));
        Close();
        SetReady(false);
    }
}

PTcpClient::PTcpClient(const std::function<void(ssize_t, std::string, void*)>& cb, void* arg)
    : IEpollEvent(-1, EPOLL_TYPE_SOCKET, arg), mCb1(nullptr), mCb2(cb)
{
    mEvtFd = socket(AF_INET, SOCK_STREAM, 0);
    if (mEvtFd == -1) {
        SPR_LOGE("socket failed! (%s)\n", strerror(errno));
        SetReady(false);
    }

    int op = 1;
    if (setsockopt(mEvtFd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) < 0) {
        SPR_LOGE("setsockopt %d failed! (%s)\n", mEvtFd, strerror(errno));
        Close();
        SetReady(false);
    }
}

PTcpClient::~PTcpClient()
{
    Close();
}

int32_t PTcpClient::AsTcpClient(bool con, const std::string& srvAddr, uint16_t srvPort, int32_t rcvLen, int32_t sndLen)
{
    int flags, op;
    struct linger so_linger;
    struct sockaddr_in server;

    op = rcvLen;
    if (setsockopt(mEvtFd, SOL_SOCKET, SO_RCVBUF, &op, sizeof(op)) < 0) {
        SPR_LOGE("setsockopt %d failed! (%s)\n", mEvtFd, strerror(errno));
        Close();
        return -1;
    }

    op = sndLen;
    if (setsockopt(mEvtFd, SOL_SOCKET, SO_SNDBUF, &op, sizeof(op)) < 0) {
        SPR_LOGE("setsockopt %d failed! (%s)\n", mEvtFd, strerror(errno));
        Close();
        return -1;
    }

    so_linger.l_onoff = 1; /* when send to sock and it close, keep time */
    so_linger.l_linger = 0; /* keep time second */
    if (setsockopt(mEvtFd, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger)) < 0) {
        SPR_LOGE("setsockopt %d failed! (%s)\n", mEvtFd, strerror(errno));
        Close();
        return -1;
    }

    op = 1; // disable Nagle, TCP_NODELAY is the only used IPPROTO_TCP layer.
    if (setsockopt(mEvtFd, IPPROTO_TCP, 1, &op, sizeof(op)) < 0) {
        SPR_LOGE("setsockopt %d failed! (%s)\n", mEvtFd, strerror(errno));
        Close();
        return -1;
    }

    if (con) {
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr(srvAddr.c_str());
        server.sin_port = htons(srvPort);

        // Linux TCP repeat connect directly return EISCONN.
        int ret = connect(mEvtFd, (struct sockaddr*)&server, sizeof(server));
        if (ret < 0 && errno != EISCONN) {
            SPR_LOGE("connect %s failed! (%s)\n", srvAddr.c_str(), strerror(errno));
            Close();
            return -1;
        }
    }

    flags = fcntl(mEvtFd, F_GETFL, 0);
    fcntl(mEvtFd, F_SETFL, flags | O_NONBLOCK);
    AddToPoll();
    return 0;
}

void* PTcpClient::EpollEvent(int fd, EpollType eType, void* arg)
{
    arg = arg ? arg : this;
    if (mCb1) {
        mCb1(fd, arg);
    }

    if (mCb2) {
        std::string bytes;
        ssize_t ret = IEpollEvent::Read(fd, bytes);
        mCb2(ret, bytes, arg);
    }

    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------------
// PUnixDgram: Unix dgram
//---------------------------------------------------------------------------------------------------------------------
PUnixDgram::~PUnixDgram()
{
    Close();
}

int32_t PUnixDgram::AsUnixDgram(const std::string& srcPath, int32_t rcvLen, int32_t sndLen)
{
    unlink(srcPath.c_str());
    mEvtFd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (mEvtFd < 0) {
        SPR_LOGE("socket failed! (%s)\n", strerror(errno));
        return -1;
    }

    int op = 1;
    if (setsockopt(mEvtFd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) < 0) {
        SPR_LOGE("setsockopt %d failed! (%s)\n", mEvtFd, strerror(errno));
        Close();
        return -1;
    }

    // bind self socket path
    struct sockaddr_un myAddr;
    if (srcPath.size() > sizeof(myAddr.sun_path) - 1) {
        SPR_LOGE("srcPath too long! len = %d, limit = %d\n", (int)srcPath.size(), (int)sizeof(myAddr.sun_path));
        Close();
        return -1;
    }

    bzero(&myAddr, sizeof(struct sockaddr_un));
    strncpy(myAddr.sun_path, srcPath.c_str(), srcPath.size());
    myAddr.sun_family = AF_UNIX;
    int ret = bind(mEvtFd, (struct sockaddr *)&myAddr, sizeof(struct sockaddr_un));
    if (ret < 0) {
        SPR_LOGE("bind failed! (%s)\n", strerror(errno));
        return ret;
    }

    AddToPoll();
    return ret;
}

int32_t PUnixDgram::Write(const std::string& bytes, const std::string& dstPath)
{
    return Write(bytes.c_str(), bytes.size(), dstPath);
}

int32_t PUnixDgram::Write(const void* data, size_t size, const std::string& dstPath)
{
    if (!data || size <= 0) {
        SPR_LOGE("Invalid params! data = %p, size = %zu\n", data, size);
        return -1;
    }

    struct sockaddr_un dstAddr;
    bzero(&dstAddr, sizeof(struct sockaddr_un));
    strncpy(dstAddr.sun_path, dstPath.c_str(), dstPath.size());
    dstAddr.sun_family = AF_UNIX;

    int32_t ret = sendto(mEvtFd, data, size, 0, (struct sockaddr*)&dstAddr, sizeof(dstAddr));
    if (ret == -1) {
        SPR_LOGE("sendto %d failed! (%s)\n", mEvtFd, strerror(errno));
    }

    return ret;
}

int32_t PUnixDgram::Read(std::string& bytes, std::string& dstPath)
{
    std::string tempBytes;
    int32_t totalBytesRead = 0;
    bool firstRead = true;

    while (true) {
        char buf[2048];
        std::string currentPath;
        int32_t bytesRead = Read(buf, sizeof(buf), currentPath);

        if (bytesRead == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            } else {
                SPR_LOGE("Read failed! (%s)\n", strerror(errno));
                return -1;
            }
        } else if (bytesRead == 0) {
            break;
        }

        totalBytesRead += bytesRead;
        tempBytes.append(buf, bytesRead);

        // Update path only once
        if (firstRead) {
            dstPath = currentPath;
            firstRead = false;
        }
    }

    bytes.swap(tempBytes);
    return totalBytesRead;
}

int32_t PUnixDgram::Read(void* data, size_t size, std::string& dstPath)
{
    if (!data || size <= 0) {
        SPR_LOGE("Invalid params! data = %p, size = %zu\n", data, size);
        return -1;
    }

    struct sockaddr_un dstAddr;
    socklen_t addrSize = sizeof(struct sockaddr_un);
    int ret = recvfrom(mEvtFd, data, size, 0, (struct sockaddr *)&dstAddr, &addrSize);
    if (ret == -1) {
        SPR_LOGE("recvfrom failed! (%s)\n", strerror(errno));
        return -1;
    }

    dstPath = dstAddr.sun_path;
    return ret;
}

void* PUnixDgram::EpollEvent(int fd, EpollType eType, void* arg)
{
    arg = arg ? arg : this;
    if (mCb1) {
        mCb1(fd, arg);
    }

    if (mCb2) {
        std::string dstPath;
        std::string bytes;
        ssize_t ret = Read(bytes, dstPath);
        mCb2(ret, bytes, dstPath, arg);
    }

    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------------
// PUnixStreamServer: Unix stream server
//---------------------------------------------------------------------------------------------------------------------
PUnixStreamServer::~PUnixStreamServer()
{
    Close();
}

int32_t PUnixStreamServer::AsUnixStreamServer(const std::string& srvPath, int32_t backlog)
{
    unlink(srvPath.c_str());
    mEvtFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (mEvtFd < 0) {
        SPR_LOGE("socket failed! (%s)\n", strerror(errno));
        return -1;
    }

    int flags = fcntl(mEvtFd, F_GETFL, 0);
    fcntl(mEvtFd, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_un myAddr;
    bzero(&myAddr, sizeof(struct sockaddr_un));
    strncpy(myAddr.sun_path, srvPath.c_str(), sizeof(myAddr.sun_path) - 1);
    myAddr.sun_path[sizeof(myAddr.sun_path) - 1] = '\0';
    myAddr.sun_family = AF_UNIX;

    int32_t ret = bind(mEvtFd, (struct sockaddr *)&myAddr, sizeof(myAddr));
    if (ret == -1){
        SPR_LOGE("bind failed! (%s)\n", strerror(errno));
        Close();
        return -1;
    }

    ret = listen(mEvtFd, backlog);
    if (ret == -1){
        SPR_LOGE("listen failed! (%s)\n", strerror(errno));
        Close();
        return -1;
    }

    AddToPoll();
    return ret;
}

void* PUnixStreamServer::EpollEvent(int fd, EpollType eType, void* arg)
{
    struct sockaddr_un client;
    socklen_t len = (socklen_t)sizeof(client);
    int cliFd = accept(fd, (struct sockaddr *)&client, &len);
    if (cliFd < 0) {
        SPR_LOGE("accept failed! (%s)\n", strerror(errno));
        return nullptr;
    }

    arg = arg ? arg : this;
    if (mCb) {
        mCb(cliFd, arg);
    }

    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------------
// PUnixStreamClient : Unix stream client
//---------------------------------------------------------------------------------------------------------------------
PUnixStreamClient::PUnixStreamClient(int fd, const std::function<void(int, void*)>& cb, void* arg)
    : IEpollEvent(fd, EPOLL_TYPE_SOCKET, arg), mCb1(cb), mCb2(nullptr)
{
    int op = 1;
    if (setsockopt(mEvtFd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) < 0) {
        SPR_LOGE("setsockopt %d failed! (%s)\n", mEvtFd, strerror(errno));
        Close();
        SetReady(false);
    }
}

PUnixStreamClient::PUnixStreamClient(int fd, const std::function<void(ssize_t, std::string, void*)>& cb, void* arg)
    : IEpollEvent(fd, EPOLL_TYPE_SOCKET, arg), mCb1(nullptr), mCb2(cb)
{
    int op = 1;
    if (setsockopt(mEvtFd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) < 0) {
        SPR_LOGE("setsockopt %d failed! (%s)\n", mEvtFd, strerror(errno));
        Close();
        SetReady(false);
    }
}

PUnixStreamClient::PUnixStreamClient(const std::function<void(int, void*)>& cb, void* arg)
    : IEpollEvent(-1, EPOLL_TYPE_SOCKET, arg), mCb1(cb), mCb2(nullptr)
{
    mEvtFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (mEvtFd < 0) {
        SPR_LOGE("socket failed! (%s)\n", strerror(errno));
        SetReady(false);
    }

    int op = 1;
    if (setsockopt(mEvtFd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) < 0) {
        SPR_LOGE("setsockopt %d failed! (%s)\n", mEvtFd, strerror(errno));
        Close();
        SetReady(false);
    }
}

PUnixStreamClient::PUnixStreamClient(const std::function<void(ssize_t, std::string, void*)>& cb, void* arg)
    : IEpollEvent(-1, EPOLL_TYPE_SOCKET, arg), mCb1(nullptr), mCb2(cb)
{
    mEvtFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (mEvtFd < 0) {
        SPR_LOGE("socket failed! (%s)\n", strerror(errno));
        SetReady(false);
    }

    int op = 1;
    if (setsockopt(mEvtFd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) < 0) {
        SPR_LOGE("setsockopt %d failed! (%s)\n", mEvtFd, strerror(errno));
        Close();
        SetReady(false);
    }
}

PUnixStreamClient::~PUnixStreamClient()
{
    Close();
}

int32_t PUnixStreamClient::AsUnixStreamClient(bool con, const std::string& srvPath, const std::string& cliPath)
{
    if (con) {
        int ret = 0;
        struct sockaddr_un mySelf;
        mySelf.sun_family = AF_UNIX;
        snprintf(mySelf.sun_path, 108, "%s", cliPath.c_str());  // 108 see <sys/un.h>

        unlink(mySelf.sun_path);
        ret = bind(mEvtFd, (struct sockaddr *)&mySelf, sizeof(mySelf));
        if (ret < 0) {
            Close();
            return -1;
        }

        struct sockaddr_un server;
        server.sun_family = AF_UNIX;
        snprintf(server.sun_path, 108, "%s", srvPath.c_str());  // 108 see <sys/un.h>
        if (connect(mEvtFd, (struct sockaddr *)&server, sizeof(server)) < 0) {
            SPR_LOGE("connect %s failed! (%s)\n", server.sun_path, strerror(errno));
            Close();
            return -1;
        }
    }

    int flags = fcntl(mEvtFd, F_GETFL, 0);
    fcntl(mEvtFd, F_SETFL, flags | O_NONBLOCK);
    AddToPoll();
    return 0;
}

void* PUnixStreamClient::EpollEvent(int fd, EpollType eType, void* arg)
{
    arg = arg ? arg : this;
    if (mCb1) {
        mCb1(fd, arg);
    }

    if (mCb2) {
        std::string bytes;
        ssize_t ret = IEpollEvent::Read(fd, bytes);
        mCb2(ret, bytes, arg);
    }

    return nullptr;
}
