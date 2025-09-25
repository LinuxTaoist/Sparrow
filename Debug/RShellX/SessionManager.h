/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SessionManager.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/10/13
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/10/13 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __SESSION_MANAGER_H__
#define __SESSION_MANAGER_H__

#include <list>
#include <memory>
#include <string>
#include <PSocket.h>

class SessionManager
{
public:
    static SessionManager* GetInstance();

    int32_t Init(uint16_t port);
    int32_t EpollLoop();

private:
    SessionManager();
    ~SessionManager();

private:
    std::shared_ptr<PTcpServer> mpTcpSrv;
    std::list<std::shared_ptr<PTcpClient>> mTcpClients;
};

#endif // __SESSION_MANAGER_H__
