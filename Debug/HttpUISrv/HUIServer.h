/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HUIServer.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : HTTP User Interface Server for project status display and remote debugging
 *  @date       : 2026/05/28
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/05/28 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __HUI_SERVER_H__
#define __HUI_SERVER_H__

#include <string>
#include <vector>
#include <memory>

#define HUI_DEFAULT_PORT                (8888)
#define HUI_SERVER_BACKLOG              (5)

class HUIServer
{
public:
    HUIServer();
    ~HUIServer();

    /**
     * @brief Start HTTP UI server on specified port
     * @param port Port to listen on (default HUI_DEFAULT_PORT)
     * @return 0 if success, -1 if failed
     */
    int Start(int port = HUI_DEFAULT_PORT);

    /**
     * @brief Stop the server
     */
    void Stop();

private:
    int  mPort;
    bool mRunning;
};

#endif // __HUI_SERVER_H__
