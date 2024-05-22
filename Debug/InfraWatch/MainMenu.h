/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : MainMenu.h
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
#ifndef __MAIN_MENU_H__
#define __MAIN_MENU_H__

#include "InfraCommon.h"

class MainMenu
{
public:
    MainMenu() = default;
    ~MainMenu() = default;

    int  MenuLoop();

private:
    char HandleInputInMenu(char input);
    char MenuEntry();
};

extern MainMenu theMainMenu;

#endif
