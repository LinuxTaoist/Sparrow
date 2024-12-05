/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : IEpollEvent.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2023/05/07
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/05/07 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <vector>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "IEpollEvent.h"
#include "EpollEventHandler.h"

#define SPR_LOGD(fmt, args...) // printf("%4d IEpEvt D: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%4d IEpEvt W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%4d IEpEvt E: " fmt, __LINE__, ##args)

IEpollEvent::~IEpollEvent() {
    if (mEvtFd > 0) {
        close(mEvtFd);
        DelFromPoll();
        mEvtFd = -1;
    }
}

ssize_t IEpollEvent::Write(int fd, const char* data, size_t size)
{
    const char *ptr = data;
    size_t nleft = size;
    ssize_t nwritten = 0;

    while (nleft > 0) {
        if ( (nwritten = write(fd, ptr, nleft)) < 0) {
            if (errno == EINTR) {
                SPR_LOGE("write failed! (%s)\n", strerror(errno));
                nwritten = 0;
            } else if (errno == EAGAIN) {
                SPR_LOGE("write failed! (%s)\n", strerror(errno));
                return -1;
            } else {
                SPR_LOGE("write failed! (%s)\n", strerror(errno));
                return -1;
            }
        } else if (nwritten == 0) {
            SPR_LOGE("Write EOF, Client disconnect! (%s)\n", strerror(errno));
            break;
        }

        nleft -= nwritten;
        ptr += nwritten;
    }

    return (size - nleft);
}

ssize_t IEpollEvent::Write(int fd, const std::string& bytes)
{
    return Write(fd, bytes.c_str(), bytes.size());
}

ssize_t IEpollEvent::Write(const char* data, size_t size)
{
    return Write(mEvtFd, data, size);
}

ssize_t IEpollEvent::Write(const std::string& bytes)
{
    return Write(mEvtFd, bytes.c_str(), bytes.size());
}

ssize_t IEpollEvent::Read(int fd, char* data, size_t size)
{
    char *ptr = data;
    size_t nleft = size;
    ssize_t nread = 0;

    while (nleft > 0) {
        if ( (nread = read(fd, ptr, nleft)) < 0) {
            if (errno == EINTR) {
                SPR_LOGE("read fail! (%s)\n", strerror(errno));
                continue;
            } else if (errno == EAGAIN) {
                //SPR_LOGW("read retry.\n");
                break;
            } else {
                SPR_LOGE("read fail! (%s)\n", strerror(errno));
                return -1;
            }
        } else if (nread == 0) {
            SPR_LOGE("Read EOF!\n");
            break;
        }

        nleft -= nread;
        ptr += nread;
    }

    return (size - nleft);
}

ssize_t IEpollEvent::Read(int fd, std::string& bytes)
{
    bytes.clear();
    ssize_t totalBytesRead = 0;

    while (true) {
        size_t additionalCapacity = 4096;
        if (bytes.capacity() - totalBytesRead < additionalCapacity) {
            bytes.reserve(std::min(bytes.capacity() + additionalCapacity, bytes.max_size()));
        }

        std::vector<char> tempBuffer(bytes.capacity() - totalBytesRead);
        char* ptr = tempBuffer.data();

        ssize_t nread = read(fd, ptr, tempBuffer.size());
        if (nread < 0) {
            if (errno == EINTR) {       // 忽略中断错误，继续读取
                continue;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {   // 非阻塞I/O，没有更多数据可读，结束本次读取
                break;
            } else {
                SPR_LOGE("Read %d EOF!\n", fd);
                return -1;
            }
        } else if (nread == 0) {        // 对端关闭连接，读取结束
            break;
        }

        bytes.append(ptr, nread);
        totalBytesRead += nread;
    }

    return totalBytesRead;
}

ssize_t IEpollEvent::Read(char* data, size_t size)
{
    return Read(mEvtFd, data, size);
}

ssize_t IEpollEvent::Read(std::string& bytes)
{
    return Read(mEvtFd, bytes);
}

void IEpollEvent::Close()
{
    if (mEvtFd == -1) {
        return;
    }

    SPR_LOGD("Close fd: %d\n", mEvtFd);
    DelFromPoll();
    close(mEvtFd);
    mEvtFd = -1;
}

void IEpollEvent::AddToPoll()
{
    EpollEventHandler::GetInstance()->AddPoll(this);
}

void IEpollEvent::DelFromPoll()
{
    EpollEventHandler::GetInstance()->DelPoll(this);
}
