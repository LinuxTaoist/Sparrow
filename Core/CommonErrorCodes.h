/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CommonErrorCodes.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/02/10
 *
 *  This header file defines common error codes for both client and server code implementations.
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/02/10 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __COMMON_ERROR_CODES_H__
#define __COMMON_ERROR_CODES_H__

enum CommonErrorCode {
    // general error code
    ERR_GENERAL_SUCCESS             =  0,
    ERR_GENERAL_ERROR               = -1,

    // sysem error code
    ERR_SYS_INVALID_PARAM           = -2,
    ERR_SYS_MEMORY_ALLOC_FAILED     = -3,

    // parcel error code
    ERR_PARCEL_WAIT_FAILED          = -300,
    ERR_PARCEL_WAIT_TIMEOUT         = -301,
    ERR_PARCEL_POST_FAILED          = -302,
    ERR_PARCE_WRITE_FAILED          = -303,
    ERR_PARCEL_READ_FAILED          = -304,

    // binder error code
    ERR_BINDER_INIT_FAILED          = -320,
    ERR_BINDER_INVALID_POINTER      = -321,

    // shared memory error code
    ERR_SHM_CREATE_FAILED           = -330,
    ERR_SHM_OPEN_FAILED             = -331,
    ERR_SHM_READ_FAILED             = -332,
    ERR_SHM_WRITE_FAILED            = -333,
    ERR_SHM_CLOSE_FAILED            = -334,
    ERR_SHM_DESTROY_FAILED          = -335,
    ERR_SHM_GET_SIZE_FAILED         = -336,

    ERR_PROPERTY_UNKOWN_ERROR       = -500,
    ERR_PROPERTY_BINDER_INIT_FAILED = -501,

    ERR_CONFIG_UNKOWN_ERROR         = -550,

    ERR_LOGM_UNKOWN_ERROR           = -600,

    ERR_TIMEM_UNKOWN_ERROR          = -650,

    ERR_TIMERM_UNKOWN_ERROR         = -650,

    ERR_SYSTEM_TIMER_UNKOWN_ERROR   = -700,

    ERR_POWERM_UNKOWN_ERROR         = -750,

    ERR_ONENET_MANAGER_UNKOWN_ERROR = -800,

    ERR_ONENET_DEV_UNKOWN_ERROR     = -850,

    ERR_ONENET_DRIVER_UNKOWN_ERROR  = -900,

    ERR_DEBUG_UNKNOW_ERROR          = -950,

    ERR_CODE_BUTT
};

#endif // __COMMON_ERROR_CODES_H__
