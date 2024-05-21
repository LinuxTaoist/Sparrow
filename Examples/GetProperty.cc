/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : GetProperty.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/04/01
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/04/01 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <string>
#include <stdio.h>
#include "Property.h"

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)
#define SPR_LOGD(fmt, args...) printf("%d DebugBinder D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d DebugBinder E: " fmt, __LINE__, ##args)

int main(int argc, const char *argv[])
{
    if (argc > 3)
    {
        SPR_LOGE("Usage:%s <property name>\n", argv[0]);
        return -1;
    }

    if (argc == 2) {
        std::string propertyName = argv[1];
        std::string propertyValue;
        Property::GetInstance()->GetProperty(propertyName, propertyValue, "null");
        SPR_LOG("%s\n", propertyValue.c_str());
    } else {
        Property::GetInstance()->GetProperties();
    }

    return 0;
}