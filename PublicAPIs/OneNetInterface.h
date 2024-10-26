/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : OneNetInterface.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/10/24
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/10/24 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __ONENET_INTERFACE_H__
#define __ONENET_INTERFACE_H__

#include <string>

class OneNetInterface
{
public:
    /**
     * @brief Destructor
     */
    ~OneNetInterface();

    /**
     * @brief Get the Instance object
     *
     * @return  Single instance of OneNetInterface
     */
    static OneNetInterface* GetInstance();

    /**
     * @brief Get binder connect ready status
     *
     * @return true  if ready, false if not
     */
    bool IsReady();

    /**
     * @brief Active OneNet device
     *
     * @param deviceName
     * @return int
     */
    int ActiveDevice(const std::string& deviceName);

    /**
     * @brief Deactive OneNet device
     *
     * @param deviceName
     * @return int
     */
    int DeactiveDevice();

private:
    /**
     * @brief Construct
     */
    OneNetInterface();
};

#endif // __ONENET_INTERFACE_H__