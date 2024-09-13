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
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __GENERAL_SOCKET_H__
#define __GENERAL_SOCKET_H__

#include <string>
#include <functional>
#include "IEpollEvent.h"

enum PSocketType
{
    PSOCKET_TYPE_IDLE,
    PSOCKET_TYPE_TCP_SERVER,
    PSOCKET_TYPE_TCP_CLIENT,
    PSOCKET_TYPE_UDP_SERVER,
    PSOCKET_TYPE_UDP_CLIENT,
    PSOCKET_TYPE_UN_STREAM_SERVER,
    PSOCKET_TYPE_UN_STREAM_CLIENT,
    PSOCKET_TYPE_UN_DGRAM_SERVER,
    PSOCKET_TYPE_UN_DGRAM_CLIENT
};

class PSocket : public IEpollEvent
{
public:
    PSocket(int domain, int type, int protocol,
               const std::function<void(int, void*)>& cb, void* arg = nullptr);

    PSocket(int sock,
               const std::function<void(int, void*)>& cb, void* arg = nullptr);

    virtual ~PSocket();

    void Close();
    int AsTcpServer(short bindPort, int backlog);
    int AsTcpClient(bool con = false,
                    const std::string& srvAddr = "",
                    short srvPort = 0,
                    int rcvLen = 512 * 1024,
                    int sndLen = 512 * 1024);

    int AsUdpServer(short bindPort, int rcvLen = 512 * 1024);
    int AsUdpClient(const std::string& srvAddr, short srvPort, int sndLen = 512 * 1024);

    int AsUnixStreamServer(const std::string& serverName, int backlog);
    int AsUnixStreamClient(bool con = false,
                           const std::string& serverName = "",
                           const std::string& clientName = "");

    int AsUnixDgramServer(const std::string& serverName);
    int AsUnixDgramClient(const std::string& serverName);

    virtual void*   EpollEvent(int fd, EpollType eType, void* arg) override;

private:
    bool            mEnable;
    PSocketType     mSockType;
    std::function<void(int, void*)> mCb;
};

#endif
