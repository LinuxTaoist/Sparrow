/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprObserverWithSocket.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/08/15
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/08/15 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __SPR_OBSERVER_WITH_SOCKET_H__
#define __SPR_OBSERVER_WITH_SOCKET_H__

#include <PSocket.h>
#include <SprObserver.h>

class SprObserverWithSocket : public SprObserver, public PSocket
{
public:
    /**
     * @brief  Construct / Destruct
     *
     * @param id module id
     * @param name module name
     * @param proxyType message proxy type
     * @param domain socket domain
     * @param type socket type
     * @param protocol socket protocol
     * @param cb callback function
     * @param arg callback function argument
     */
    SprObserverWithSocket(ModuleIDType id, const std::string& name, InternalDefs::EProxyType proxyType,
        int domain, int type, int protocol, std::function<void(int, void*)> cb, void* arg = nullptr);

    virtual ~SprObserverWithSocket();

    /**
     * @brief Initialize socket framework for derived class called in Initialize
     *
     * @return 0 on success, or -1 if an error occurred
     */
    virtual int32_t InitFramework() override;
};

#endif // __SPR_OBSERVER_WITH_SOCKET_H__
