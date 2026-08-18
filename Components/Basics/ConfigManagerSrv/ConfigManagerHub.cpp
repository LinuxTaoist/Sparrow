/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ConfigManagerHub.cpp
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
#include "SprLog.h"
#include <string.h>
#include "CommonMacros.h"
#include "CommonTypeDefs.h"
#include "CoreTypeDefs.h"
#include "ConfigManagerHub.h"

#define LOG_TAG "ConfigHub"

using namespace InternalDefs;

static int32_t ConfigValueTypeSize(int valueType)
{
    switch (valueType) {
        case CONFIG_VALUE_TYPE_BOOL:
            return static_cast<int32_t>(sizeof(bool));
        case CONFIG_VALUE_TYPE_INT32:
            return static_cast<int32_t>(sizeof(int32_t));
        case CONFIG_VALUE_TYPE_INT64:
            return static_cast<int32_t>(sizeof(int64_t));
        case CONFIG_VALUE_TYPE_FLOAT:
            return static_cast<int32_t>(sizeof(float));
        case CONFIG_VALUE_TYPE_DOUBLE:
            return static_cast<int32_t>(sizeof(double));
        default:
            return 0;
    }
}

ConfigManagerHub::ConfigManagerHub(const std::string& srvName, ConfigManager* pManager)
    : BinderHub(srvName)
{
    mConfigManager = pManager;
}

ConfigManagerHub::~ConfigManagerHub()
{
}

