/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ConfigManagerWatch.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/06/21
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/06/21 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __CONFIG_MANAGER_WATCH_H__
#define __CONFIG_MANAGER_WATCH_H__

#include "BaseWatch.h"

class ConfigManagerWatch : public BaseWatch {
public:
    static ConfigManagerWatch& GetInstance();

private:
    ConfigManagerWatch() = default;
    ~ConfigManagerWatch() override = default;
    void Usage() override;
    void Menu(char input) override;

    int  ReadScopeFromInput(int32_t& scope);
    char HandleSetValue();
    char HandleGetValue();
    char HandleListNamespace();
    char HandleGetMeta();
    char HandleBackup();
};

#endif // __CONFIG_MANAGER_WATCH_H__
