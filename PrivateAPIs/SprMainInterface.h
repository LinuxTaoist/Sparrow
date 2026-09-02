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

extern void (*pSprUserEntry)();
extern void NOT_IMPLEMENT_SPR_ENTRY();
extern void SprRunEventLoop();

// SPR_ENTRY(...) 宏: 定义 Sparrow 进程业务入口。
//
// 设计目标:
// - 用户侧只需关注业务初始化逻辑；
// - 局部对象在事件循环期间保持存活。
//
// 典型写法:
//   SPR_ENTRY(
//       MyModule mod;        // 局部变量
//       mod.init();
//   );
//
// 上述展开后会在同一作用域末尾自动执行 SprRunEventLoop();
// 因此 mod 在 EpollLoop 运行期间不会析构，直到循环退出。
//
// NOT_IMPLEMENT_SPR_ENTRY: 链接期检查，未实现 SPR_ENTRY() 时触发报错。
#define SPR_ENTRY(...)                                                       \
    void SprUserEntry() {                                                    \
        [&]() {                                                              \
            __VA_ARGS__                                                      \
            SprRunEventLoop();                                               \
        }();                                                                 \
    }                                                                        \
    static struct SprEntryRegister {                                          \
        SprEntryRegister() { ::pSprUserEntry = SprUserEntry; }                \
    } SprEntryRegister;                                                       \
    void NOT_IMPLEMENT_SPR_ENTRY(){}

#endif // __SPR_MAIN_INTERFACE_H__
