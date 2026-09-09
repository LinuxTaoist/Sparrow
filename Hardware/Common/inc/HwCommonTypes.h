/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HwCommonTypes.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/07/27
 *
 *  Common types, enums, error codes and callbacks shared across all
 *  Hardware Abstraction Layer interfaces.
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/07/27 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __HW_COMMON_TYPES_H__
#define __HW_COMMON_TYPES_H__

#include <cstdint>
#include <functional>
#include <string>

#define HW_COMMON_TYPES_VERSION   "HW_COMMON_TYPES_R1002"

enum EHwErrorCode {
    HW_OK                           =  0,
    HW_ERR_GENERAL                  = -1,
    HW_ERR_NOT_IMPL                 = -2,
    HW_ERR_NOT_INITIALIZED          = -3,
    HW_ERR_ALREADY_INITIALIZED      = -4,

    HW_ERR_POWER_INVALID_MODE       = -10,
    HW_ERR_POWER_TRANSITION_DENIED  = -11,

    HW_ERR_GPIO_INVALID_PIN         = -60,
    HW_ERR_GPIO_EXPORT_FAILED       = -61,
    HW_ERR_GPIO_DIRECTION_FAILED    = -62,
    HW_ERR_GPIO_READ_FAILED         = -63,
    HW_ERR_GPIO_WRITE_FAILED        = -64,
    HW_ERR_GPIO_IRQ_FAILED          = -65,

    HW_ERR_UART_INVALID_PORT        = -110,
    HW_ERR_UART_OPEN_FAILED         = -111,
    HW_ERR_UART_CONFIG_FAILED       = -112,
    HW_ERR_UART_SEND_FAILED         = -113,
    HW_ERR_UART_RECV_FAILED         = -114,
    HW_ERR_UART_RECV_TIMEOUT        = -115,
    HW_ERR_UART_FRAME_ERROR         = -116,
    HW_ERR_UART_PARITY_ERROR        = -117,
    HW_ERR_UART_OVERRUN             = -118,

    HW_ERR_WDT_OPEN_FAILED          = -160,
    HW_ERR_WDT_FEED_FAILED          = -161,

    HW_ERR_RTC_READ_FAILED          = -210,
    HW_ERR_RTC_SET_FAILED           = -211,

    HW_ERR_NET_NO_SERVICE           = -260,
    HW_ERR_NET_REGISTRATION_DENIED  = -261,
    HW_ERR_NET_QUERY_FAILED         = -262,
    HW_ERR_NET_INTERFACE_DOWN       = -263,

    HW_ERR_SIM_QUERY_FAILED         = -310,
    HW_ERR_SIM_NOT_PRESENT          = -311,
};

enum EHwPowerMode {
    HW_POWER_ACTIVE   = 0,
    HW_POWER_STANDBY  = 1,
    HW_POWER_SLEEP    = 2,
    HW_POWER_SHUTDOWN = 3,
};

enum EHwLpmEdge {
    HW_LPM_EDGE_FALLING                  = 0,
    HW_LPM_EDGE_RISING                   = 1,
    HW_LPM_EDGE_TTY_WAKEUP               = 2,
    HW_LPM_EDGE_CUSTOMIZED_FIRST_FALLING = 3,
    HW_LPM_EDGE_CUSTOMIZED_FIRST_RISING  = 4,
    HW_LPM_EDGE_CUSTOMIZED_SEC_FALLING   = 5,
    HW_LPM_EDGE_CUSTOMIZED_SEC_RISING    = 6,
};

enum EHwWakeupSource {
    HW_WAKEUP_UNKNOWN  = 0,
    HW_WAKEUP_IGNITION = 1,
    HW_WAKEUP_CAN      = 2,
    HW_WAKEUP_RTC      = 3,
    HW_WAKEUP_SMS      = 4,
    HW_WAKEUP_NETWORK  = 5,
    HW_WAKEUP_GPIO     = 6,
};

enum EHwGpioDirection {
    HW_GPIO_INPUT  = 0,
    HW_GPIO_OUTPUT = 1,
};

enum EHwGpioPull {
    HW_GPIO_PULL_NONE = 0,
    HW_GPIO_PULL_UP   = 1,
    HW_GPIO_PULL_DOWN = 2,
};

enum EHwGpioValue {
    HW_GPIO_LOW  = 0,
    HW_GPIO_HIGH = 1,
};

enum EHwGpioEdge {
    HW_GPIO_EDGE_NONE    = 0,
    HW_GPIO_EDGE_RISING  = 1,
    HW_GPIO_EDGE_FALLING = 2,
    HW_GPIO_EDGE_BOTH    = 3,
};

struct SHwGpioPinConfig {
    int32_t          number;
    EHwGpioDirection direction;
    EHwGpioPull      pull;
    EHwGpioEdge      edge;
    std::string      label;
};

enum EHwUartBaudRate {
    HW_UART_BAUD_9600   = 9600,
    HW_UART_BAUD_19200  = 19200,
    HW_UART_BAUD_38400  = 38400,
    HW_UART_BAUD_57600  = 57600,
    HW_UART_BAUD_115200 = 115200,
    HW_UART_BAUD_230400 = 230400,
    HW_UART_BAUD_460800 = 460800,
    HW_UART_BAUD_921600 = 921600,
};

enum EHwUartDataBits {
    HW_UART_DATA_5 = 5,
    HW_UART_DATA_6 = 6,
    HW_UART_DATA_7 = 7,
    HW_UART_DATA_8 = 8,
};

enum EHwUartParity {
    HW_UART_PARITY_NONE = 0,
    HW_UART_PARITY_ODD  = 1,
    HW_UART_PARITY_EVEN = 2,
};

