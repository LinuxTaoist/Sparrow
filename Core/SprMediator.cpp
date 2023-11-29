/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMediator.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2023/11/25
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *------------------------------------------------------------------------------
 *
 */
#include <string>
#include "SprMediator.h"

using namespace std;

const string MSG_QUEUE_PORT = "/SprMdrQ_20231126";          // mqueue
const string UNIX_DGRAM_PATH = "/tmp/SprMdrU_20231126";     // unix socket
const short INET_PORT = 1126;                               // inet socket

SprMediator::SprMediator()
{

}

SprMediator::~SprMediator()
{

}

