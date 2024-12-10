/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SampleHttp.cc
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
#include "HttpProtocol.h"

#define SPR_LOGD(fmt, args...) printf("%4d SampleHttp D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%4d SampleHttp E: " fmt, __LINE__, ##args)

void TestRequestBytes()
{
    std::string bytes =
    "GET /favicon.ico HTTP/1.1\r\n"
    "Host: 127.0.0.1:8080\r\n"
    "Connection: keep-alive\r\n"
    "sec-ch-ua-platform: \"Windows\"\r\n"
    "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36 Edg/131.0.0.0\r\n"
    "sec-ch-ua: \"Microsoft Edge\";v=\"131\", \"Chromium\";v=\"131\", \"Not_A Brand\";v=\"24\"\r\n"
    "sec-ch-ua-mobile: ?0\r\n"
    "Accept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8\r\n"
    "Sec-Fetch-Site: same-origin\r\n"
    "Sec-Fetch-Mode: no-cors\r\n"
    "Sec-Fetch-Dest: image\r\n"
    "Referer: http://127.0.0.1:8080/\r\n"
    "Accept-Encoding: gzip, deflate, br, zstd\r\n"
    "Accept-Language: zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6\r\n"
    "\r\n";

    HttpMsgRequest theRequest(bytes);
    SPR_LOGD("method : %s\n", theRequest.GetMethod().c_str());
    SPR_LOGD("uri    : %s\n", theRequest.GetURI().c_str());
    SPR_LOGD("version: %s\n", theRequest.GetHttpVersion().c_str());
    SPR_LOGD("headers:\n");

    std::map<std::string, std::string>  headers = theRequest.GetMsgHeaders();
    for (auto& header : headers) {
        SPR_LOGD("        %s: %s\n", header.first.c_str(), header.second.c_str());
    }

    SPR_LOGD("body   : %s\n", theRequest.GetMsgBody().c_str());

    std::string enBytes;
    theRequest.Encode(enBytes);
    SPR_LOGD("encode : %s\n", enBytes.c_str());
}

int main(int argc, char *argv[])
{
    TestRequestBytes();
    return 0;
}

