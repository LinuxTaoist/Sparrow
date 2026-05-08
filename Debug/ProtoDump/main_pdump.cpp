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
#include "FileUtils.h"

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
    std::shared_ptr<CNode> theParser = theFactory.CreateParserWithJFile(cfgPath);
    if (argc == 2) {
        theFactory.PrintConfigDetails(theParser);
    } else {
        std::vector<uint8_t> bytes;
        std::string bytesPath = argv[2];
        int ret = FileUtils::ReadCharTextToHexVector(bytesPath, bytes);
        if (ret == -1) {
            SPR_LOG("Read %s Failed! (%s)", bytesPath.c_str(), strerror(errno));
            return -1;
        }

        std::shared_ptr<CNode> pData = theFactory.DecodeWithParser(theParser, bytes);
        if (!pData) {
            SPR_LOG("Decode %s Failed!", bytesPath.c_str());
            return -1;
        }

        theFactory.PrintDataDetails(pData);
    }

    return 0;
}
