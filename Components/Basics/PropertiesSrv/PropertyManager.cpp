/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PropertyManager.h
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
#include <atomic>
#include <fstream>
#include <sstream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include "SprLog.h"
#include "SprSigId.h"
#include "CommonMacros.h"
#include "SprDebugNode.h"
#include "SprPropertyMacros.h"
#include "PropertyManager.h"

using namespace InternalDefs;

#define LOG_TAG "ProperM"

#define DEBUG_MODULE_NAME       "Properties"
#define SYSTEM_PROP_PATH        "system.prop"
#define DEFAULT_PROP_PATH       "default.prop"
#define VENDOR_PROP_PATH        "vendor.prop"
#define SHARED_MEMORY_MAX_SIZE  (128 * 1024)    // 128KB

static std::atomic<bool> gObjAlive(true);

PropertyManager::PropertyManager(ModuleIDType id, const std::string& name)
    : SprObserverWithMQueue(id, name)
{
    mSharedMemoryPath = std::string(DEFAULT_DEBUG_ROOT_DIR) + "/" + "__property_shared_memory__";
    mSharedPersistPath = std::string(DEFAULT_DEBUG_ROOT_DIR) + "/" + "__property_shared_persist__";
}

PropertyManager::~PropertyManager()
{
    gObjAlive = false;
    UnregisterDebugFuncs();
}

PropertyManager* PropertyManager::GetInstance(ModuleIDType id, const std::string& name)
{
    if (!gObjAlive) {
        return nullptr;
    }

    static PropertyManager instance(id, name);
    return &instance;
}

int32_t PropertyManager::SetProperty(const std::string& key, const std::string& value)
{
    if (mpSharedMemory == nullptr) {
        SPR_LOGE("mpSharedMemory is nullptr!\n");
        return -1;
    }

    return HandleKeyValue(key, value);
}

int32_t PropertyManager::GetProperty(const std::string& key, std::string& value, const std::string& defaultValue)
{
    int32_t ret = -1;
    if (mpSharedMemory == nullptr) {
        SPR_LOGE("mpSharedMemory is nullptr!\n");
        return ret;
    }

    ret = mpSharedMemory->GetValue(key, value);
    if (ret != 0) {
        value = defaultValue;
    }

    return ret;
}

int32_t PropertyManager::GetProperties(std::map<std::string, std::string>& properties)
{
    if (mpSharedMemory == nullptr) {
        SPR_LOGE("mpSharedMemory is nullptr!\n");
        return -1;
    }

    mpSharedMemory->GetAllKeyValues(properties);
    return 0;
}

int32_t PropertyManager::DumpProperties()
{
    if (mpSharedMemory == nullptr) {
        SPR_LOGE("mpSharedMemory is nullptr!\n");
        return -1;
    }

    return DumpPropertyList();
}

int32_t PropertyManager::Init()
{
    mpSharedMemory = std::unique_ptr<SharedBinaryTree>(new (std::nothrow) SharedBinaryTree(mSharedMemoryPath, SHARED_MEMORY_MAX_SIZE));
    mpPersistMemory = std::unique_ptr<SharedBinaryTree>(new (std::nothrow) SharedBinaryTree(mSharedPersistPath, SHARED_MEMORY_MAX_SIZE, false));

    // load default property
    LoadPropertiesFromFile(DEFAULT_PROP_PATH);

    // load system property
    LoadPropertiesFromFile(SYSTEM_PROP_PATH);

    // load vendor property
    LoadPropertiesFromFile(VENDOR_PROP_PATH);

    // load persist properties
    LoadPersistProperty();

    RegisterDebugFuncs();
    return 0;
}

int32_t PropertyManager::ProcessMsg(const SprMsg& msg)
{
    SPR_LOGD("Recv msg: %s\n", GetSigName(msg.GetMsgId()));
    switch (msg.GetMsgId()) {
        case SIG_ID_PROPERTY_CHANGED:
            MsgRespondPropertyChanged(msg);
            break;
        case SIG_ID_PROPERTY_GET_REQUEST:
            MsgRespondPropertyGetRequest(msg);
            break;
        case SIG_ID_PROPERTY_SET_REQUEST:
            MsgRespondPropertySetRequest(msg);
            break;
        default:
            break;
    }
    return 0;
}

