/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : Config.cpp
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
#include <atomic>
#include <memory>
#include <mutex>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "Config.h"
#include "Parcel.h"
#include "ProcMutex.h"
#include "CommonMacros.h"
#include "CoreTypeDefs.h"
#include "GeneralUtils.h"
#include "BindInterface.h"

using namespace InternalDefs;
using namespace GeneralUtils;

#define SPR_LOGI(fmt, args...) printf("%s %6d %12s I: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "Config", __LINE__, ##args)
#define SPR_LOGD(fmt, args...) printf("%s %6d %12s D: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "Config", __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%s %6d %12s W: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "Config", __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%s %6d %12s E: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "Config", __LINE__, ##args)

static std::mutex gTMutex;
static ProcMutex gPMutex("IConfigMutex");
static std::atomic<bool> gObjAlive(true);
static std::shared_ptr<Parcel> pReqParcel = nullptr;
static std::shared_ptr<Parcel> pRspParcel = nullptr;

Config::Config()
{
    mEnable = true;
    if (!EnsureConnected()) {
        mEnable = false;
    }
}

Config::~Config()
{
    gObjAlive = false;
}

Config* Config::GetInstance()
{
    if (!gObjAlive) {
        return nullptr;
    }

    static Config instance;
    return &instance;
}

bool Config::EnsureConnected()
{
    if (mEnable && pReqParcel && pRspParcel) {
        return true;
    }

    bool ret = BindInterface::GetInstance()->InitializeClientBinder(SRV_NAME_CONFIG_MANAGER, pReqParcel, pRspParcel);
    mEnable = ret && pReqParcel && pRspParcel;
    return mEnable;
}

int Config::SetValue(const std::string& nameSpace, const std::string& key, const std::string& value)
{
    return SetValueWithScope(nameSpace, key, value, ::CONFIG_SCOPE_USER);
}

int Config::SetValue(const std::string& key, const std::string& value)
{
    return SetValue(DEFAULT_NAMESPACE, key, value);
}

int Config::SetStrValue(const std::string& key, const std::string& value,
                        int32_t scope, const std::string& nameSpace)
{
    return SetValueWithScope(nameSpace, key, value, scope);
}

int Config::SetBoolValue(const std::string& key, bool value,
                         int32_t scope, const std::string& nameSpace)
{
    int32_t revision = 0;
    return SetTypedValueWithScope(nameSpace, key, &value, static_cast<int32_t>(sizeof(bool)),
                                  ::CONFIG_VALUE_TYPE_BOOL, scope, revision);
}

int Config::SetIntValue(const std::string& key, int32_t value,
                        int32_t scope, const std::string& nameSpace)
{
    int32_t revision = 0;
    return SetTypedValueWithScope(nameSpace, key, &value, static_cast<int32_t>(sizeof(int32_t)),
                                  ::CONFIG_VALUE_TYPE_INT32, scope, revision);
}

int Config::SetInt64Value(const std::string& key, int64_t value,
                          int32_t scope, const std::string& nameSpace)
{
    int32_t revision = 0;
    return SetTypedValueWithScope(nameSpace, key, &value, static_cast<int32_t>(sizeof(int64_t)),
                                  ::CONFIG_VALUE_TYPE_INT64, scope, revision);
}

int Config::SetFloatValue(const std::string& key, float value,
                          int32_t scope, const std::string& nameSpace)
{
    int32_t revision = 0;
    return SetTypedValueWithScope(nameSpace, key, &value, static_cast<int32_t>(sizeof(float)),
                                  ::CONFIG_VALUE_TYPE_FLOAT, scope, revision);
}

int Config::SetDoubleValue(const std::string& key, double value,
                           int32_t scope, const std::string& nameSpace)
{
    int32_t revision = 0;
    return SetTypedValueWithScope(nameSpace, key, &value, static_cast<int32_t>(sizeof(double)),
                                  ::CONFIG_VALUE_TYPE_DOUBLE, scope, revision);
}

