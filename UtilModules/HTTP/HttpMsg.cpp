/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HttpMsg.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/12/10
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/12/10 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "HttpCommon.h"
#include "HttpMsg.h"

//---------------------------------------------------------------------------------------------------------------------
// PTcpClient: TCP client
//---------------------------------------------------------------------------------------------------------------------

std::string HttpMsgBase::GetStatusText(int32_t status) const
{

    switch(status)
    {
        case HTTP_STATUS_100: return "Continue";
        case HTTP_STATUS_101: return "Switching Protocols";
        case HTTP_STATUS_102: return "Processing";
        case HTTP_STATUS_103: return "Early Hints";
        case HTTP_STATUS_200: return "OK";
        case HTTP_STATUS_201: return "Created";
        case HTTP_STATUS_202: return "Accepted";
        case HTTP_STATUS_203: return "Non-Authoritative Information";
        case HTTP_STATUS_204: return "No Content";
        case HTTP_STATUS_205: return "Reset Content";

    }
}

