/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : LoginManager.h
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
#ifndef __LOGIN_MANAGER_H__
#define __LOGIN_MANAGER_H__

#include <list>
#include <memory>
#include <string>
#include <sys/types.h>
#include <PPipe.h>
#include <PSocket.h>

class LoginManager
{
public:
    LoginManager();
    ~LoginManager();
    static LoginManager* GetInstance();

    int Init();
    int BuildConnectAsTcpServer(short port);
    int ConnectLoop();

private:
    int Usage();
    int ExitShell();
    int ExitAll();
    int WriteStdin(const std::string& buf);
    int RegisterSignal();
    int ListenPipeEvent(int pipeFd);
    int ExecuteCmd(std::string& cmdBytes);
    // int Login(const char* username, const char* password);
    // int Logout();

private:
    bool mIsLogin;
    pid_t mCurPid;
    static pid_t mShellPid;
    int mStdin;
    int mStdout;
    int mStderr;
    int mInPipe[2];
    int mOutPipe[2];
    std::shared_ptr<PPipe> mPipePtr;
    std::shared_ptr<PSocket> mTcpSrvPtr;
    std::list<std::shared_ptr<PSocket>> mTcpClients;
};

#endif // __LOGIN_MANAGER_H__
