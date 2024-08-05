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
#include "CommonMacros.h"
#include "PropertyManager.h"
#include "SprMediatorIpcProxy.h"

#define SYSTEM_PROP_PATH        "system.prop"
#define DEFAULT_PROP_PATH       "default.prop"
#define VENDOR_PROP_PATH        "default.prop"

#define PERSIST_FILE_PATH       "/tmp/persist/"
#define SHARED_MEMORY_PATH      "/tmp/__property_shared_memory__"

#define SHARED_MEMORY_MAX_SIZE  (128 * 1024)

#define SPR_LOGD(fmt, args...) LOGD("Properties", fmt, ##args)
#define SPR_LOGI(fmt, args...) LOGI("Properties", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("Properties", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("Properties", fmt, ##args)

PropertyManager::PropertyManager()
{
}

PropertyManager::~PropertyManager()
{
}

PropertyManager* PropertyManager::GetInstance()
{
    static PropertyManager instance;
    return &instance;
}

int PropertyManager::SetProperty(const std::string& key, const std::string& value)
{
    if (mSharedMemoryPtr == nullptr)
    {
        SPR_LOGE("mSharedMemoryPtr is nullptr!\n");
        return -1;
    }

    return HandleKeyValue(key, value);
}

int PropertyManager::GetProperty(const std::string& key, std::string& value, const std::string& defaultValue)
{
    int ret = -1;
    if (mSharedMemoryPtr == nullptr)
    {
        SPR_LOGE("mSharedMemoryPtr is nullptr!\n");
        return ret;
    }

    ret = mSharedMemoryPtr->GetValue(key, value);
    if (ret != 0)
    {
        value = defaultValue;
    }

    return ret;
}

int PropertyManager::GetProperties()
{
    if (mSharedMemoryPtr == nullptr)
    {
        SPR_LOGE("mSharedMemoryPtr is nullptr!\n");
        return -1;
    }

    return DumpPropertyList();
}

int PropertyManager::Init()
{
    mSharedMemoryPtr = std::unique_ptr<SharedBinaryTree>(new SharedBinaryTree(SHARED_MEMORY_PATH, SHARED_MEMORY_MAX_SIZE));

    // load default property
    LoadPropertiesFromFile(DEFAULT_PROP_PATH);

    // load system property
    LoadPropertiesFromFile(SYSTEM_PROP_PATH);

    // load vendor property
    LoadPropertiesFromFile(VENDOR_PROP_PATH);

    // load persist properties
    LoadPersistProperty();

    EnvReady(SRV_NAME_PROPERTY);
    return 0;
}

int PropertyManager::DumpPropertyList()
{
    std::map<std::string, std::string> keyValueMap;
    mSharedMemoryPtr->GetAllKeyValues(keyValueMap);

    for (auto& it : keyValueMap) {
        SPR_LOGD("%s=%s\n", it.first.c_str(), it.second.c_str());
    }

    return 0;
}

int32_t PropertyManager::EnvReady(const std::string& srvName)
{
    std::string node = "/tmp/" + srvName;
    int fd = creat(node.c_str(), 0644);
    if (fd != -1) {
        close(fd);
    }

    return 0;
}

int PropertyManager::LoadPropertiesFromFile(const std::string& fileName)
{
    std::ifstream file(fileName);
    if (!file)
    {
        SPR_LOGE("Open %s fail! \n", fileName.c_str());
        return -1;
    }

    SPR_LOGI("Load %s.\n", fileName.c_str());
    std::string line;
    std::string buffer;
    while (std::getline(file, buffer))
    {
        line += buffer + "\n";
    }

    std::istringstream iss(line);
    std::string keyValue;
    while (std::getline(iss, keyValue, '\n'))
    {
        size_t delimiter = keyValue.find('=');
        if (delimiter != std::string::npos)
        {
            std::string key = keyValue.substr(0, delimiter);
            std::string value = keyValue.substr(delimiter + 1);
            HandleKeyValue(key, value);
        }
    }

    return 0;
}

int PropertyManager::LoadPersistProperty()
{
    DIR* dir;

    if (access(PERSIST_FILE_PATH, F_OK) != 0)
    {
        SPR_LOGW("%s not exist. (%s)\n", PERSIST_FILE_PATH, strerror(errno));
        return 0;
    }

    if ((dir = opendir(PERSIST_FILE_PATH)) != nullptr)
    {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr)
        {
            if (entry->d_type == DT_REG)
            {
                std::string filePath = std::string(PERSIST_FILE_PATH) + entry->d_name;
                std::ifstream file(filePath);

                if (file)
                {
                    std::string value((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                    mSharedMemoryPtr->SetValue(entry->d_name, value);
                }
                else
                {
                    SPR_LOGE("Open %s fail! (%s)\n", entry->d_name, strerror(errno));
                }
            }
        }

        closedir(dir);
    }
    else
    {
        SPR_LOGW("Open %s fail! (%s)\n", PERSIST_FILE_PATH, strerror(errno));
        return -1;
    }

    return 0;
}

int PropertyManager::HandleKeyValue(const std::string& key, const std::string& value)
{
    int ret = -1;
    if (key.rfind("ro.", 0) == 0)
    {
        std::string tmpValue;
        int rs = mSharedMemoryPtr->GetValue(key, tmpValue);
        if (rs < 0)
        {
            ret = mSharedMemoryPtr->SetValue(key, value);
        } else {
            SPR_LOGW("%s already exists, modify fail!\n", key.c_str());
        }
    } else if (key.rfind("persist.", 0) == 0) {
        ret = mSharedMemoryPtr->SetValue(key, value);
        if (ret == 0)
        {
            SavePersistProperty(key, value);
        }
    } else {
        ret = mSharedMemoryPtr->SetValue(key, value);
    }

    return ret;
}

int PropertyManager::SavePersistProperty(const std::string& key, const std::string& value)
{
    std::string filePath = std::string(PERSIST_FILE_PATH) + key;

    struct stat info;
    if (stat(PERSIST_FILE_PATH, &info) != 0)
    {
        if (mkdir(PERSIST_FILE_PATH, 0777) != 0)
        {
            SPR_LOGE("Create %s fail! (%s)", PERSIST_FILE_PATH, strerror(errno));
            return -1;
        }
    }

    std::ofstream file(filePath);
    if (file)
    {
        SPR_LOGD("Save persist property %s=%s\n", key.c_str(), value.c_str());
        file << value;
        file.close();
    }
    else
    {
        SPR_LOGE("Open %s fail! \n", filePath.c_str());
        return -1;
    }

    return 0;
}
