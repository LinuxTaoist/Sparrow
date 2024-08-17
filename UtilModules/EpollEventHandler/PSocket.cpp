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
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include "PSocket.h"

#define SPR_LOGD(fmt, args...) printf("%4d PSocket D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%4d PSocket E: " fmt, __LINE__, ##args)

PSocket::PSocket(int domain, int type, int protocol, std::function<void(int, void*)> cb, void* arg)
    : IEpollEvent(-1, EPOLL_TYPE_SOCKET, arg), mCb(cb)
{
    mEnable = true;
    mEpollFd = socket(domain, type, protocol);
    if (mEpollFd == -1)
    {
        mEnable = false;
        SPR_LOGE("socket failed! (%s)\n", strerror(errno));
    }

    int op = 1;
    if (setsockopt(mEpollFd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) < 0) {
        SPR_LOGE("setsockopt failed! (%s)\n", strerror(errno));
        close(mEpollFd);
        mEpollFd = -1;
        mEnable = false;
    }

    mSockType = PSOCKET_TYPE_IDLE;
}

PSocket::PSocket(int sock, std::function<void(int, void*)> cb, void* arg)
    : IEpollEvent(sock, EPOLL_TYPE_SOCKET, arg), mCb(cb)
{
    mEnable = true;
    mSockType = PSOCKET_TYPE_IDLE;

    int op = 1;
    if (setsockopt(mEpollFd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) < 0) {
        SPR_LOGE("setsockopt %d failed! (%s)\n", mEpollFd, strerror(errno));
        close(mEpollFd);
        mEnable = false;
    }
}

PSocket::~PSocket()
{
    Close();
}

void PSocket::Close()
{
    if (mEpollFd > 0) {
        SPR_LOGD("Close socket [%d %d]\n", mEpollFd, mSockType);
        close(mEpollFd);
        mEpollFd = -1;
        mEnable = false;
    }
}

int PSocket::AsTcpServer(short bindPort, int backlog)
{
    if (!mEnable || mSockType != PSOCKET_TYPE_IDLE) {
        SPR_LOGE("Not enable! mEnable = %d, mSockType = %d\n", mEnable, mSockType);
        return -1;
    }

    struct sockaddr_in myAddr;
    bzero(&myAddr, sizeof(myAddr));
    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myAddr.sin_port = htons(bindPort);
    if (bind(mEpollFd, (struct sockaddr *)&myAddr, sizeof(myAddr)) < 0) {
        SPR_LOGE("bind failed! (%s)\n", strerror(errno));
        goto ERROR;
    }

    if (listen(mEpollFd, backlog) < 0) {
        SPR_LOGE("bind failed! (%s)\n", strerror(errno));
        goto ERROR;
    }

    mSockType = PSOCKET_TYPE_TCP_SERVER;
    return 0;

ERROR:
    close(mEpollFd);
    return -1;
}

