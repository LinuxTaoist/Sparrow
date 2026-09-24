/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_watch.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/04/23
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <stdio.h>
#include "MainMenu.h"

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)

int main(int argc, const char* argv[]) {
    MainMenu::GetInstance().MenuLoop();
    return 0;
}
