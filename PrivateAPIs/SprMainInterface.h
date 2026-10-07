/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMainInterface.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/10/12
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __SPR_MAIN_INTERFACE_H__
#define __SPR_MAIN_INTERFACE_H__

extern void (*pSprUserEntry)();
extern void NOT_IMPLEMENT_SPR_ENTRY();
extern void SprRunEventLoop();

// 定义 Sparrow 进程的业务入口：初始化业务模块并启动事件循环。
// 入口中声明的局部对象在事件循环退出前保持有效。
//
// 用法:
//   SPR_ENTRY(
//       MyModule mod;        // 局部变量
//       mod.init();
//   );
//
// 进程未定义 SPR_ENTRY 时，构建将因缺少业务入口而失败。
#define SPR_ENTRY(...)                                              \
    void SprUserEntry() {                                           \
        [&]() {                                                     \
            __VA_ARGS__                                             \
            SprRunEventLoop();                                      \
        }();                                                        \
    }                                                               \
    static struct SprEntryRegister {                                \
        SprEntryRegister() { ::pSprUserEntry = SprUserEntry; }      \
    } SprEntryRegister;                                             \
    void NOT_IMPLEMENT_SPR_ENTRY(){}

#endif // __SPR_MAIN_INTERFACE_H__
