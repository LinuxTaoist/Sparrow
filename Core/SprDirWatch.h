/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprSystem.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/10/17
 *
 *  System initialization file
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/10/17 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __SPR_DIR_WATCH_H__
#define __SPR_DIR_WATCH_H__

#include <set>
#include <string>
#include <sys/inotify.h>

class SprDirWatch
{
public:
    SprDirWatch();
    ~SprDirWatch();

    int32_t GetInotifyFd() const { return mInotifyFd; }
    int32_t AddDirWatch(const std::string& path, uint32_t mask);

private:
    int32_t DelDirWatch(int32_t fd);

private:
    int32_t mInotifyFd;
    std::set<int32_t> mWatchFds;
};

#endif // __SPR_DIR_WATCH_H__

