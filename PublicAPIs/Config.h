/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : Config.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/06/20
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/06/20 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <map>
#include <string>
#include <stdint.h>

#include "CommonTypeDefs.h"

#define DEFAULT_NAMESPACE "global"

class Config
{
public:
    /**
     * @brief Get the Instance object
     *
     * @return object
     */
    static Config* GetInstance();

    /**
     * @brief SetValue
     *
     * @param nameSpace
     * @param key
     * @param value
     * @return 0 on success, or -1 if an error occurred
     */
    int SetValue(const std::string& nameSpace, const std::string& key, const std::string& value);

    /**
     * @brief SetValue
     *
     * @param key
     * @param value
     * @return 0 on success, or -1 if an error occurred
     */
    int SetValue(const std::string& key, const std::string& value);

    int SetStrValue(const std::string& key, const std::string& value,
                    int32_t scope = CONFIG_SCOPE_USER,
                    const std::string& nameSpace = DEFAULT_NAMESPACE);

    int SetBoolValue(const std::string& key, bool value,
                     int32_t scope = CONFIG_SCOPE_USER,
                     const std::string& nameSpace = DEFAULT_NAMESPACE);

    int SetIntValue(const std::string& key, int32_t value,
                    int32_t scope = CONFIG_SCOPE_USER,
                    const std::string& nameSpace = DEFAULT_NAMESPACE);

    int SetInt64Value(const std::string& key, int64_t value,
                      int32_t scope = CONFIG_SCOPE_USER,
                      const std::string& nameSpace = DEFAULT_NAMESPACE);

    int SetFloatValue(const std::string& key, float value,
                      int32_t scope = CONFIG_SCOPE_USER,
                      const std::string& nameSpace = DEFAULT_NAMESPACE);

    int SetDoubleValue(const std::string& key, double value,
                       int32_t scope = CONFIG_SCOPE_USER,
                       const std::string& nameSpace = DEFAULT_NAMESPACE);

    /**
     * @brief SetValueWithScope
     *
     * @param nameSpace
     * @param key
     * @param value
     * @param scope
     * @param revision
     * @return 0 on success, or -1 if an error occurred
     */
    int SetValueWithScope(const std::string& nameSpace, const std::string& key,
                          const std::string& value, int32_t scope, int32_t& revision);

    /**
     * @brief SetValueWithScope
     *
     * @param nameSpace
     * @param key
     * @param value
     * @param scope
     * @return 0 on success, or -1 if an error occurred
     */
    int SetValueWithScope(const std::string& nameSpace, const std::string& key,
                          const std::string& value, int32_t scope);

    /**
     * @brief GetValue
     *
     * @param nameSpace
     * @param key
     * @param value
     * @param defaultValue  The default value is returned when error
     * @param scope
     * @param revision
     * @return 0 on success, or -1 if an error occurred
     */
    int GetValue(const std::string& nameSpace, const std::string& key,
                 std::string& value, const std::string& defaultValue, int32_t& scope,
                 int32_t& revision);

    /**
     * @brief GetValue
     *
     * @param nameSpace
     * @param key
     * @param value
     * @param defaultValue  The default value is returned when error
     * @param scope
     * @return 0 on success, or -1 if an error occurred
     */
    int GetValue(const std::string& nameSpace, const std::string& key,
                 std::string& value, const std::string& defaultValue, int32_t& scope);

    /**
     * @brief GetValue
     *
     * @param nameSpace
     * @param key
     * @param value
     * @param defaultValue  The default value is returned when error
     * @return 0 on success, or -1 if an error occurred
     */
    int GetValue(const std::string& nameSpace, const std::string& key,
                 std::string& value, const std::string& defaultValue = "");

    int GetBoolValue(const std::string& key, bool& value,
                     bool defaultValue = false,
                     const std::string& nameSpace = DEFAULT_NAMESPACE);

    int GetIntValue(const std::string& key, int32_t& value,
                    int32_t defaultValue = 0,
                    const std::string& nameSpace = DEFAULT_NAMESPACE);

    int GetInt64Value(const std::string& key, int64_t& value,
                      int64_t defaultValue = 0,
                      const std::string& nameSpace = DEFAULT_NAMESPACE);

    int GetFloatValue(const std::string& key, float& value,
                      float defaultValue = 0.0f,
                      const std::string& nameSpace = DEFAULT_NAMESPACE);

    int GetDoubleValue(const std::string& key, double& value,
                       double defaultValue = 0.0,
                       const std::string& nameSpace = DEFAULT_NAMESPACE);

    /**
     * @brief GetValue
     *
     * @param key
     * @param value
     * @param defaultValue  The default value is returned when error
     * @return 0 on success, or -1 if an error occurred
     */
    int GetValue(const std::string& key, std::string& value, const std::string& defaultValue = "");

    /**
     * @brief ListNamespace
     *
     * @param nameSpace
     * @param items
     * @return 0 on success, or -1 if an error occurred
     */
    int ListNamespace(const std::string& nameSpace, std::map<std::string, std::string>& items);

    /**
     * @brief DeleteValue
     *
     * @param nameSpace
     * @param key
     * @param scope
     * @param revision
     * @return 0 on success, or -1 if an error occurred
     */
    int DeleteValue(const std::string& nameSpace, const std::string& key,
                    int32_t scope, int32_t& revision);

    /**
     * @brief DeleteValue
     *
     * @param nameSpace
     * @param key
     * @param scope
     * @return 0 on success, or -1 if an error occurred
     */
    int DeleteValue(const std::string& nameSpace, const std::string& key,
                    int32_t scope = CONFIG_SCOPE_USER);

    /**
     * @brief GetMeta
     *
     * @param nameSpace
     * @param key
     * @param scope
     * @param revision
     * @return 0 on success, or -1 if an error occurred
     */
    int GetMeta(const std::string& nameSpace, const std::string& key, int32_t& scope, int32_t& revision);

    /**
     * @brief Backup
     *
     * @return 0 on success, or -1 if an error occurred
     */
    int Backup();

private:
    /**
     * @brief Constructor
     */
    Config();

    /**
     * @brief Destructor
     */
    ~Config();

    bool EnsureConnected();

    int SetTypedValueWithScope(const std::string& nameSpace, const std::string& key,
                               const void* valueData, int32_t valueSize,
                               int32_t valueType, int32_t scope, int32_t& revision);
    int GetTypedValue(const std::string& nameSpace, const std::string& key,
                      void* valueData, int32_t valueSize,
                      int32_t valueType, int32_t& scope, int32_t& revision);

private:
    bool mEnable;
};

#endif // __CONFIG_H__