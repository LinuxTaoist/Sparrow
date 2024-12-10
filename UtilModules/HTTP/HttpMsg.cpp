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
#include <sstream>
#include <algorithm>
#include "HttpCommon.h"
#include "HttpMsg.h"

//---------------------------------------------------------------------------------------------------------------------
// HttpMsgBase
//---------------------------------------------------------------------------------------------------------------------
void HttpMsgBase::Trim(std::string& bytes)
{
    bytes.erase(bytes.begin(), std::find_if(bytes.begin(), bytes.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));

    bytes.erase(std::find_if(bytes.rbegin(), bytes.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), bytes.end());
}

std::string HttpMsgBase::GetStatusText(int32_t status) const
{
    switch (status) {
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
        case HTTP_STATUS_206: return "Partial Content";
        case HTTP_STATUS_207: return "Multi-Status";
        case HTTP_STATUS_208: return "Already Reported";
        case HTTP_STATUS_226: return "IM Used";
        case HTTP_STATUS_300: return "Multiple Choices";
        case HTTP_STATUS_301: return "Moved Permanently";
        case HTTP_STATUS_302: return "Found";
        case HTTP_STATUS_303: return "See Other";
        case HTTP_STATUS_304: return "Not Modified";
        case HTTP_STATUS_305: return "Use Proxy";
        case HTTP_STATUS_307: return "Temporary Redirect";
        case HTTP_STATUS_308: return "Permanent Redirect";
        case HTTP_STATUS_400: return "Bad Request";
        case HTTP_STATUS_401: return "Unauthorized";
        case HTTP_STATUS_402: return "Payment Required";
        case HTTP_STATUS_403: return "Forbidden";
        case HTTP_STATUS_404: return "Not Found";
        case HTTP_STATUS_405: return "Method Not Allowed";
        case HTTP_STATUS_406: return "Not Acceptable";
        case HTTP_STATUS_407: return "Proxy Authentication Required";
        case HTTP_STATUS_408: return "Request Timeout";
        case HTTP_STATUS_409: return "Conflict";
        case HTTP_STATUS_410: return "Gone";
        case HTTP_STATUS_411: return "Length Required";
        case HTTP_STATUS_412: return "Precondition Failed";
        case HTTP_STATUS_413: return "Payload Too Large";
        case HTTP_STATUS_414: return "URI Too Long";
        case HTTP_STATUS_415: return "Unsupported Media Type";
        case HTTP_STATUS_416: return "Range Not Satisfiable";
        case HTTP_STATUS_417: return "Expectation Failed";
        case HTTP_STATUS_418: return "I'm a teapot";
        case HTTP_STATUS_421: return "Misdirected Request";
        case HTTP_STATUS_422: return "Unprocessable Entity";
        case HTTP_STATUS_423: return "Locked";
        case HTTP_STATUS_424: return "Failed Dependency";
        case HTTP_STATUS_425: return "Too Early";
        case HTTP_STATUS_426: return "Upgrade Required";
        case HTTP_STATUS_428: return "Precondition Required";
        case HTTP_STATUS_429: return "Too Many Requests";
        case HTTP_STATUS_431: return "Request Header Fields Too Large";
        case HTTP_STATUS_451: return "Unavailable For Legal Reasons";
        case HTTP_STATUS_500: return "Internal Server Error";
        case HTTP_STATUS_501: return "Not Implemented";
        case HTTP_STATUS_502: return "Bad Gateway";
        case HTTP_STATUS_503: return "Service Unavailable";
        case HTTP_STATUS_504: return "Gateway Timeout";
        case HTTP_STATUS_505: return "HTTP Version Not Supported";
        case HTTP_STATUS_506: return "Variant Also Negotiates";
        case HTTP_STATUS_507: return "Insufficient Storage";
        case HTTP_STATUS_508: return "Loop Detected";
        case HTTP_STATUS_510: return "Not Extended";
        case HTTP_STATUS_511: return "Network Authentication Required";
        default: return "Internal Server Error";
    }
}