int PSocket::AsTcpClient(bool con, const std::string& srvAddr, short srvPort, int rcvLen, int sndLen)
{
    if (!mEnable || mSockType != PSOCKET_TYPE_IDLE) {
        SPR_LOGE("Not enable! mEnable = %d, mSockType = %d\n", mEnable, mSockType);
        return -1;
    }

    int flags, op;
    struct linger so_linger;
    struct sockaddr_in server;

    op = rcvLen;
    if (setsockopt(mEpollFd, SOL_SOCKET, SO_RCVBUF, &op, sizeof(op)) < 0) {
        SPR_LOGE("setsockopt %d failed! (%s)\n", mEpollFd, strerror(errno));
        goto ERROR;
    }

    op = sndLen;
    if (setsockopt(mEpollFd, SOL_SOCKET, SO_SNDBUF, &op, sizeof(op)) < 0) {
        SPR_LOGE("setsockopt failed! (%s)\n", strerror(errno));
        goto ERROR;
    }

    so_linger.l_onoff = 1; /* when send to sock and it close, keep time */
    so_linger.l_linger = 0; /* keep time second */
    if (setsockopt(mEpollFd, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger)) < 0) {
        SPR_LOGE("setsockopt failed! (%s)\n", strerror(errno));
        goto ERROR;
    }

    op = 1; // disable Nagle, TCP_NODELAY is the only used IPPROTO_TCP layer.
    if (setsockopt(mEpollFd, IPPROTO_TCP, 1, &op, sizeof(op)) < 0) {
        SPR_LOGE("setsockopt failed! (%s)\n", strerror(errno));
        goto ERROR;
    }

    if (con) {
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr(srvAddr.c_str());
        server.sin_port = htons(srvPort);
        // Linux TCP repeat connect directly return EISCONN.
        if (connect(mEpollFd, (struct sockaddr*)&server, sizeof(server)) < 0 && errno != EISCONN) {
            SPR_LOGE("connect failed! (%s)\n", strerror(errno));
            goto ERROR;
        }
    }

    flags = fcntl(mEpollFd, F_GETFL, 0);
    fcntl(mEpollFd, F_SETFL, flags | O_NONBLOCK);

    mSockType = PSOCKET_TYPE_TCP_CLIENT;
    return 0;

ERROR:
    close(mEpollFd);
    return -1;
}

int PSocket::AsUdpServer(short bindPort, int rcvLen)
{
    if (!mEnable || mSockType != PSOCKET_TYPE_IDLE) {
        SPR_LOGE("Not enable! mEnable = %d, mSockType = %d\n", mEnable, mSockType);
        return -1;
    }

    int flags = fcntl(mEpollFd, F_GETFL, 0);
    fcntl(mEpollFd, F_SETFL, flags | O_NONBLOCK);

    int op = rcvLen;
    if (setsockopt(mEpollFd, SOL_SOCKET, SO_RCVBUF, &op, sizeof(op)) < 0) {
        SPR_LOGE("setsockopt failed! (%s)\n", strerror(errno));
        goto ERROR;
    }

    struct sockaddr_in myAddr;
    bzero(&myAddr, sizeof(myAddr));
    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myAddr.sin_port = htons(bindPort);
    if (bind(mEpollFd, (struct sockaddr *)&myAddr, sizeof(myAddr)) < 0) {
        SPR_LOGE("bind failed! (%s)\n", strerror(errno));
        goto ERROR;
    }

    mSockType = PSOCKET_TYPE_UDP_SERVER;
    return 0;

ERROR:
    close(mEpollFd);
    mEpollFd = -1;
    return -1;
}

