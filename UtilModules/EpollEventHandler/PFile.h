/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PFile.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/10/17
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/10/17 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __PFILE_H__
#define __PFILE_H__

#include <string>
#include <functional>
#include "IEpollEvent.h"

class PFile : public IEpollEvent
{
public:
    PFile(int fd, std::function<void(int, void*)> cb = nullptr, void* arg = nullptr);
    virtual ~PFile();
    void* EpollEvent(int fd, EpollType eType, void* arg) override;

private:
    std::function<void(int, void*)> mCb;
};
#endif // __PFILE_H__
