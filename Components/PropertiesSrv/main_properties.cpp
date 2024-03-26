/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_properties.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
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
#include "SprCommonType.h"
#include "PropertyManager.h"

using namespace std;
using namespace InternalEnum;

#define SPR_LOGD(fmt, args...)  printf("%d Properties D: " fmt, __LINE__, ##args)

int main(int argc, char * argv[])
{
    PropertyManager* pProperM = PropertyManager::GetInstance(MODULE_PROPERTYM, "PropertyManager");
    pProperM->Init();

    pProperM->MainLoop();
    return 0;
}
