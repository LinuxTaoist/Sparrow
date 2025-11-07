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

// SprEntryRegister struct: Automatically assigns the user's
// SprUserEntry to the global pSprUserEntry before main()
// NOT_IMPLEMENT_SPR_ENTRY: Reports an error when the library
// is linked but SprUserEntry is not implemented
#define SPR_ENTRY()         \
    void SprUserEntry();                                        \
    static struct SprEntryRegister {                            \
        SprEntryRegister() { ::pSprUserEntry = SprUserEntry; }  \
    } SprEntryRegister;                                         \
    void NOT_IMPLEMENT_SPR_ENTRY(){}                            \
    void SprUserEntry()

#endif // __SPR_MAIN_INTERFACE_H__

