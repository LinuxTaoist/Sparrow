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

class ConfigManagerWatch
{
public:
    ConfigManagerWatch() = default;
    ~ConfigManagerWatch() = default;

    char MenuEntry();

private:
    char ShowMenu();
    char HandleInputInMenu(char input);
    int  ReadScopeFromInput(int32_t& scope);
    char HandleSetValue();
    char HandleGetValue();
    char HandleListNamespace();
    char HandleGetMeta();
    char HandleBackup();
};

extern ConfigManagerWatch theConfigManagerWatch;

#endif // __CONFIG_MANAGER_WATCH_H__