//---------------------------------------------------------------------------------------------------------------------
// HttpMsgRequest
//---------------------------------------------------------------------------------------------------------------------
HttpMsgRequest::HttpMsgRequest(const std::string& bytes)
{
    Decode(bytes);
}

HttpMsgRequest::HttpMsgRequest(const std::string& method, const std::string& uri, const std::string& version)
    : mReqMethod(method), mReqURI(uri), mHttpVersion(version)
{
}

HttpMsgRequest::~HttpMsgRequest()
{
}

int32_t HttpMsgRequest::SetMethod(const std::string& method)
{
    mReqMethod = method;
    return 0;
}

int32_t HttpMsgRequest::SetURI(const std::string& uri)
{
    mReqURI = uri;
    return 0;
}

int32_t HttpMsgRequest::SetHttpVersion(const std::string& version)
{
    mHttpVersion = version;
    return 0;
}

int32_t HttpMsgRequest::SetHeader(const std::string& key, const std::string& value)
{
    mMsgHeaders[key] = value;
    return 0;
}

int32_t HttpMsgRequest::SetMsgHeaders(const std::map<std::string, std::string>& headers)
{
    mMsgHeaders = headers;
    return 0;
}

int32_t HttpMsgRequest::SetMsgBody(const std::string& body)
{
    mMsgBody = body;
    return 0;
}

std::string HttpMsgRequest::GetMethod() const
{
    return mReqMethod;
}

std::string HttpMsgRequest::GetURI() const
{
    return mReqURI;
}

std::string HttpMsgRequest::GetHttpVersion() const
{
    return mHttpVersion;
}

std::string HttpMsgRequest::GetMsgHeader(const std::string& key) const
{
    auto it = mMsgHeaders.find(key);
    if (it != mMsgHeaders.end())
    {
        return it->second;
    }
    return "";
}

std::map<std::string, std::string> HttpMsgRequest::GetMsgHeaders() const
{
    return mMsgHeaders;
}

std::string HttpMsgRequest::GetMsgBody() const
{
    return mMsgBody;
}

int32_t HttpMsgRequest::Decode(const std::string& bytes)
{
    std::istringstream stream(bytes);
    std::string line;

    if (!std::getline(stream, line)) {
        return -1;
    }

    std::istringstream iss(line);
    if (!(iss >> mReqMethod >> mReqURI >> mHttpVersion)) {
        return -1;
    }

    Trim(mHttpVersion);
    if (mHttpVersion != "HTTP/1.1" && mHttpVersion != "HTTP/1.0") {
        return -1;
    }

    while (std::getline(stream, line) && !line.empty() && line.find_first_not_of(" \t\r\n") != std::string::npos) {
        size_t colPos = line.find(':');
        if (colPos == std::string::npos || colPos == 0) {
            return -1;
        }

        std::string key = line.substr(0, colPos);
        std::string value = line.substr(colPos + 1);
        Trim(value);
        mMsgHeaders[key] = value;
    }

    if (!line.empty() && line.find_first_not_of(" \t\r\n") != std::string::npos) {
        return -1;
    }

    if (stream.peek() != EOF) {
        std::ostringstream bodyStream;
        bodyStream << stream.rdbuf();
        mMsgBody = bodyStream.str();
    }

    return 0;
}

int32_t HttpMsgRequest::Encode(std::string& bytes)
{
    // Request        = Request-Line                          ; Section 5.1
    //                  *( ( general-header                   ; Section 4.5
    //                     | request-header                   ; Section 5.3
    //                     | entity-header ) CRLF )           ; Section 7.1
    //                  CRLF
    //                  [ message-body ]                      ; Section 4.3
    bytes += mReqMethod + " " + mReqURI + " " + mHttpVersion + "\r\n";

    for (const auto& pair : mMsgHeaders) {
        bytes += pair.first + ": " + pair.second + "\r\n";
    }

    bytes += "\r\n";
    bytes += mMsgBody;
    return 0;
}