enum EHwUartStopBits {
    HW_UART_STOP_1   = 1,
    HW_UART_STOP_1_5 = 2,
    HW_UART_STOP_2   = 2,
};

enum EHwUartFlowControl {
    HW_UART_FLOW_NONE     = 0,
    HW_UART_FLOW_RTS_CTS  = 1,
    HW_UART_FLOW_XON_XOFF = 2,
};

struct SHwUartConfig {
    EHwUartBaudRate      baud;
    EHwUartDataBits      dataBits;
    EHwUartParity        parity;
    EHwUartStopBits      stopBits;
    EHwUartFlowControl   flowCtrl;
    int32_t              vmin;
    int32_t              vtime;
};

enum EHwNetRegState {
    HW_NET_NOT_SEARCHING       = 0,
    HW_NET_REGISTERED_HOME     = 1,
    HW_NET_SEARCHING           = 2,
    HW_NET_REGISTRATION_DENIED = 3,
    HW_NET_UNKNOWN             = 4,
    HW_NET_REGISTERED_ROAMING  = 5,
};

enum EHwModemState {
    HW_MODEM_OFFLINE = 0,
    HW_MODEM_ONLINE  = 1,
    HW_MODEM_UNKNOWN = 2,
};

enum EHwAirplaneMode {
    HW_AIRPLANE_UNKNOWN = 0,
    HW_AIRPLANE_ON      = 1,
    HW_AIRPLANE_OFF     = 2,
};

struct SHwSignalStrength {
    int32_t rssi;
    int32_t rsrp;
    int32_t rsrq;
    int32_t sinr;
};

struct SHwCellInfo {
    int32_t     mcc;
    int32_t     mnc;
    int32_t     lac;
    int64_t     cellId;
    std::string rat;
};

enum EHwPdnIpType {
    HW_PDN_IP_V4 = 1,
    HW_PDN_IP_V6 = 2,
    HW_PDN_IP_V4V6 = 3,
};

struct SHwNetAddr {
    std::string addr;
    std::string netmask;
    int32_t subnetBits;
    std::string gateway;
    std::string dnsp;
    std::string dnss;
};

struct SHwNetAddr6 {
    std::string addr;
    std::string prefix;
    int32_t prefixBits;
    std::string gateway;
    std::string dnsp;
    std::string dnss;
};

struct SHwApnProfile {
    int32_t callId;
    std::string callName;
    int32_t simId;
    int32_t apnId;
    std::string apnName;
    std::string userName;
    std::string password;
    int32_t authPref;
    EHwPdnIpType ipType;
    bool isBackground;
    std::string ifaceName;
};

struct SHwPdnResult {
    int32_t callId;
    int32_t simId;
    int32_t status;
    std::string ifaceName;
    bool hasIpv4;
    SHwNetAddr ipv4;
    bool hasIpv6;
    SHwNetAddr6 ipv6;
    int32_t endReasonType;
    int32_t endReasonCode;
};

struct SHwRouteItem {
    std::string destination;
    std::string gateway;
    std::string ifaceName;
    int32_t metric;
};

enum EHwSimSlot {
    HW_SIM_SLOT_INVALID = 0,
    HW_SIM_SLOT_1       = 1,
    HW_SIM_SLOT_2       = 2,
};

enum EHwSimCardState {
    HW_SIM_CARD_STATE_UNKNOWN                     = 0,
    HW_SIM_CARD_STATE_ABSENT                      = 1,
    HW_SIM_CARD_STATE_PRESENT                     = 2,
    HW_SIM_CARD_STATE_ERROR_UNKNOWN               = 3,
    HW_SIM_CARD_STATE_ERROR_POWER_DOWN            = 4,
    HW_SIM_CARD_STATE_ERROR_POLL_ERROR            = 5,
    HW_SIM_CARD_STATE_ERROR_NO_ATR_RECEIVED       = 6,
    HW_SIM_CARD_STATE_ERROR_VOLT_MISMATCH         = 7,
    HW_SIM_CARD_STATE_ERROR_PARITY_ERROR          = 8,
    HW_SIM_CARD_STATE_ERROR_TECHNICAL_PROBLEMS    = 9,
    HW_SIM_CARD_STATE_ERROR_POSSIBLY_REMOVED      = 10,
    HW_SIM_CARD_STATE_ERROR_NULL_BYTES            = 11,
    HW_SIM_CARD_STATE_ERROR_SAP_CONNECTED         = 12,
    HW_SIM_CARD_STATE_ERROR_CMD_TIMEOUT           = 13,
};

enum EHwSimAppState {
    HW_SIM_APP_STATE_UNKNOWN              = 0,
    HW_SIM_APP_STATE_DETECTED             = 1,
    HW_SIM_APP_STATE_PIN1_REQ             = 2,
    HW_SIM_APP_STATE_PUK1_REQ             = 3,
    HW_SIM_APP_STATE_INITIALIZING         = 4,
    HW_SIM_APP_STATE_PERSO_CK_REQ         = 5,
    HW_SIM_APP_STATE_PERSO_PUK_REQ        = 6,
    HW_SIM_APP_STATE_PERSO_PERM_BLOCKED   = 7,
    HW_SIM_APP_STATE_PIN1_PERM_BLOCKED    = 8,
    HW_SIM_APP_STATE_ILLEGAL              = 9,
    HW_SIM_APP_STATE_READY                = 10,
};

struct SHwSimAppInfo {
    EHwSimAppState state;
    int32_t pin1NumRetries;
    int32_t puk1NumRetries;
    int32_t pin2NumRetries;
    int32_t puk2NumRetries;
};

struct SHwSimCardInfo {
    EHwSimCardState state;
    std::string imsi;
    std::string iccid;
    SHwSimAppInfo appInfo;
};


#endif // __HW_COMMON_TYPES_H__
