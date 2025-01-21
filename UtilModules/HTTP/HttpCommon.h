/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HttpCommon.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/08/20
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/12/10 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __HTTP_COMMON_H__
#define __HTTP_COMMON_H__

enum EHttpStatusCode
{
    HTTP_STATUS_100 = 100,  // Continue
    HTTP_STATUS_101 = 101,  // Switching Protocols
    HTTP_STATUS_102 = 102,  // Processing (WebDAV)
    HTTP_STATUS_103 = 103,  // Early Hints

    HTTP_STATUS_200 = 200,  // OK
    HTTP_STATUS_201 = 201,  // Created
    HTTP_STATUS_202 = 202,  // Accepted
    HTTP_STATUS_203 = 203,  // Non-Authoritative Information
    HTTP_STATUS_204 = 204,  // No Content
    HTTP_STATUS_205 = 205,  // Reset Content
    HTTP_STATUS_206 = 206,  // Partial Content
    HTTP_STATUS_207 = 207,  // Multi-Status (WebDAV)
    HTTP_STATUS_208 = 208,  // Already Reported (WebDAV)
    HTTP_STATUS_226 = 226,  // IM Used

    HTTP_STATUS_300 = 300,  // Multiple Choices
    HTTP_STATUS_301 = 301,  // Moved Permanently
    HTTP_STATUS_302 = 302,  // Found
    HTTP_STATUS_303 = 303,  // See Other
    HTTP_STATUS_304 = 304,  // Not Modified
    HTTP_STATUS_305 = 305,  // Use Proxy (Deprecated)
    HTTP_STATUS_307 = 307,  // Temporary Redirect
    HTTP_STATUS_308 = 308,  // Permanent Redirect

    HTTP_STATUS_400 = 400,  // Bad Request
    HTTP_STATUS_401 = 401,  // Unauthorized
    HTTP_STATUS_402 = 402,  // Payment Required (Reserved for future use)
    HTTP_STATUS_403 = 403,  // Forbidden
    HTTP_STATUS_404 = 404,  // Not Found
    HTTP_STATUS_405 = 405,  // Method Not Allowed
    HTTP_STATUS_406 = 406,  // Not Acceptable
    HTTP_STATUS_407 = 407,  // Proxy Authentication Required
    HTTP_STATUS_408 = 408,  // Request Timeout
    HTTP_STATUS_409 = 409,  // Conflict
    HTTP_STATUS_410 = 410,  // Gone
    HTTP_STATUS_411 = 411,  // Length Required
    HTTP_STATUS_412 = 412,  // Precondition Failed
    HTTP_STATUS_413 = 413,  // Payload Too Large
    HTTP_STATUS_414 = 414,  // URI Too Long
    HTTP_STATUS_415 = 415,  // Unsupported Media Type
    HTTP_STATUS_416 = 416,  // Range Not Satisfiable
    HTTP_STATUS_417 = 417,  // Expectation Failed
    HTTP_STATUS_418 = 418,  // I'm a teapot (RFC 2324)
    HTTP_STATUS_421 = 421,  // Misdirected Request
    HTTP_STATUS_422 = 422,  // Unprocessable Entity (WebDAV)
    HTTP_STATUS_423 = 423,  // Locked (WebDAV)
    HTTP_STATUS_424 = 424,  // Failed Dependency (WebDAV)
    HTTP_STATUS_425 = 425,  // Too Early (RFC 8470)
    HTTP_STATUS_426 = 426,  // Upgrade Required
    HTTP_STATUS_428 = 428,  // Precondition Required
    HTTP_STATUS_429 = 429,  // Too Many Requests
    HTTP_STATUS_431 = 431,  // Request Header Fields Too Large
    HTTP_STATUS_451 = 451,  // Unavailable For Legal Reasons

    HTTP_STATUS_500 = 500,  // Internal Server Error
    HTTP_STATUS_501 = 501,  // Not Implemented
    HTTP_STATUS_502 = 502,  // Bad Gateway
    HTTP_STATUS_503 = 503,  // Service Unavailable
    HTTP_STATUS_504 = 504,  // Gateway Timeout
    HTTP_STATUS_505 = 505,  // HTTP Version Not Supported
    HTTP_STATUS_506 = 506,  // Variant Also Negotiates
    HTTP_STATUS_507 = 507,  // Insufficient Storage (WebDAV)
    HTTP_STATUS_508 = 508,  // Loop Detected (WebDAV)
    HTTP_STATUS_510 = 510,  // Not Extended
    HTTP_STATUS_511 = 511,  // Network Authentication Required
    HTTP_STATUS_BUTT = 999
};

#endif // __HTTP_COMMON_H__
