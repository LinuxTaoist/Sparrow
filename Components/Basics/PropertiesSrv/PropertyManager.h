/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PropertyManager.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/03/13
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/03/13 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __PROPERTY_MANAGER_H__
#define __PROPERTY_MANAGER_H__

#include <vector>
#include <string>
#include <memory>
#include "SharedBinaryTree.h"
#include "SprObserverWithMQueue.h"

class PropertyManager : public SprObserverWithMQueue
{
public:
    /**
     * @brief GetInstance
     * @return PropertyManager*
     *
     */
    static PropertyManager* GetInstance(ModuleIDType id, const std::string& name);

    /**
     * @brief SetProperty
     *
     * @param key
     * @param value
     * @return 0 on success, or -1 if an error occurred
     *
     */
    int32_t SetProperty(const std::string& key, const std::string& value);

    /**
     * @brief GetProperty
     *
     * @param key
     * @param value
     * @param defaultValue  The default value is returned when error
     * @return 0 on success, or -1 if an error occurred
     *
     */
    int32_t GetProperty(const std::string& key, std::string& value, const std::string& defaultValue);

    /**
     * @brief GetProperties
     *
     * @param properties
     * @return 0 on success, or -1 if an error occurred
     */
    int32_t GetProperties(std::map<std::string, std::string>& properties);

    /**
     * @brief GetProperties
     * @return 0 on success, or -1 if an error occurred
     *
     * Dump all properties to logs, only used for debug.
     */
    int32_t DumpProperties();

private:
    PropertyManager(ModuleIDType id, const std::string& name);
    ~PropertyManager();
    PropertyManager(const PropertyManager&) = delete;
    PropertyManager& operator=(const PropertyManager&) = delete;

    int32_t Init() override;
    int32_t ProcessMsg(const SprMsg& msg) override;

    /* 消息响应函数 */
    void MsgRespondPropertyChanged(const SprMsg& msg);
    void MsgRespondPropertyGetRequest(const SprMsg& msg);
    void MsgRespondPropertySetRequest(const SprMsg& msg);

    // Register/Unregister debug functions
    void RegisterDebugFuncs();
    void UnregisterDebugFuncs();

    // Debug functions
    void DebugDumpPropertyList(const std::vector<std::string>& args);

    int32_t DumpPropertyList();
    int32_t LoadPropertiesFromFile(const std::string& fileName);
    int32_t LoadPersistProperty();
    int32_t HandlePropertyLogLevel(const std::string& text);
    int32_t HandlePropertyLogLength(const std::string& text);
    int32_t HandleKeyValue(const std::string& key, const std::string& value);
    int32_t SavePersistProperty(const std::string& key, const std::string& value);

private:
    std::string mDevName;
    std::unique_ptr<SharedBinaryTree> mpSharedMemory;
    std::unique_ptr<SharedBinaryTree> mpPersistMemory;
};

#endif // __PROPERTY_MANAGER_H__
