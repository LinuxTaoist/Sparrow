/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HttpMsg.h
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
#ifndef __HTTP_MSG_H__
#define __HTTP_MSG_H__

#include <map>
#include <string>
#include <stdint.h>

class HttpMsgBase
{
public:
    HttpMsgBase() = default;
    virtual ~HttpMsgBase() = default;

    void Trim(std::string& bytes);
    std::string GetStatusText(int32_t status) const;

    virtual int32_t Decode(const std::string& bytes) = 0;
    virtual int32_t Encode(std::string& bytes) = 0;
};

class HttpMsgRequest : public HttpMsgBase
{
public:
    HttpMsgRequest(const std::string& bytes);
    HttpMsgRequest(const std::string& method, const std::string& uri, const std::string& version);
    virtual ~HttpMsgRequest();

    int32_t SetMethod(const std::string& method);
    int32_t SetURI(const std::string& uri);
    int32_t SetHttpVersion(const std::string& version);
    int32_t SetHeader(const std::string& key, const std::string& value);
    int32_t SetMsgHeaders(const std::map<std::string, std::string>& headers);
    int32_t SetMsgBody(const std::string& body);

    std::string GetMethod() const;
    std::string GetURI() const;
    std::string GetHttpVersion() const;
    std::string GetMsgHeader(const std::string& key) const;
    std::map<std::string, std::string> GetMsgHeaders() const;
    std::string GetMsgBody() const;

    virtual int32_t Decode(const std::string& bytes) override;
    virtual int32_t Encode(std::string& bytes) override;

private:
    std::string mReqMethod;
    std::string mReqURI;
    std::string mHttpVersion;
    std::map<std::string, std::string> mMsgHeaders;
    std::string mMsgBody;
};

#endif // __HTTP_MSG_H__
