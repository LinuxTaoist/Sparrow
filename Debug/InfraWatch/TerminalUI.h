/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TerminalUI.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
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
#ifndef __TERMINAL_UI_H__
#define __TERMINAL_UI_H__

class TerminalUI
{
public:
    TerminalUI();
    ~TerminalUI();
    int MainMenuLoop();

private:
    void ClearScreen();
    char WaitUserInputUntilEnter();
    char WaitUserInputWithoutEnter();
    char DisplayMainMenuAndHandleInput();
    char HandleInputInMainMenu(char input);
    char DisplayMessageQueueStatusAndHandleInput();
    char HandleInputInMessageQueueMenu(char input);
    char DisplayManagerStatusAndHandleInput();
    char HandleInputInManagerStatusMenu(char input);
};

#endif // __TERMINAL_UI_H__