int Config::SetValueWithScope(const std::string& nameSpace, const std::string& key,
                              const std::string& value, int32_t scope, int32_t& revision)
{
    if (!EnsureConnected()) {
        SPR_LOGE("Config service is disable!\n");
        return -1;
    }

    ProcLockGuard lock(gPMutex, gTMutex);
    NONZERO_CHECK_RET(pReqParcel->WriteInt(CONFIG_CMD_SET_VALUE));
    NONZERO_CHECK_RET(pReqParcel->WriteString(nameSpace));
    NONZERO_CHECK_RET(pReqParcel->WriteString(key));
    NONZERO_CHECK_RET(pReqParcel->WriteString(value));
    NONZERO_CHECK_RET(pReqParcel->WriteInt(scope));
    NONZERO_CHECK_RET(pReqParcel->Post());

    int ret = 0;
    NONZERO_CHECK_RET(pRspParcel->TimedWait());
    NONZERO_CHECK_RET(pRspParcel->ReadInt(revision));
    NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));
    return ret;
}

int Config::SetTypedValueWithScope(const std::string& nameSpace, const std::string& key,
                                   const void* valueData, int32_t valueSize,
                                   int32_t valueType, int32_t scope, int32_t& revision)
{
    if (!EnsureConnected()) {
        SPR_LOGE("Config service is disable!\n");
        return -1;
    }

    ProcLockGuard lock(gPMutex, gTMutex);
    NONZERO_CHECK_RET(pReqParcel->WriteInt(CONFIG_CMD_SET_TYPED_VALUE));
    NONZERO_CHECK_RET(pReqParcel->WriteString(nameSpace));
    NONZERO_CHECK_RET(pReqParcel->WriteString(key));
    NONZERO_CHECK_RET(pReqParcel->WriteInt(valueType));
    NONZERO_CHECK_RET(pReqParcel->WriteInt(valueSize));
    NONZERO_CHECK_RET(pReqParcel->WriteData(const_cast<void*>(valueData), valueSize));
    NONZERO_CHECK_RET(pReqParcel->WriteInt(scope));
    NONZERO_CHECK_RET(pReqParcel->Post());

    int ret = 0;
    NONZERO_CHECK_RET(pRspParcel->TimedWait());
    NONZERO_CHECK_RET(pRspParcel->ReadInt(revision));
    NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));
    return ret;
}

int Config::SetValueWithScope(const std::string& nameSpace, const std::string& key,
                              const std::string& value, int32_t scope)
{
    int32_t revision = 0;
    return SetValueWithScope(nameSpace, key, value, scope, revision);
}

int Config::GetValue(const std::string& nameSpace, const std::string& key,
                     std::string& value, const std::string& defaultValue, int32_t& scope, int32_t& revision)
{
    if (!EnsureConnected()) {
        value = defaultValue;
        SPR_LOGE("Config service is disable!\n");
        return -1;
    }

    ProcLockGuard lock(gPMutex, gTMutex);
    NONZERO_CHECK_RET(pReqParcel->WriteInt(CONFIG_CMD_GET_VALUE));
    NONZERO_CHECK_RET(pReqParcel->WriteString(nameSpace));
    NONZERO_CHECK_RET(pReqParcel->WriteString(key));
    NONZERO_CHECK_RET(pReqParcel->Post());

    int ret = 0;
    scope = ::CONFIG_SCOPE_DEFAULT;
    revision = 0;
    NONZERO_CHECK_RET(pRspParcel->TimedWait());
    NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));
    if (ret == 0) {
        NONZERO_CHECK_RET(pRspParcel->ReadString(value));
        NONZERO_CHECK_RET(pRspParcel->ReadInt(scope));
        NONZERO_CHECK_RET(pRspParcel->ReadInt(revision));
    } else {
        value = defaultValue;
    }
    return ret;
}

int Config::GetValue(const std::string& nameSpace, const std::string& key,
                     std::string& value, const std::string& defaultValue, int32_t& scope)
{
    int32_t revision = 0;
    return GetValue(nameSpace, key, value, defaultValue, scope, revision);
}

int Config::GetValue(const std::string& nameSpace, const std::string& key,
                     std::string& value, const std::string& defaultValue)
{
    int32_t scope = ::CONFIG_SCOPE_DEFAULT;
    int32_t revision = 0;
    return GetValue(nameSpace, key, value, defaultValue, scope, revision);
}

