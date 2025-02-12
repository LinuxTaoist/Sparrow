/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : BindInterface.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/03/16
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/03/16 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __BIND_INTERFACE_H__
#define __BIND_INTERFACE_H__

#include <string>
#include <memory>
#include "Binder.h"
#include "IBinder.h"

class BindInterface
{
public:
    /**
     * @brief  Destructor
     */
    ~BindInterface();

    /**
     * @brief  Get the Instance object
     *
     * @return  Signle instance of BindInterface
     */
    static BindInterface* GetInstance();

    /**
     * @brief  Initializes a service as a binder.
     *
     * @param srvName  The name of the service being initialized.
     * @param pReqParcel  A shared pointer to the request parcel.
     * @param pRspParcel  A shared pointer to the response parcel.
     * @return  true if success, false if failed.
     *
     * Used to the service register the entry for external processes to call.
     */
    bool InitializeServiceBinder(const std::string& srvName,
                                 std::shared_ptr<Parcel>& pReqParcel,
                                 std::shared_ptr<Parcel>& pRspParcel);

    /**
     * @brief Initializes a client to communicate with a service via binder.
     *
     * @param srvName The name of the service the client wishes to communicate with.
     * @param pReqParcel A shared pointer to the request parcel.
     * @param pRspParcel A shared pointer to the response parcel.
     * @return  true if success, false if failed.
     *
     * Used to the client connect to binder of target serivce.
     */
    bool InitializeClientBinder(const std::string& srvName,
                                std::shared_ptr<Parcel>& pReqParcel,
                                std::shared_ptr<Parcel>& pRspParcel);

private:
    /**
     * @brief  Constructor
     */
    BindInterface() = default;

    /**
     * @brief  Add a service as a binder
     *
     * @param name  The name of the service to add
     * @return  A shared pointer of the binder
     *
     * This function adds as a binder, called by self service.
     */
    std::shared_ptr<Binder> AddService(const std::string& name);

    /**
     * @brief  Get the Service object binder
     *
     * @param name  The name of the service to get
     * @return  A shared pointer of the ibinder
     *
     * This function gets the binder of the service, called by other clients.
     */
    std::shared_ptr<IBinder> GetService(const std::string& name);

    /**
     * @brief  Remove a service binder
     *
     * @param name The name of the service to remove
     * @return  0 if success, -1 if failed
     */
    int32_t RemoveService(const std::string& name);
};

#endif // __BIND_INTERFACE_H__
