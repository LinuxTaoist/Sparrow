/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : 02_SprDirWatch.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/09/27
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/09/27 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <memory>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "PFile.h"
#include "SprDirWatch.h"
#include "EpollEventHandler.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <dir>\n", argv[0]);
        return -1;
    }

    std::string path = argv[1];
    SprDirWatch theDirWatch;
    theDirWatch.AddDirWatch(path, IN_CLOSE_WRITE | IN_MOVED_TO | IN_MOVED_FROM | IN_DELETE);
    std::shared_ptr<PFile> pFile = std::make_shared<PFile>(theDirWatch.GetInotifyFd(), [&](int fd, void* arg) {
        const int size = 100;
        char buffer[size] = {0};
        ssize_t numRead = read(fd, buffer, size);
        if (numRead == -1) {
            printf("read %d failed! (%s)\n", fd, strerror(errno));
            return;
        }

        int offset = 0;
        while( offset < numRead) {
            struct inotify_event* pEvent = reinterpret_cast<struct inotify_event*>(&buffer[offset]);
            if (!pEvent) {
                printf("pEvent is nullptr!\n");
                return;
            }

            if (pEvent->len > 0) {
                printf("File %s changed: 0x%x\n", pEvent->name, pEvent->mask);
            }
            offset += sizeof(struct inotify_event) + pEvent->len;
        }
    });

    pFile->AddToPoll();
    EpollEventHandler::GetInstance()->EpollLoop();
    return 0;
}
