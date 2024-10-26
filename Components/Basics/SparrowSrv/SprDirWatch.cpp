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
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "SprLog.h"
#include "SprDirWatch.h"

#define SPR_LOGD(fmt, args...) LOGD("SprDirWatch", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("SprDirWatch", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("SprDirWatch", fmt, ##args)

SprDirWatch::SprDirWatch()
{
    mInotifyFd = inotify_init();
    if (mInotifyFd == -1) {
        SPR_LOGE("inotify_init failed! (%s)\n", strerror(errno));
    }
}

SprDirWatch::~SprDirWatch()
{
    for (int wd : mWatchFds) {
        RemoveDirWatch(wd);
    }
    close(mInotifyFd);
}

int SprDirWatch::AddDirWatch(const std::string& path, uint32_t mask)
{
    int wd = inotify_add_watch(mInotifyFd, path.c_str(), mask);
    if (wd == -1) {
        SPR_LOGE("Add watch %s failed! (%s)\n", path.c_str(), strerror(errno));
        return -1;
    }

    SPR_LOGD("Add watch %s success! wd = %d\n", path.c_str(), wd);
    mWatchFds.insert(wd);
    return wd;
}

int SprDirWatch::RemoveDirWatch(int wd)
{
    SPR_LOGD("Remove watch %d\n", wd);
    if (inotify_rm_watch(mInotifyFd, wd) == -1) {
        SPR_LOGE("Remove watch %d failed! (%s)\n", wd, strerror(errno));
    }

    mWatchFds.erase(wd);
    return 0;
}
