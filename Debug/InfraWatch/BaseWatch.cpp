/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : BaseWatch.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/09/08
 *---------------------------------------------------------------------------------------------------------------------
 */
#include "BaseWatch.h"

void BaseWatch::Entry() {
    char input = 0;
    do {
        Usage();
        input = InfraWatch::WaitUserInputWithoutEnter();
        Menu(input);
    } while (input != 'q' && input != 'Q');
}
