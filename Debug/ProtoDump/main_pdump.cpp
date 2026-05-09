/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_pdump.cpp
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
#include <string>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include "CFactory.h"

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)

int main(int argc, const char* argv[])
{
    if (argc != 2 && argc != 3) {
        SPR_LOG("Usage: \n");
        SPR_LOG(" Dump Config: %s <config path> \n", argv[0]);
        SPR_LOG(" Dump Bytes: %s <config path> <bytes path>\n", argv[0]);
        return -1;
    }

    std::string cfgPath = argv[1];
    CFactory& theFactory = CFactory::GetInstance();
    if (argc == 2) {
        std::shared_ptr<CNode> theCfgParser = theFactory.CreateCfgParserByCfgFile(cfgPath);
        theFactory.PrintConfigDetails(theCfgParser);
    } else {
        std::string bytesPath = argv[2];
        theFactory.PrintDataDetailsByFiles(cfgPath, bytesPath);
    }

    return 0;
}
