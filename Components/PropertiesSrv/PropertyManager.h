/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PropertyManager.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
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

#include <string>
#include <memory>
#include "SharedBinaryTree.h"
#include "SprObserver.h"

class PropertyManager
{
public:
    /**
     * @brief Destroy the Property Manager object
     *
     */
    virtual ~PropertyManager();

    /**
     * @brief Init
     *
     * @return int
     */
    int Init();

    /**
     * @brief GetInstance
     * @return PropertyManager*
     *
     */
    static PropertyManager* GetInstance();

    /**
     * @brief SetProperty
     *
     * @param key
     * @param value
     * @return 0 on success, or -1 if an error occurred
     *
     */
    int SetProperty(const std::string& key, const std::string& value);

    /**
     * @brief GetProperty
     *
     * @param key
     * @param value
     * @param defaultValue  The default value is returned when error
     * @return 0 on success, or -1 if an error occurred
     *
     */
    int GetProperty(const std::string& key, std::string& value, const std::string& defaultValue);

    /**
     * @brief GetProperties
     * @return 0 on success, or -1 if an error occurred
     *
     * Dump all properties to logs, only used for debug.
     */
    int GetProperties();

private:
    PropertyManager();
    PropertyManager(const PropertyManager&) = delete;
    PropertyManager& operator=(const PropertyManager&) = delete;

    int DumpPropertyList();
    int EnvReady(const std::string& srvName);
    int LoadPropertiesFromFile(const std::string& fileName);
    int LoadPersistProperty();
    int HandleKeyValue(const std::string& key, const std::string& value);
    int SavePersistProperty(const std::string& key, const std::string& value);

private:
    std::string mDevName;
    std::unique_ptr<SharedBinaryTree> mSharedMemoryPtr;
};

#endif // __PROPERTY_MANAGER_H__