void ConfigManagerHub::handleCmd(const std::shared_ptr<Parcel>& pReqParcel,
                                 const std::shared_ptr<Parcel>& pRspParcel, int32_t cmd)
{
    if (cmd == GENERAL_REGISTER_CALLBACK) {
        return;
    }

    switch (cmd) {
        case CONFIG_CMD_SET_VALUE: {
            std::string nameSpace;
            std::string key;
            std::string value;
            int scope = CONFIG_SCOPE_USER;
            int revision = 0;
            NONZERO_CHECK(pReqParcel->ReadString(nameSpace));
            NONZERO_CHECK(pReqParcel->ReadString(key));
            NONZERO_CHECK(pReqParcel->ReadString(value));
            NONZERO_CHECK(pReqParcel->ReadInt(scope));
            int ret = mConfigManager->SetValue(nameSpace, key, value, scope, revision);

            NONZERO_CHECK(pRspParcel->WriteInt(revision));
            NONZERO_CHECK(pRspParcel->WriteInt(ret));
            NONZERO_CHECK(pRspParcel->Post());
            break;
        }
        case CONFIG_CMD_GET_VALUE: {
            std::string nameSpace;
            std::string key;
            std::string value;
            int scope = CONFIG_SCOPE_DEFAULT;
            int revision = 0;
            NONZERO_CHECK(pReqParcel->ReadString(nameSpace));
            NONZERO_CHECK(pReqParcel->ReadString(key));
            int ret = mConfigManager->GetValue(nameSpace, key, value, scope, revision);

            NONZERO_CHECK(pRspParcel->WriteInt(ret));
            if (ret == 0) {
                NONZERO_CHECK(pRspParcel->WriteString(value));
                NONZERO_CHECK(pRspParcel->WriteInt(scope));
                NONZERO_CHECK(pRspParcel->WriteInt(revision));
            }
            NONZERO_CHECK(pRspParcel->Post());
            break;
        }
        case CONFIG_CMD_SET_TYPED_VALUE: {
            std::string nameSpace;
            std::string key;
            int valueType = CONFIG_VALUE_TYPE_STRING;
            int valueSize = 0;
            int scope = CONFIG_SCOPE_USER;
            int revision = 0;
            std::vector<uint8_t> value;

            NONZERO_CHECK(pReqParcel->ReadString(nameSpace));
            NONZERO_CHECK(pReqParcel->ReadString(key));
            NONZERO_CHECK(pReqParcel->ReadInt(valueType));
            NONZERO_CHECK(pReqParcel->ReadInt(valueSize));
            if (valueSize <= 0 || valueType <= CONFIG_VALUE_TYPE_STRING || valueType >= CONFIG_VALUE_TYPE_BUTT) {
                NONZERO_CHECK(pRspParcel->WriteInt(revision));
                NONZERO_CHECK(pRspParcel->WriteInt(-1));
                NONZERO_CHECK(pRspParcel->Post());
                break;
            }

            value.resize(valueSize);
            int readSize = 0;
            NONZERO_CHECK(pReqParcel->ReadData(reinterpret_cast<void*>(value.data()), readSize));
            NONZERO_CHECK(pReqParcel->ReadInt(scope));

            int ret = -1;
            if (readSize == valueSize) {
                ret = mConfigManager->SetRawValue(nameSpace, key, value.data(), valueSize, valueType, scope, revision);
            }

            NONZERO_CHECK(pRspParcel->WriteInt(revision));
            NONZERO_CHECK(pRspParcel->WriteInt(ret));
            NONZERO_CHECK(pRspParcel->Post());
            break;
        }
        case CONFIG_CMD_GET_TYPED_VALUE: {
            std::string nameSpace;
            std::string key;
            int valueType = CONFIG_VALUE_TYPE_STRING;
            int scope = CONFIG_SCOPE_DEFAULT;
            int revision = 0;
            std::vector<uint8_t> value;

            NONZERO_CHECK(pReqParcel->ReadString(nameSpace));
            NONZERO_CHECK(pReqParcel->ReadString(key));
            NONZERO_CHECK(pReqParcel->ReadInt(valueType));

            int valueSize = ConfigValueTypeSize(valueType);
            int ret = -1;
            if (valueSize > 0) {
                value.resize(static_cast<size_t>(valueSize));
                ret = mConfigManager->GetRawValue(nameSpace, key, value.data(), valueSize, valueType, scope, revision);
            }
            NONZERO_CHECK(pRspParcel->WriteInt(ret));
            if (ret == 0) {
                NONZERO_CHECK(pRspParcel->WriteInt(valueType));
                NONZERO_CHECK(pRspParcel->WriteInt(scope));
                NONZERO_CHECK(pRspParcel->WriteInt(revision));
                NONZERO_CHECK(pRspParcel->WriteInt(static_cast<int>(value.size())));
                NONZERO_CHECK(pRspParcel->WriteData(reinterpret_cast<void*>(value.data()), static_cast<int>(value.size())));
            }
            NONZERO_CHECK(pRspParcel->Post());
            break;
        }
        case CONFIG_CMD_DELETE_VALUE: {
            std::string nameSpace;
            std::string key;
            int scope = CONFIG_SCOPE_USER;
            int revision = 0;
            NONZERO_CHECK(pReqParcel->ReadString(nameSpace));
            NONZERO_CHECK(pReqParcel->ReadString(key));
            NONZERO_CHECK(pReqParcel->ReadInt(scope));
            int ret = mConfigManager->DeleteValue(nameSpace, key, scope, revision);

            NONZERO_CHECK(pRspParcel->WriteInt(revision));
            NONZERO_CHECK(pRspParcel->WriteInt(ret));
            NONZERO_CHECK(pRspParcel->Post());
            break;
        }
        case CONFIG_CMD_LIST_NAMESPACE: {
            std::string nameSpace;
            std::map<std::string, std::string> items;
            NONZERO_CHECK(pReqParcel->ReadString(nameSpace));
            int ret = mConfigManager->ListNamespace(nameSpace, items);

            NONZERO_CHECK(pRspParcel->WriteInt(ret));
            if (ret == 0) {
                NONZERO_CHECK(pRspParcel->WriteInt(static_cast<int>(items.size())));
                for (std::map<std::string, std::string>::const_iterator it = items.begin(); it != items.end(); ++it) {
                    NONZERO_CHECK(pRspParcel->WriteString(it->first));
                    NONZERO_CHECK(pRspParcel->WriteString(it->second));
                }
            }
            NONZERO_CHECK(pRspParcel->Post());
            break;
        }
        case CONFIG_CMD_GET_META: {
            std::string nameSpace;
            std::string key;
            int scope = CONFIG_SCOPE_DEFAULT;
            int revision = 0;
            NONZERO_CHECK(pReqParcel->ReadString(nameSpace));
            NONZERO_CHECK(pReqParcel->ReadString(key));
            int ret = mConfigManager->GetMeta(nameSpace, key, scope, revision);

            NONZERO_CHECK(pRspParcel->WriteInt(scope));
            NONZERO_CHECK(pRspParcel->WriteInt(revision));
            NONZERO_CHECK(pRspParcel->WriteInt(ret));
            NONZERO_CHECK(pRspParcel->Post());
            break;
        }
        case CONFIG_CMD_BACKUP: {
            int ret = mConfigManager->Backup();
            NONZERO_CHECK(pRspParcel->WriteInt(ret));
            NONZERO_CHECK(pRspParcel->Post());
            break;
        }
        default: {
            SPR_LOGE("Unknown cmd: 0x%x\n", cmd);
            break;
        }
    }
}