/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : IHwUart.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/07/27
 *
 *  UART HAL interface.
 *
 *  Typical call sequence:
 *    1. Open(port, &config)                 — open and configure
 *    2. RegisterRecvCallback()               — set async receive callback
 *    3. Send(data, len)                     — transmit
 *    4. recvHandler fires on incoming data  — receive
 *    5. Flush() / Close()                   — tear down
 *
 *  Design note:
 *    Each instance represents ONE physical UART port.  For multi-port
 *    scenarios, create multiple instances.
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/07/27 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __IHW_UART_H__
#define __IHW_UART_H__

#include <cstdint>
#include <string>
#include "HwCommonTypes.h"

using HwUartRecvCb = std::function<void(const uint8_t* data, int32_t len)>;

class IHwUart {
public:
    virtual ~IHwUart() = default;

    virtual std::string GetVersion() const = 0;

    /**
     * @brief Open a UART port with the given configuration.
     *
     * @param  port    platform-specific port identifier (e.g. "/dev/ttyHS0", "UART1")
     * @param  config  port parameters; if nullptr, defaults are used
     * @return HW_OK on success, HW_ERR_UART_OPEN_FAILED / HW_ERR_UART_CONFIG_FAILED
     */
    virtual int32_t Open(const std::string& port, const SHwUartConfig& config) = 0;

    /**
     * @brief Close the port and release resources.
     * @return HW_OK on success
     */
    virtual int32_t Close() = 0;

    /**
     * @brief Send a block of data.
     *
     * The implementation may buffer and return immediately.
     * The caller retains ownership of data.
     *
     * @param  data   pointer to send buffer
     * @param  len    number of bytes to send
     * @return number of bytes actually queued, or negative error code
     */
    virtual int32_t Send(const uint8_t* data, int32_t len) = 0;

    /**
     * @brief Register a callback for asynchronous received data.
     *
     * The callback fires from an internal read thread / epoll context;
     * heavy work MUST be deferred.
     *
     * @param  handler   receive callback object
     * @return HW_OK on success
     */
    virtual int32_t RegisterRecvCallback(HwUartRecvCb cb) = 0;

    /**
     * @brief Discard all buffered data (both TX and RX).
     * @return HW_OK on success
     */
    virtual int32_t Flush() = 0;

    /**
     * @brief Return true if the port is currently open and operational.
     */
    virtual bool IsOpen() const = 0;
};

#endif // __IHW_UART_H__
