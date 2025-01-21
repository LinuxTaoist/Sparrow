/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SampleHttpServer.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/12/11
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/12/11 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <list>
#include "PSocket.h"
#include "EpollEventHandler.h"
#include "HttpProtocol.h"

using namespace std;

#define SPR_LOGD(fmt, args...) printf("%4d SampleHttpSrv D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%4d SampleHttpSrv E: " fmt, __LINE__, ##args)

int main(int argc, char *argv[])
{
    EpollEventHandler* pEpoll = EpollEventHandler::GetInstance();

    std::list<std::shared_ptr<PTcpClient>> clients;
    auto tcpServer = make_shared<PTcpServer>([&](int cli, void *arg) {
        PTcpServer* pSrvObj = (PTcpServer*)arg;
        if (pSrvObj == nullptr) {
            SPR_LOGE("pSrvObj is nullptr\n");
            return;
        }

        auto tcpClient = make_shared<PTcpClient>(cli, [&](int sock, void *arg) {
            PTcpClient* pCliObj = (PTcpClient*)arg;
            if (pCliObj == nullptr) {
                SPR_LOGE("pCliObj is nullptr\n");
                return;
            }

            std::string rBuf;
            int rc = pCliObj->Read(sock, rBuf);
            if (rc > 0) {
                SPR_LOGD("# RECV [%d]> %s\n", sock, rBuf.c_str());
                HttpMsgRequest req;
                int32_t rc = req.Decode(rBuf);

                int32_t statusCode = rc;
                std::string httpVersion = "HTTP/1.0";
                std::string body = "<h1>I'm Sorry!</h1>";
                if (rc == 0) {
                    statusCode = 200;
                    httpVersion = req.GetHttpVersion();
                    body = "<h1>Hello World!</h1>";
                }

                std::string sBuf;
                HttpMsgResponse rsp(httpVersion, statusCode, body);
                rsp.Encode(sBuf);
                rc = pCliObj->Write(sock, sBuf);
                if (rc > 0) {
                    SPR_LOGD("# SEND [%d]> %s\n", sock, sBuf.c_str());
                }
            }

            if (rc <= 0) {
                clients.remove_if([sock, pEpoll, pCliObj](shared_ptr<PTcpClient>& v) {
                    return (v->GetEvtFd() == sock);
                });

                SPR_LOGD("Remove Client: %d, total = %d\n", sock, (int)clients.size());
            }
        });

        tcpClient->AsTcpClient();
        clients.push_back(tcpClient);
        SPR_LOGD("New Client: %d %d\n", cli, tcpClient->GetEvtFd());
    });

    tcpServer->AsTcpServer(8080, 5);
    pEpoll->EpollLoop();
    return 0;
}

