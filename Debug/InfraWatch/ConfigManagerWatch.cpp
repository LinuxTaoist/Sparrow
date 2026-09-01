/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ConfigManagerWatch.cpp
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
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <sstream>
#include <ctype.h>
#include "InfraCommon.h"
#include "CoreTypeDefs.h"
#include "Config.h"
#include "ConfigManagerWatch.h"

using namespace InternalDefs;

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)

ConfigManagerWatch theConfigManagerWatch;

char ConfigManagerWatch::MenuEntry()
{
    char bInput = 0;
    do {
        HandleInputInMenu(bInput);
        char input = InfraWatch::WaitUserInputWithoutEnter();
        bInput = toupper(input);
    } while (bInput != 'Q');

    return bInput;
}

char ConfigManagerWatch::ShowMenu()
{
    InfraWatch::ClearScreen();
    SPR_LOG("===============================  Config Manager  ================================\n"
            "\n"
            "    1. SetValueWithScope  (Add/Update) \n"
            "    2. GetValue           (Query)      \n"
            "    3. ListNamespace                   \n"
            "    4. GetMeta                         \n"
            "    5. Backup                          \n"
            "\n"
            "    [Q] Quit \n"
            "\n"
            "=================================================================================\n");
    return 0;
}

char ConfigManagerWatch::HandleInputInMenu(char input)
{
    ShowMenu();
    switch(input) {
        case '1': {
            HandleSetValue();
            break;
        }
        case '2': {
            HandleGetValue();
            break;
        }
        case '3': {
            HandleListNamespace();
            break;
        }
        case '4': {
            HandleGetMeta();
            break;
        }
        case '5': {
            HandleBackup();
            break;
        }
        case 'Q': {
            break;
        }
        default:
            break;
    }

    return input;
}

int ConfigManagerWatch::ReadScopeFromInput(int32_t& scope)
{
    std::string line;
    SPR_LOG("Input scope [0:default 1:factory 2:user]: ");
    if (InfraWatch::ReadLineFromUserInput(line) != 0) {
        SPR_LOG("Invalid input!\n");
        return -1;
    }

    std::istringstream iss(line);
    int inputScope = -1;
    std::string extra;
    if (!(iss >> inputScope) || (iss >> extra)) {
        SPR_LOG("Invalid input!\n");
        return -1;
    }

    scope = inputScope;
    if (scope < CONFIG_SCOPE_DEFAULT || scope >= CONFIG_SCOPE_BUTT) {
        SPR_LOG("Invalid scope!\n");
        return -1;
    }

    return 0;
}

char ConfigManagerWatch::HandleSetValue()
{
    std::string line;
    std::string nameSpace;
    std::string key;
    std::string value;
    std::string extra;
    int32_t scope = CONFIG_SCOPE_USER;
    int32_t revision = 0;

    SPR_LOG("\nInput <namespace key value>: ");
    if (InfraWatch::ReadLineFromUserInput(line) != 0) {
        SPR_LOG("Invalid input!\n");
        return -1;
    }

    std::istringstream iss(line);
    if (!(iss >> nameSpace >> key >> value) || (iss >> extra)) {
        SPR_LOG("Invalid input!\n");
        return -1;
    }

    if (ReadScopeFromInput(scope) != 0) {
        return -1;
    }

    int ret = Config::GetInstance()->SetValueWithScope(nameSpace, key, value, scope, revision);
    SPR_LOG("SetValueWithScope ret=%d, revision=%d\n", ret, revision);
    return 0;
}

char ConfigManagerWatch::HandleGetValue()
{
    std::string line;
    std::string nameSpace;
    std::string key;
    std::string value;
    std::string extra;
    int32_t scope = CONFIG_SCOPE_DEFAULT;
    int32_t revision = 0;

    SPR_LOG("\nInput <namespace key>: ");
    if (InfraWatch::ReadLineFromUserInput(line) != 0) {
        SPR_LOG("Invalid input!\n");
        return -1;
    }

    std::istringstream iss(line);
    if (!(iss >> nameSpace >> key) || (iss >> extra)) {
        SPR_LOG("Invalid input!\n");
        return -1;
    }

    int ret = Config::GetInstance()->GetValue(nameSpace, key, value, "", scope, revision);
    SPR_LOG("GetValue ret=%d, value=%s, scope=%d, revision=%d\n", ret, value.c_str(), scope, revision);
    return 0;
}

char ConfigManagerWatch::HandleListNamespace()
{
    std::string line;
    std::string nameSpace;
    std::map<std::string, std::string> items;
    std::string extra;

    SPR_LOG("\nInput <namespace>: ");
    if (InfraWatch::ReadLineFromUserInput(line) != 0) {
        SPR_LOG("Invalid input!\n");
        return -1;
    }

    std::istringstream iss(line);
    if (!(iss >> nameSpace) || (iss >> extra)) {
        SPR_LOG("Invalid input!\n");
        return -1;
    }

    int ret = Config::GetInstance()->ListNamespace(nameSpace, items);
    SPR_LOG("ListNamespace ret=%d, count=%d\n", ret, (int)items.size());
    for (std::map<std::string, std::string>::const_iterator it = items.begin(); it != items.end(); ++it) {
        SPR_LOG("  %s = %s\n", it->first.c_str(), it->second.c_str());
    }
    return 0;
}

char ConfigManagerWatch::HandleGetMeta()
{
    std::string line;
    std::string nameSpace;
    std::string key;
    std::string extra;
    int32_t scope = CONFIG_SCOPE_DEFAULT;
    int32_t revision = 0;

    SPR_LOG("\nInput <namespace key>: ");
    if (InfraWatch::ReadLineFromUserInput(line) != 0) {
        SPR_LOG("Invalid input!\n");
        return -1;
    }

    std::istringstream iss(line);
    if (!(iss >> nameSpace >> key) || (iss >> extra)) {
        SPR_LOG("Invalid input!\n");
        return -1;
    }

    int ret = Config::GetInstance()->GetMeta(nameSpace, key, scope, revision);
    SPR_LOG("GetMeta ret=%d, scope=%d, revision=%d\n", ret, scope, revision);
    return 0;
}

char ConfigManagerWatch::HandleBackup()
{
    int ret = Config::GetInstance()->Backup();
    SPR_LOG("Backup ret=%d\n", ret);
    return 0;
}
