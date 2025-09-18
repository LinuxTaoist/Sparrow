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
#include "PropertyManager.h"

using namespace InternalDefs;

#define LOG_TAG "ProperM"

#define DEBUG_MODULE_NAME       "Properties"
#define SYSTEM_PROP_PATH        "system.prop"
#define DEFAULT_PROP_PATH       "default.prop"
#define VENDOR_PROP_PATH        "vendor.prop"

#define PERSIST_FILE_PATH       "/tmp/persist/"
#define SHARED_MEMORY_PATH      "/tmp/__property_shared_memory__"

#define SHARED_MEMORY_MAX_SIZE  (128 * 1024)

static std::atomic<bool> gObjAlive(true);

PropertyManager::PropertyManager(ModuleIDType id, const std::string& name)
    : SprObserverWithMQueue(id, name)
{
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

int32_t PropertyManager::GetProperties()
{
    if (mpSharedMemory == nullptr) {
        SPR_LOGE("mpSharedMemory is nullptr!\n");
        return -1;
    }

    return DumpPropertyList();
}

int32_t PropertyManager::Init()
{
    mpSharedMemory = std::unique_ptr<SharedBinaryTree>(new (std::nothrow) SharedBinaryTree(SHARED_MEMORY_PATH, SHARED_MEMORY_MAX_SIZE));

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
    if (key == PROPERTY_KEY_LOG_LEVEL) {
        HandlePropertyLogLevel(text);
        return;
    }

    SprMsg copyMsg = msg;
    NotifyAllObserver(copyMsg);
    return;
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
    std::string buffer;
    while (std::getline(file, buffer)) {
        line += buffer + "\n";
    }

    std::istringstream iss(line);
    std::string keyValue;
    while (std::getline(iss, keyValue, '\n')) {
        size_t delimiter = keyValue.find('=');
        if (delimiter != std::string::npos) {
            std::string key = keyValue.substr(0, delimiter);
            std::string value = keyValue.substr(delimiter + 1);
            HandleKeyValue(key, value);
        }
    }

    return 0;
}

int32_t PropertyManager::LoadPersistProperty()
{
    DIR* dir;

    if (access(PERSIST_FILE_PATH, F_OK) != 0) {
        SPR_LOGW("%s not exist. (%s)\n", PERSIST_FILE_PATH, strerror(errno));
        return 0;
    }

    if ((dir = opendir(PERSIST_FILE_PATH)) != nullptr) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_type == DT_REG) {
                std::string filePath = std::string(PERSIST_FILE_PATH) + entry->d_name;
                std::ifstream file(filePath);

                if (file) {
                    std::string value((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                    mpSharedMemory->SetValue(entry->d_name, value);
                } else {
                    SPR_LOGE("Open %s fail! (%s)\n", entry->d_name, strerror(errno));
                }
            }
        }

        closedir(dir);
    } else {
        SPR_LOGW("Open %s fail! (%s)\n", PERSIST_FILE_PATH, strerror(errno));
        return -1;
    }

    return 0;
}

int32_t PropertyManager::HandlePropertyLogLevel(const std::string& text)
{
    const size_t eqPos = text.find('=');
    if (eqPos == std::string::npos || text.substr(0, eqPos) != PROPERTY_KEY_LOG_LEVEL) {
        return 0;
    }

    char* endptr;
    const int32_t level = strtol(text.substr(eqPos + 1).c_str(), &endptr, 10);
    if (*endptr != '\0' || level < (int32_t)InternalDefs::LOG_LEVEL_MIN || level > (int32_t)InternalDefs::LOG_LEVEL_BUTT) {
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
    std::string filePath = std::string(PERSIST_FILE_PATH) + key;

    struct stat info;
    if (stat(PERSIST_FILE_PATH, &info) != 0) {
        if (mkdir(PERSIST_FILE_PATH, 0777) != 0) {
            SPR_LOGE("Create %s fail! (%s)", PERSIST_FILE_PATH, strerror(errno));
            return -1;
        }
    }

    std::ofstream file(filePath);
    if (file) {
        SPR_LOGD("Save persist property %s=%s\n", key.c_str(), value.c_str());
        file << value;
        file.close();
    } else {
        SPR_LOGE("Open %s fail! \n", filePath.c_str());
        return -1;
    }

    return 0;
}
