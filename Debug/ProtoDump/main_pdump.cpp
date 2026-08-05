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
#include "CLog.h"
#include "CFactory.h"

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)

int main(int argc, const char* argv[])
{
    if (argc < 2) {
        SPR_LOG("Usage: \n");
        SPR_LOG(" Dump Config: %s <config path> \n", argv[0]);
        SPR_LOG(" Dump Bytes: %s <config path> <bytes path> [option]\n", argv[0]);
        return -1;
    }

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-d" || arg == "--debug") {
            CLog::GetInstance().SetLevel(CLogLevel::CLOG_LEVEL_DEBUG);
        }
    }

    std::string cfgPath = argv[1];
    CFactory& theFactory = CFactory::GetInstance();
    if (argc == 2) {
        std::shared_ptr<CNode> theCfgParser = theFactory.CreateCfgParserByCfgFile(cfgPath);
        theFactory.PrintConfigDetails(theCfgParser);
    } else {
        std::string bytesPath = argv[2];
        theFactory.PrintDataDetails(cfgPath, bytesPath);
    }

    return 0;
}