void PropertyManager::MsgRespondPropertyChanged(const SprMsg& msg)
{
    const std::string text = msg.GetString();
    const size_t eqPos = text.find('=');
    if (eqPos == std::string::npos) {
        SPR_LOGE("Invalid property: %s\n", text.c_str());
        return;
    }

    std::string key = text.substr(0, eqPos);
    if (key == PROP_KEY_LOG_LEVEL) {
        HandlePropertyLogLevel(text);
        return;
    } else if (key == PROP_KEY_LOG_LENGTH) {
        HandlePropertyLogLength(text);
        return;
    }

    SprMsg copyMsg = msg;
    NotifyAllObserver(copyMsg);
}

void PropertyManager::MsgRespondPropertyGetRequest(const SprMsg& msg)
{
    std::string value;
    std::string key = msg.GetString();
    int32_t ret = GetProperty(key, value, "");
    if (ret != 0) {
        SPR_LOGE("Get property %s fail!\n", key.c_str());
        return;
    }

    SprMsg rspMsg(SIG_ID_PROPERTY_GET_RESPONSE);
    rspMsg.SetString(key + "=" + value);
    NotifyObserver((ModuleIDType)msg.GetFrom(), rspMsg);
}

void PropertyManager::MsgRespondPropertySetRequest(const SprMsg& msg)
{
    const std::string text = msg.GetString();
    const size_t eqPos = text.find('=');
    if (eqPos == std::string::npos) {
        SPR_LOGE("Invalid property: %s\n", text.c_str());
        return;
    }

    std::string key = text.substr(0, eqPos);
    std::string value = text.substr(eqPos + 1);
    int32_t ret = SetProperty(key, value);
    if (ret != 0) {
        SPR_LOGE("Set property %s fail!\n", text.c_str());
        return;
    }

    SprMsg rspMsg(SIG_ID_PROPERTY_SET_RESPONSE);
    rspMsg.SetString(text);
    rspMsg.SetBoolValue(true);
    NotifyObserver((ModuleIDType)msg.GetFrom(), rspMsg);
}

void PropertyManager::RegisterDebugFuncs()
{
    SprDebugNode* p = SprDebugNode::GetInstance();
    if (!p) {
        SPR_LOGE("p is nullptr!\n");
        return;
    }

    p->RegisterCmd(DEBUG_MODULE_NAME, "DumpAllProperties", "Dump all properties",  std::bind(&PropertyManager::DebugDumpPropertyList, this, std::placeholders::_1));
}

void PropertyManager::UnregisterDebugFuncs()
{
    SprDebugNode* p = SprDebugNode::GetInstance();
    if (!p) {
        SPR_LOGE("p is nullptr!\n");
        return;
    }

    SPR_LOGD("Unregister %s all debug funcs\n", DEBUG_MODULE_NAME);
    p->UnregisterCmd(DEBUG_MODULE_NAME);
}

void PropertyManager::DebugDumpPropertyList(const std::vector<std::string>& args)
{
    DumpPropertyList();
}

int32_t PropertyManager::DumpPropertyList()
{
    std::map<std::string, std::string> keyValueMap;
    mpSharedMemory->GetAllKeyValues(keyValueMap);

    for (auto& it : keyValueMap) {
        SPR_LOGI("%s=%s\n", it.first.c_str(), it.second.c_str());
    }

    return 0;
}

int32_t PropertyManager::LoadPropertiesFromFile(const std::string& fileName)
{
    std::ifstream file(fileName);
    if (!file) {
        SPR_LOGE("Open %s fail! \n", fileName.c_str());
        return -1;
    }

    SPR_LOGI("Load %s.\n", fileName.c_str());
    std::string line;

    while (std::getline(file, line)) {
        // Deal with line tail comment: keep the content before #
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }

        // Jump through empty lines
        if (line.empty()) continue;

        // Jump through lines that only contain whitespace characters
        size_t firstNonSpace = line.find_first_not_of(" \t");
        if (firstNonSpace == std::string::npos) {
            continue;
        }

        size_t delimiter = line.find('=');
        if (delimiter != std::string::npos) {
            std::string key = line.substr(0, delimiter);
            std::string value = line.substr(delimiter + 1);

            key.erase(key.find_last_not_of(" \t") + 1);
            key.erase(0, key.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));

            HandleKeyValue(key, value);
        }
    }

    return 0;
}

