/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMainEntry.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/10/12
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/10/12 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __SPR_MAIN_INTERFACE_H__
#define __SPR_MAIN_INTERFACE_H__

#include <functional>

extern void (*pSprUserEntry)();
extern void NOT_IMPLEMENT_SPR_ENTRY();
extern void SprRunEventLoop();

// SPR_ENTRY() 宏: 定义 Sparrow 进程业务入口。
//
// 宏展开后，用户的 {} body 成为内部函数 _SprUserInit，
// 并接收一个参数 _SprRunLoop (std::function<void()>)。
// 在 body 内调用 _SprRunLoop() 即启动事件循环；
// 调用点之前声明的所有局部变量在事件循环运行期间保持存活，
// 事件循环退出后才随 } 一并析构。
//
// 典型写法:
//   SPR_ENTRY()
//   {
//       MyModule mod;        // 局部变量
//       mod.init();
//       _SprRunLoop();       // EpollLoop 在此运行，mod 仍然存活
//   }                        // mod 析构发生在这里，EpollLoop 退出之后
//
// NOT_IMPLEMENT_SPR_ENTRY: 链接期检查，未实现 SPR_ENTRY() 时触发报错。
#define SPR_ENTRY()                                                          \
    static void _SprUserInit(std::function<void()> _SprRunLoop);             \
    void SprUserEntry() {                                                     \
        _SprUserInit([]() { SprRunEventLoop(); });                            \
    }                                                                         \
    static struct SprEntryRegister {                                          \
        SprEntryRegister() { ::pSprUserEntry = SprUserEntry; }                \
    } SprEntryRegister;                                                       \
    void NOT_IMPLEMENT_SPR_ENTRY(){}                                          \
    static void _SprUserInit(std::function<void()> _SprRunLoop)

#endif // __SPR_MAIN_INTERFACE_H__