int Config::GetValue(const std::string& key, std::string& value, const std::string& defaultValue)
{
    return GetValue(DEFAULT_NAMESPACE, key, value, defaultValue);
}

int Config::GetBoolValue(const std::string& key, bool& value,
                         bool defaultValue, const std::string& nameSpace)
{
    int32_t scope = ::CONFIG_SCOPE_DEFAULT;
    int32_t revision = 0;
    int ret = GetTypedValue(nameSpace, key, &value, static_cast<int32_t>(sizeof(bool)),
                            ::CONFIG_VALUE_TYPE_BOOL, scope, revision);
    if (ret != 0) {
        value = defaultValue;
    }
    return ret;
}

int Config::GetIntValue(const std::string& key, int32_t& value,
                        int32_t defaultValue, const std::string& nameSpace)
{
    int32_t scope = ::CONFIG_SCOPE_DEFAULT;
    int32_t revision = 0;
    int ret = GetTypedValue(nameSpace, key, &value, static_cast<int32_t>(sizeof(int32_t)),
                            ::CONFIG_VALUE_TYPE_INT32, scope, revision);
    if (ret != 0) {
        value = defaultValue;
    }
    return ret;
}

int Config::GetInt64Value(const std::string& key, int64_t& value,
                          int64_t defaultValue, const std::string& nameSpace)
{
    int32_t scope = ::CONFIG_SCOPE_DEFAULT;
    int32_t revision = 0;
    int ret = GetTypedValue(nameSpace, key, &value, static_cast<int32_t>(sizeof(int64_t)),
                            ::CONFIG_VALUE_TYPE_INT64, scope, revision);
    if (ret != 0) {
        value = defaultValue;
    }
    return ret;
}

int Config::GetFloatValue(const std::string& key, float& value,
                          float defaultValue, const std::string& nameSpace)
{
    int32_t scope = ::CONFIG_SCOPE_DEFAULT;
    int32_t revision = 0;
    int ret = GetTypedValue(nameSpace, key, &value, static_cast<int32_t>(sizeof(float)),
                            ::CONFIG_VALUE_TYPE_FLOAT, scope, revision);
    if (ret != 0) {
        value = defaultValue;
    }
    return ret;
}

int Config::GetDoubleValue(const std::string& key, double& value,
                           double defaultValue, const std::string& nameSpace)
{
    int32_t scope = ::CONFIG_SCOPE_DEFAULT;
    int32_t revision = 0;
    int ret = GetTypedValue(nameSpace, key, &value, static_cast<int32_t>(sizeof(double)),
                            ::CONFIG_VALUE_TYPE_DOUBLE, scope, revision);
    if (ret != 0) {
        value = defaultValue;
    }
    return ret;
}

int Config::GetTypedValue(const std::string& nameSpace, const std::string& key,
                          void* valueData, int32_t valueSize,
                          int32_t valueType, int32_t& scope, int32_t& revision)
{
    if (!EnsureConnected()) {
        SPR_LOGE("Config service is disable!\n");
        return -1;
    }

    ProcLockGuard lock(gPMutex, gTMutex);
    NONZERO_CHECK_RET(pReqParcel->WriteInt(CONFIG_CMD_GET_TYPED_VALUE));
    NONZERO_CHECK_RET(pReqParcel->WriteString(nameSpace));
    NONZERO_CHECK_RET(pReqParcel->WriteString(key));
    NONZERO_CHECK_RET(pReqParcel->WriteInt(valueType));
    NONZERO_CHECK_RET(pReqParcel->Post());

    int ret = 0;
    int dataType = ::CONFIG_VALUE_TYPE_STRING;
    int dataSize = 0;
    scope = ::CONFIG_SCOPE_DEFAULT;
    revision = 0;

    NONZERO_CHECK_RET(pRspParcel->TimedWait());
    NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));
    if (ret != 0) {
        return ret;
    }

    NONZERO_CHECK_RET(pRspParcel->ReadInt(dataType));
    NONZERO_CHECK_RET(pRspParcel->ReadInt(scope));
    NONZERO_CHECK_RET(pRspParcel->ReadInt(revision));
    NONZERO_CHECK_RET(pRspParcel->ReadInt(dataSize));
    if (dataType != valueType || dataSize != valueSize) {
        return -1;
    }

    int readSize = 0;
    NONZERO_CHECK_RET(pRspParcel->ReadData(valueData, readSize));
    if (readSize != valueSize) {
        return -1;
    }

    return 0;
}