int32_t PropertyManager::LoadPersistProperty()
{
    if (!mpPersistMemory) {
        SPR_LOGE("mpPersistMemory is nullptr!\n");
        return -1;
    }

    std::map<std::string, std::string> persistMap;
    mpPersistMemory->GetAllKeyValues(persistMap);
    for (auto& it : persistMap) {
        mpSharedMemory->SetValue(it.first, it.second);
    }

    return 0;
}

int32_t PropertyManager::HandlePropertyLogLevel(const std::string& text)
{
    const size_t eqPos = text.find('=');
    if (eqPos == std::string::npos || text.substr(0, eqPos) != PROP_KEY_LOG_LEVEL) {
        return 0;
    }

    char* endptr = nullptr;
    const std::string value = text.substr(eqPos + 1);
    const int32_t level = strtol(value.c_str(), &endptr, 10);
    if (*endptr != '\0' ||
        level < (int32_t)InternalDefs::LOG_LEVEL_MIN ||
        level > (int32_t)InternalDefs::LOG_LEVEL_BUTT) {
        SPR_LOGE("Invalid log level: %s\n", text.c_str());
        return -1;
    }

    const int32_t oldLevel = SprLog::GetInstance()->GetLevel();
    if (level != oldLevel) {
        SprLog::GetInstance()->SetLevel(level);
        SPR_LOGD("Log level changed! %d -> %d\n", oldLevel, level);
    }

    SprMsg msg(SIG_ID_PROPERTY_LOG_LEVEL_CHANGED);
    msg.SetI32Value(level);
    NotifyAllObserver(msg);
    return 0;
}

int32_t PropertyManager::HandlePropertyLogLength(const std::string& text)
{
    const size_t eqPos = text.find('=');
    if (eqPos == std::string::npos || text.substr(0, eqPos) != PROP_KEY_LOG_LENGTH) {
        return 0;
    }

    char* endptr = nullptr;
    const std::string value = text.substr(eqPos + 1);
    const int32_t length = strtol(value.c_str(), &endptr, 10);
    if (*endptr != '\0' || length < 0 || length > LOG_BUFFER_SIZE_LIMIT) {
        SPR_LOGE("Invalid log length: %s\n", text.c_str());
        return -1;
    }

    const int32_t oldLength = SprLog::GetInstance()->GetLength();
    if (length != oldLength) {
        SprLog::GetInstance()->SetLength(length);
        SPR_LOGD("Log length changed! %d -> %d\n", oldLength, length);
    }

    SprMsg msg(SIG_ID_PROPERTY_LOG_LENGTH_CHANGED);
    msg.SetI32Value(length);
    NotifyAllObserver(msg);
    return 0;
}

int32_t PropertyManager::HandleKeyValue(const std::string& key, const std::string& value)
{
    if (!mpSharedMemory) {
        SPR_LOGE("mpSharedMemory is nullptr!\n");
        return -1;
    }

    // 1. Not modify, if the same value
    std::string tmpValue;
    int32_t ret = mpSharedMemory->GetValue(key, tmpValue);
    if (ret == 0 && tmpValue == value) {
        SPR_LOGD("Not modify \"%s\" (the same value \"%s\")\n", key.c_str(), value.c_str());
        return 0;
    }

    // 2. Not modify, if the key is "ro." type and already exists
    if (ret == 0 && key.rfind("ro.", 0) == 0) {
        SPR_LOGW("Not modify \"%s\" (ro. type key already exists)\n", key.c_str());
        return -1;
    }

    // 3. Set the value, if the key is new or can be modified
    // 4. Notify the property changed
    // 5. Save the "persist." type value to file
    ret = mpSharedMemory->SetValue(key, value);
    if (ret == 0) {
        SPR_LOGD("Set \"%s\"=\"%s\" success!\n", key.c_str(), value.c_str());
        SprMsg msg(SIG_ID_PROPERTY_CHANGED);
        msg.SetString(key + "=" + value);
        SendMsg(msg);

        if (key.rfind("persist.", 0) == 0) {
            SavePersistProperty(key, value);
        }
    }

    return ret;
}

int32_t PropertyManager::SavePersistProperty(const std::string& key, const std::string& value)
{
    return mpPersistMemory->SetValue(key, value);
}
