/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMQueueDetails.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/03/30
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/03/30 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include "SprLog.h"
#include "SprMQueueDetails.h"

#define LOG_TAG "SprMQDetails"
#define SPR_ROOT_PATH "/tmp/spr_details"

SprMQueueDetails::SprMQueueDetails(const std::string& mqName, bool create) : mIsCreator(create), mMqName(mqName) {
    int32_t fd = -1;
    mpDetails = nullptr;
    std::string filePath = std::string(SPR_ROOT_PATH) + "/" + mqName;

    if (access(SPR_ROOT_PATH, F_OK) != 0) {
        int32_t ret = mkdir(SPR_ROOT_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (ret != 0) {
            SPR_LOGE("mkdir %s failed! (%s)\n", SPR_ROOT_PATH, strerror(errno));
        }
    }

    if (create) {
        fd = open(filePath.c_str(), O_CREAT | O_RDWR, 0666);
        if (fd == -1) {
            SPR_LOGE("Failed to create file: %s", filePath.c_str());
            mpDetails = nullptr;
            return;
        }
        if (ftruncate(fd, sizeof(SMQueueDetails)) == -1) {
            SPR_LOGE("Failed to set size of file: %s", filePath.c_str());
            close(fd);
            mpDetails = nullptr;
            return;
        }
    } else {
        fd = open(filePath.c_str(), O_RDWR);
        if (fd == -1) {
            SPR_LOGE("Failed to open file: %s", filePath.c_str());
            mpDetails = nullptr;
            return;
        }
    }

    mpDetails = static_cast<SMQueueDetails*>(mmap(nullptr, sizeof(SMQueueDetails), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
    if (mpDetails == MAP_FAILED) {
        SPR_LOGE("Failed to map file: %s", filePath.c_str());
        close(fd);
        mpDetails = nullptr;
        return;
    }

    close(fd);
    if (create) {
        mpDetails->handle = -1;
        mpDetails->msgLenPeak = 0;
        mpDetails->msgTotal = 0;
        mpDetails->lastMsgID = 0;
        mpDetails->usedPeak = 0;
        std::snprintf(mpDetails->mqName, MQ_NAME_MAX_LENGTH, "%s", mqName.c_str());
        memset(&mpDetails->mqAttr, 0, sizeof(mq_attr));
    }
}

SprMQueueDetails::~SprMQueueDetails() {
    if (mpDetails != MAP_FAILED && mpDetails != nullptr) {
        if (munmap(mpDetails, sizeof(SMQueueDetails)) == -1) {
            SPR_LOGE("Failed to unmap file");
        }
    }

    if (mIsCreator) {
        std::string filePath = std::string(SPR_ROOT_PATH) + "/" + mMqName;
        if (remove(filePath.c_str()) != 0) {
            SPR_LOGE("Failed to remove file: %s", filePath.c_str());
        }
    }
}

int32_t SprMQueueDetails::SetHandle(int32_t handle) {
    if (!mpDetails && !mIsCreator) {
        SPR_LOGE("Set handle failed! mpDetails = %p, mIsCreator = %d", mpDetails, mIsCreator);
        return -1;
    }

    mpDetails->handle = handle;
    return 0;
}

int32_t SprMQueueDetails::SetMsgLenPeak(int32_t msgLenPeak) {
    if (!mpDetails && !mIsCreator) {
        SPR_LOGE("Set msgLenPeak failed! mpDetails = %p, mIsCreator = %d", mpDetails, mIsCreator);
        return -1;
    }

    mpDetails->msgLenPeak = msgLenPeak;
    return 0;
}

int32_t SprMQueueDetails::SetLastMsgID(uint32_t lastMsgID) {
    if (!mpDetails && !mIsCreator) {
        SPR_LOGE("Set lastMsgID failed! mpDetails = %p, mIsCreator = %d", mpDetails, mIsCreator);
        return -1;
    }

    mpDetails->lastMsgID = lastMsgID;
    return 0;
}

int32_t SprMQueueDetails::SetUsedPeak(int32_t usedPeak) {
    if (!mpDetails && !mIsCreator) {
        SPR_LOGE("Set usedPeak failed! mpDetails = %p, mIsCreator = %d", mpDetails, mIsCreator);
        return -1;
    }

    mpDetails->usedPeak = usedPeak;
    return 0;
}

int32_t SprMQueueDetails::IncrementMsgTotal() {
    if (!mpDetails && !mIsCreator) {
        SPR_LOGE("Increment msgTotal failed! mpDetails = %p, mIsCreator = %d", mpDetails, mIsCreator);
        return -1;
    }

    mpDetails->msgTotal++;
    return mpDetails->msgTotal;
}

int32_t SprMQueueDetails::GetHandle(int32_t& handle) {
    if (!mpDetails) {
        SPR_LOGE("Get handle failed! mpDetails = %p", mpDetails);
        return -1;
    }

    handle = mpDetails->handle;
    return 0;
}

int32_t SprMQueueDetails::GetMsgLenPeak(int32_t& msgLenPeak) {
    if (!mpDetails) {
        SPR_LOGE("Get msgLenPeak failed! mpDetails = %p", mpDetails);
        return -1;
    }

    msgLenPeak = mpDetails->msgLenPeak;
    return 0;
}

int32_t SprMQueueDetails::GetMsgTotal(int32_t& msgTotal) {
    if (!mpDetails) {
        SPR_LOGE("Get msgTotal failed! mpDetails = %p", mpDetails);
        return -1;
    }

    msgTotal = mpDetails->msgTotal;
    return 0;
}

int32_t SprMQueueDetails::GetLastMsgID(uint32_t& lastMsgID) {
    if (!mpDetails) {
        SPR_LOGE("Get lastMsgID failed! mpDetails = %p", mpDetails);
        return -1;
    }

    lastMsgID = mpDetails->lastMsgID;
    return 0;
}

int32_t SprMQueueDetails::GetUsedPeak(int32_t& usedPeak) {
    if (!mpDetails) {
        SPR_LOGE("Get usedPeak failed! mpDetails = %p", mpDetails);
        return -1;
    }

    usedPeak = mpDetails->usedPeak;
    return 0;
}

int32_t SprMQueueDetails::GetMQName(std::string& mqName) {
    if (!mpDetails) {
        mqName = "";
        SPR_LOGE("Get mqName failed! mpDetails = %p", mpDetails);
        return -1;
    }

    mqName = mpDetails->mqName;
    return 0;
}

int32_t SprMQueueDetails::GetMQDetails(SMQueueDetails& details) {
    if (!mpDetails) {
        SPR_LOGE("Get mqDetails failed! mpDetails = %p", mpDetails);
        return -1;
    }

    UpdateMQAttr();
    memcpy(&details, mpDetails, sizeof(SMQueueDetails));
    return 0;
}

int32_t SprMQueueDetails::UpdateMQAttr() {
    if (!mpDetails) {
        SPR_LOGE("Update mqAttr failed! mpDetails = %p", mpDetails);
        return -1;
    }

    int32_t ret = mq_getattr(mpDetails->handle, &mpDetails->mqAttr);
    return ret;
}
