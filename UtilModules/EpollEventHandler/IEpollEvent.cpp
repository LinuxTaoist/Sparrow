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
#include <unistd.h>
#include "IEpollEvent.h"

ssize_t IEpollEvent::Write(int fd, const std::string& bytes)
{
    const char *ptr = bytes.data();
    size_t nleft = bytes.size();
    ssize_t nwritten = 0;

    while (nleft > 0) {
        if ( (nwritten = write(fd, ptr, nleft)) < 0) {
            if (errno == EINTR) {
                fprintf(stderr, "write EINTR :%m\n");
                nwritten = 0;
            } else if (errno == EAGAIN) {
                fprintf(stderr, "write EAGAIN: %m\n"); // buf full
                return -1;
            } else {
                fprintf(stderr, "write err: %m\n");
                return -1;
            }
        } else if (nwritten == 0) {
            printf("write EOF. Client disconnect!\n");
            break;
        }

        nleft -= nwritten;
        ptr += nwritten;
    }

    return (bytes.size() - nleft);
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
                perror("read fail!");
                return -1;
            }
        } else if (nread == 0) {        // 对端关闭连接，读取结束
            break;
        }

        // 将读取的内容添加到原字符串中
        bytes.append(ptr, nread);
        totalBytesRead += nread;
    }

    return totalBytesRead; // 成功读取的字节数
}
