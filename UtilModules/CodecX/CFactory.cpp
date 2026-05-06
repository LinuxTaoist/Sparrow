/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CFactory.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/05/05
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/05/05 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <fstream>
#include <errno.h>
#include <string.h>
#include "cJSON.h"
#include "CLog.h"
#include "CField.h"
#include "CFactory.h"

CFactory::CFactory() {

}

CFactory::~CFactory() {

}

CFactory& CFactory::GetInstance() {
    static CFactory factory;
    return factory;
}

std::shared_ptr<CNode> CFactory::LoadJsonFile(const std::string& path) {
    std::string text;
    int32_t ret = ReadFile(path, text);
    if (ret <= 0) {
        return nullptr;
    }

    return LoadJsonString(text);
}

std::shared_ptr<CNode> CFactory::LoadJsonString(const std::string& str) {
    cJSON* pRoot = cJSON_Parse(str.c_str());
    if (!pRoot) {
        CLOGE("Parse json failed! (%s)", cJSON_GetErrorPtr());
        return nullptr;
    }

    std::shared_ptr<CField> pRoot = std::make_shared<CField>(nullptr);

    cJSON_Delete(pRoot);
    return nullptr;
}

int32_t CFactory::ReadFile(const std::string& path, std::string& str) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file) {
        return -1;
    }

    file.seekg(0, std::ios::end);
    int32_t size = static_cast<int32_t>(file.tellg());
    file.seekg(0, std::ios::beg);

    str.resize(size);
    file.read(&str[0], size);
    if (!file.good() && !file.eof()) {
        CLOGE("Read %s failed! (%s)", path.c_str(), strerror(errno));
        file.close();
        return -1;
    }

    file.close();
    return size;
}