int PSocket::AsUdpClient(const std::string& srvAddr, short srvPort, int sndLen)
{
    if (!mEnable || mSockType != PSOCKET_TYPE_IDLE) {
        SPR_LOGE("Not enable! mEnable = %d, mSockType = %d\n", mEnable, mSockType);
        return -1;
    }

    int flags, op;
    struct linger so_linger;

    flags = fcntl(mEpollFd, F_GETFL, 0);
    fcntl(mEpollFd, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(srvAddr.c_str());
    server.sin_port = htons(srvPort);
    // Linux UDP repeat connect directly return 0.
    // UDP not really connect to dst. Only just send.
    if (connect(mEpollFd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        SPR_LOGE("connect failed! (%s)\n", strerror(errno));
        goto ERROR;
    }

    op = sndLen;
    if (setsockopt(mEpollFd, SOL_SOCKET, SO_SNDBUF, &op, sizeof(op)) < 0) {
        SPR_LOGE("setsockopt failed! (%s)\n", strerror(errno));
        goto ERROR;
    }

    so_linger.l_onoff  = 1;     /* when send to sock and it close, keep time */
    so_linger.l_linger = 0;     /* keep time second */
    if (setsockopt(mEpollFd, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger)) < 0) {
        SPR_LOGE("setsockopt failed! (%s)\n", strerror(errno));
        goto ERROR;
    }

    mSockType = PSOCKET_TYPE_UDP_CLIENT;
    return 0;

ERROR:
    close(mEpollFd);
    mEpollFd = -1;
    return -1;
}

int PSocket::AsUnixStreamServer(const std::string& serverName, int backlog)
{
    if (!mEnable || mSockType != PSOCKET_TYPE_IDLE) {
        SPR_LOGE("Not enable! mEnable = %d, mSockType = %d\n", mEnable, mSockType);
        return -1;
    }

    struct sockaddr_un myAddr;
    myAddr.sun_family = AF_UNIX;
    snprintf(myAddr.sun_path, 108, "/tmp/%s", serverName.c_str());  // 108 see <sys/un.h>
    if (bind(mEpollFd, (struct sockaddr *)&myAddr, sizeof(myAddr)) < 0) {
        if (connect(mEpollFd, (struct sockaddr *)&myAddr, sizeof(myAddr)) == 0) {
            SPR_LOGE("connect failed! (%s)\n", strerror(errno));
            return 0;
        }

        if (unlink(myAddr.sun_path) < 0) {
            SPR_LOGE("unlink %s failed! (%s)\n", myAddr.sun_path, strerror(errno));
            goto ERROR;
        }

        if (bind(mEpollFd, (struct sockaddr *)&myAddr, sizeof(myAddr)) < 0) {
            SPR_LOGE("bind failed! (%s)\n", strerror(errno));
            goto ERROR;
        }
    }

    if (listen(mEpollFd, backlog) < 0) {
        SPR_LOGE("listen failed! (%s)\n", strerror(errno));
        goto ERROR;
    }

    mSockType = PSOCKET_TYPE_UN_STREAM_SERVER;
    return 0;

ERROR:
    close(mEpollFd);
    mEpollFd = -1;
    return -1;
}

int PSocket::AsUnixStreamClient(bool con, const std::string& serverName, const std::string& clientName)
{
    if (!mEnable || mSockType != PSOCKET_TYPE_IDLE) {
        SPR_LOGE("Not enable! mEnable = %d, mSockType = %d\n", mEnable, mSockType);
        return -1;
    }

    int flags;
    if (con) {
        struct sockaddr_un mySelf;
        mySelf.sun_family = AF_UNIX;
        snprintf(mySelf.sun_path, 108, "/tmp/%s", clientName.c_str());  // 108 see <sys/un.h>
        if (bind(mEpollFd, (struct sockaddr *)&mySelf, sizeof(mySelf)) < 0) {
            if (connect(mEpollFd, (struct sockaddr *)&mySelf, sizeof(mySelf)) == 0) {
                SPR_LOGD("unix_sock:%s already connected\n", mySelf.sun_path);
                return 0;
            }

            if (unlink(mySelf.sun_path) < 0) {
                SPR_LOGE("unlink %s failed! (%s)\n", mySelf.sun_path, strerror(errno));
                goto ERROR;
            }

            if (bind(mEpollFd, (struct sockaddr *)&mySelf, sizeof(mySelf)) < 0) {
                SPR_LOGE("bind failed! (%s)\n", strerror(errno));
                goto ERROR;
            }
        }

        struct sockaddr_un server;
        server.sun_family = AF_UNIX;
        snprintf(server.sun_path, 108, "/tmp/%s", serverName.c_str());  // 108 see <sys/un.h>
        if (connect(mEpollFd, (struct sockaddr*)&server, sizeof(server)) < 0 && errno != EISCONN) {
            SPR_LOGE("connect failed! (%s)\n", strerror(errno));
            goto ERROR;
        }
    }

    flags = fcntl(mEpollFd, F_GETFL, 0);
    fcntl(mEpollFd, F_SETFL, flags | O_NONBLOCK);

    mSockType = PSOCKET_TYPE_UN_STREAM_CLIENT;
    return 0;

ERROR:
    close(mEpollFd);
    mEpollFd = -1;
    return -1;
}

int PSocket::AsUnixDgramServer(const std::string& serverName)
{
    if (!mEnable || mSockType != PSOCKET_TYPE_IDLE) {
        SPR_LOGE("Not enable! mEnable = %d, mSockType = %d\n", mEnable, mSockType);
        return -1;
    }

    int flags = fcntl(mEpollFd, F_GETFL, 0);
    fcntl(mEpollFd, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_un mySelf;
    mySelf.sun_family = AF_UNIX;
    snprintf(mySelf.sun_path, 108, "/tmp/%s", serverName.c_str());  // 108 see <sys/un.h>
    if (bind(mEpollFd, (struct sockaddr *)&mySelf, sizeof(mySelf)) < 0) {
        if (connect(mEpollFd, (struct sockaddr *)&mySelf, sizeof(mySelf)) == 0) {
            SPR_LOGD("unix_sock:%s already connected\n", mySelf.sun_path);
            return 0;
        }

        if (unlink(mySelf.sun_path) < 0) {
            SPR_LOGE("unlink %s failed! (%s)\n", mySelf.sun_path, strerror(errno));
            goto ERROR;
        }

        if (bind(mEpollFd, (struct sockaddr *)&mySelf, sizeof(mySelf)) < 0) {
            SPR_LOGE("listen failed! (%s)\n", strerror(errno));
            goto ERROR;
        }
    }

    mSockType = PSOCKET_TYPE_UN_DGRAM_SERVER;
    return 0;

ERROR:
    close(mEpollFd);
    mEpollFd = -1;
    return -1;
}

int PSocket::AsUnixDgramClient(const std::string& serverName)
{
    if (!mEnable || mSockType != PSOCKET_TYPE_IDLE) {
        SPR_LOGE("Not enable! mEnable = %d, mSockType = %d\n", mEnable, mSockType);
        return -1;
    }

    int flags = fcntl(mEpollFd, F_GETFL, 0);
    fcntl(mEpollFd, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_un mySelf;
    mySelf.sun_family = AF_UNIX;
    snprintf(mySelf.sun_path, 108, "/tmp/%s", serverName.c_str());  // 108 see <sys/un.h>
    if (connect(mEpollFd, (struct sockaddr *)&mySelf, sizeof(mySelf)) < 0) {
        SPR_LOGE("connect %s failed! (%s)\n", mySelf.sun_path, strerror(errno));
        goto ERROR;
    }

    mSockType = PSOCKET_TYPE_UN_DGRAM_CLIENT;
    return 0;

ERROR:
    close(mEpollFd);
    mEpollFd = -1;
    return -1;
}

void* PSocket::EpollEvent(int fd, EpollType eType, void* arg)
{
    switch (mSockType) {
        case PSOCKET_TYPE_TCP_SERVER:
        {
            struct sockaddr_in client;
            socklen_t len = (socklen_t)sizeof(client);
            int sock = accept(fd, (struct sockaddr *)&client, &len);
            if (sock < 0) {
                SPR_LOGE("accept failed! (%s)\n", strerror(errno));
                return nullptr;
            }

            fd = sock;
            break;
        }

        case PSOCKET_TYPE_UN_STREAM_SERVER:
        {
            struct sockaddr_un client;
            socklen_t len = (socklen_t)sizeof(client);
            int sock = accept(fd, (struct sockaddr *)&client, &len);
            if (sock < 0) {
                SPR_LOGE("accept failed! (%s)\n", strerror(errno));
                return nullptr;
            }

            fd = sock;
            break;
        }

        case PSOCKET_TYPE_TCP_CLIENT:
        case PSOCKET_TYPE_UDP_SERVER:
        case PSOCKET_TYPE_UDP_CLIENT:
        case PSOCKET_TYPE_UN_STREAM_CLIENT:
        case PSOCKET_TYPE_UN_DGRAM_SERVER:
        case PSOCKET_TYPE_UN_DGRAM_CLIENT:
        {
            break;
        }

        case PSOCKET_TYPE_IDLE:
        default:
            SPR_LOGD("No this option %d! \n", mSockType);
            return nullptr;
    }

    if (mCb) {
        arg = arg ? arg : this;
        mCb(fd, arg);
    }

    return nullptr;
}
