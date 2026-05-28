/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : HTTP UI Server entry point
 *  @date       : 2026/05/28
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/05/28 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <stdio.h>
#include "HUIServer.h"

int main(int argc, char* argv[])
{
    int port = HUI_DEFAULT_PORT;
    if (argc > 1) {
        port = atoi(argv[1]);
    }

    HUIServer server;
    return server.Start(port);
}
