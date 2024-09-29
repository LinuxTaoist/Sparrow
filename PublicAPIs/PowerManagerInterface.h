/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PowerManagerInterface.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/05/11
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/05/11 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __POWER_MANAGER_INTERFACE_H__
#define __POWER_MANAGER_INTERFACE_H__

class PowerManagerInterface
{
public:
    /**
     * @brief  Destructor
     */
    ~PowerManagerInterface();

    /**
     * @brief Get the Instance object
     *
     * @return  Single instance of PowerManagerInterface
     */
    static PowerManagerInterface* GetInstance();

    /**
     * @brief  Trigger power on
     *
     * @return  0 if success, -1 if failed
     */
    int PowerOn();

    /**
     * @brief  Trigger power off
     *
     * @return  0 if success, -1 if failed
     */
    int PowerOff();

private:
    /**
     * @brief Constructor
     */
    PowerManagerInterface();
};

#endif // __POWER_MANAGER_INTERFACE_H__
