/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : InfraCommon.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/04/23
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/04/23 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __INFRA_COMMON_H__
#define __INFRA_COMMON_H__

#include <iostream>

namespace InfraWatch {

#define CONTINUE_ON_NONQUIT(handle) while(toupper((handle())) != 'Q')

void ClearScreen();
char WaitUserInputUntilEnter();
char WaitUserInputWithoutEnter();
int  ReadIntFromUserInput(int& input);
};

#endif // __INFRA_COMMON_H__