int Config::ListNamespace(const std::string& nameSpace, std::map<std::string, std::string>& items)
{
    if (!EnsureConnected()) {
        SPR_LOGE("Config service is disable!\n");
        return -1;
    }

    ProcLockGuard lock(gPMutex, gTMutex);
    NONZERO_CHECK_RET(pReqParcel->WriteInt(CONFIG_CMD_LIST_NAMESPACE));
    NONZERO_CHECK_RET(pReqParcel->WriteString(nameSpace));
    NONZERO_CHECK_RET(pReqParcel->Post());

    int ret = 0;
    NONZERO_CHECK_RET(pRspParcel->TimedWait());
    NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));
    if (ret == 0) {
        int num = 0;
        items.clear();
        NONZERO_CHECK_RET(pRspParcel->ReadInt(num));
        for (int i = 0; i < num; ++i) {
            std::string key;
            std::string value;
            NONZERO_CHECK_RET(pRspParcel->ReadString(key));
            NONZERO_CHECK_RET(pRspParcel->ReadString(value));
            items[key] = value;
        }
    }

    return ret;
}

int Config::DeleteValue(const std::string& nameSpace, const std::string& key,
                        int32_t scope, int32_t& revision)
{
    if (!EnsureConnected()) {
        SPR_LOGE("Config service is disable!\n");
        return -1;
    }

    ProcLockGuard lock(gPMutex, gTMutex);
    NONZERO_CHECK_RET(pReqParcel->WriteInt(CONFIG_CMD_DELETE_VALUE));
    NONZERO_CHECK_RET(pReqParcel->WriteString(nameSpace));
    NONZERO_CHECK_RET(pReqParcel->WriteString(key));
    NONZERO_CHECK_RET(pReqParcel->WriteInt(scope));
    NONZERO_CHECK_RET(pReqParcel->Post());

    int ret = 0;
    NONZERO_CHECK_RET(pRspParcel->TimedWait());
    NONZERO_CHECK_RET(pRspParcel->ReadInt(revision));
    NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));
    return ret;
}

int Config::DeleteValue(const std::string& nameSpace, const std::string& key,
                        int32_t scope)
{
    int32_t revision = 0;
    return DeleteValue(nameSpace, key, scope, revision);
}

int Config::GetMeta(const std::string& nameSpace, const std::string& key, int32_t& scope, int32_t& revision)
{
    if (!EnsureConnected()) {
        SPR_LOGE("Config service is disable!\n");
        return -1;
    }

    ProcLockGuard lock(gPMutex, gTMutex);
    NONZERO_CHECK_RET(pReqParcel->WriteInt(CONFIG_CMD_GET_META));
    NONZERO_CHECK_RET(pReqParcel->WriteString(nameSpace));
    NONZERO_CHECK_RET(pReqParcel->WriteString(key));
    NONZERO_CHECK_RET(pReqParcel->Post());

    int ret = 0;
    NONZERO_CHECK_RET(pRspParcel->TimedWait());
    NONZERO_CHECK_RET(pRspParcel->ReadInt(scope));
    NONZERO_CHECK_RET(pRspParcel->ReadInt(revision));
    NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));
    return ret;
}

int Config::Backup()
{
    if (!EnsureConnected()) {
        SPR_LOGE("Config service is disable!\n");
        return -1;
    }

    ProcLockGuard lock(gPMutex, gTMutex);
    NONZERO_CHECK_RET(pReqParcel->WriteInt(CONFIG_CMD_BACKUP));
    NONZERO_CHECK_RET(pReqParcel->Post());

    int ret = 0;
    NONZERO_CHECK_RET(pRspParcel->TimedWait());
    NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));
    return ret;
}