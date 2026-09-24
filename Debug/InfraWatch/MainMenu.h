/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : MainMenu.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/04/23
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __MAIN_MENU_H__
#define __MAIN_MENU_H__

#include "InfraCommon.h"

class MainMenu {
public:
    static MainMenu& GetInstance();
    int  MenuLoop();

private:
    MainMenu() = default;
    ~MainMenu() = default;
    void Usage();
    char Menu(char input);
};

#endif // __MAIN_MENU_H__
