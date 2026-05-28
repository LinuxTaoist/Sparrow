/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HUIServer.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : HTTP UI Server implementation
 *  @date       : 2026/05/28
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/05/28 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <stdio.h>
#include <list>
#include <memory>
#include <sstream>
#include "PSocket.h"
#include "EpollEventHandler.h"
#include "HttpCommon.h"
#include "HttpProtocol.h"
#include "SprMediatorInterface.h"
#include "SprLog.h"
#include "HUIServer.h"
#include "HUIRender.h"
#include "HUIShell.h"

using namespace std;

#define LOG_TAG "HUIServer"

namespace {

std::string UrlDecode(const std::string& input)
{
    std::string result;
    for (size_t i = 0; i < input.length(); ++i) {
        if (input[i] == '%' && i + 2 < input.length()) {
            char hex[3];
            hex[0] = input[i + 1];
            hex[1] = input[i + 2];
            hex[2] = '\0';
            int charCode = std::stoi(hex, nullptr, 16);
            result += static_cast<char>(charCode);
            i += 2;
        } else if (input[i] == '+') {
            result += ' ';
        } else {
            result += input[i];
        }
    }
    return result;
}

std::string EscapeJson(const std::string& input)
{
    std::ostringstream oss;
    for (const auto c : input) {
        switch (c) {
            case '"':
                oss << "\\\"";
                break;
            case '\\':
                oss << "\\\\";
                break;
            case '\b':
                oss << "\\b";
                break;
            case '\f':
                oss << "\\f";
                break;
            case '\n':
                oss << "\\n";
                break;
            case '\r':
                oss << "\\r";
                break;
            case '\t':
                oss << "\\t";
                break;
            default:
                oss << c;
                break;
        }
    }
    return oss.str();
}

std::string BuildJsonStatus(int32_t queues, int32_t pending, int32_t total, int32_t peak, const std::string& busiest)
{
    std::ostringstream oss;
    oss << "{"
        << "\"queues\":" << queues << ","
        << "\"pending\":" << pending << ","
        << "\"total\":" << total << ","
        << "\"peak\":" << peak << ","
        << "\"busiest\":\"" << EscapeJson(busiest) << "\""
        << "}";
    return oss.str();
}

std::string BuildJsonQueue(const SMQueueDetails& mq)
{
    std::ostringstream oss;
    oss << "{"
        << "\"name\":\"" << EscapeJson(mq.mqName) << "\","
        << "\"handle\":" << mq.handle << ","
        << "\"pending\":" << (long)mq.mqAttr.mq_curmsgs << ","
        << "\"total\":" << mq.msgTotal << ","
        << "\"peak\":" << mq.usedPeak
        << "}";
    return oss.str();
}

std::string GetHttpStatus()
{
    int32_t queueCount = 0;
    int32_t pendingMsgs = 0;
    int32_t totalMsgs = 0;
    int32_t usedPeak = 0;
    std::string busiestQueue = "-";

    std::vector<SMQueueDetails> mqAttrVec;
    SprMediatorInterface* pMediator = SprMediatorInterface::GetInstance();
    if (pMediator != nullptr && pMediator->GetAllMQStatus(mqAttrVec) == 0) {
        queueCount = static_cast<int32_t>(mqAttrVec.size());
        for (const auto& mqInfo : mqAttrVec) {
            pendingMsgs += mqInfo.mqAttr.mq_curmsgs;
            totalMsgs += mqInfo.msgTotal;
            if (mqInfo.usedPeak >= usedPeak) {
                usedPeak = mqInfo.usedPeak;
                busiestQueue = mqInfo.mqName;
            }
        }
    }

    return BuildJsonStatus(queueCount, pendingMsgs, totalMsgs, usedPeak, busiestQueue);
}

std::string GetQueueDetails()
{
    std::vector<SMQueueDetails> mqAttrVec;
    SprMediatorInterface* pMediator = SprMediatorInterface::GetInstance();

    std::vector<std::string> jsonItems;
    if (pMediator != nullptr && pMediator->GetAllMQStatus(mqAttrVec) == 0) {
        for (const auto& mq : mqAttrVec) {
            jsonItems.push_back(BuildJsonQueue(mq));
        }
    }

    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < jsonItems.size(); ++i) {
        if (i > 0) oss << ",";
        oss << jsonItems[i];
    }
    oss << "]";
    return oss.str();
}

std::string GetHtmlPage()
{
    return HUIRender::HTML_PAGE;
}

std::string GetDeviceProfile()
{
    std::string profile;
    HUIShell::GetDeviceProfile(profile);
    return profile;
}

std::string GetResourceUsage()
{
    std::string resources;
    HUIShell::GetResourceUsage(resources);
    return resources;
}

std::string ExecuteShellCommand(const std::string& cmd)
{
    if (cmd.empty()) {
        std::ostringstream oss;
        oss << "{\"prompt\":\"" << EscapeJson(HUIShell::GetPrompt()) << "\",\"error\":\"Empty command\"}";
        return oss.str();
    }

    std::string promptBefore = HUIShell::GetPrompt();
    std::string output;
    int ret = HUIShell::ExecuteCommand(cmd, output, 5000);
    std::string promptAfter = HUIShell::GetPrompt();

    // Build response in terminal interaction format
    std::ostringstream oss;
    oss << "{"
        << "\"promptBefore\":\"" << EscapeJson(promptBefore) << "\","
        << "\"command\":\"" << EscapeJson(cmd) << "\","
        << "\"output\":\"";

    // Escape output
    for (const auto c : output) {
        switch (c) {
            case '"': oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default: oss << c; break;
        }
    }

    oss << "\","
        << "\"promptAfter\":\"" << EscapeJson(promptAfter) << "\","
        << "\"exitCode\":" << (ret == 0 ? 0 : 1)
        << "}";

    return oss.str();
}

void BuildHttpResponse(const std::string& uri, std::string& body, int32_t& statusCode, bool& isHtmlResponse)
{
    statusCode = HTTP_STATUS_404;
    isHtmlResponse = false;
    body.clear();

    if (uri == "/" || uri.empty()) {
        statusCode = HTTP_STATUS_200;
        body = GetHtmlPage();
        isHtmlResponse = true;
    } else if (uri == "/api/status") {
        statusCode = HTTP_STATUS_200;
        body = GetHttpStatus();
    } else if (uri == "/api/queues") {
        statusCode = HTTP_STATUS_200;
        body = GetQueueDetails();
    } else if (uri == "/api/profile") {
        statusCode = HTTP_STATUS_200;
        body = GetDeviceProfile();
    } else if (uri == "/api/resources") {
        statusCode = HTTP_STATUS_200;
        body = GetResourceUsage();
    } else if (uri.substr(0, 11) == "/api/shell?") {
        statusCode = HTTP_STATUS_200;
        std::string query = uri.substr(11);
        size_t cmdPos = query.find("cmd=");
        if (cmdPos != std::string::npos) {
            std::string encodedCmd = query.substr(cmdPos + 4);
            std::string cmd = UrlDecode(encodedCmd);
            body = ExecuteShellCommand(cmd);
        } else {
            body = "{\"error\":\"Missing cmd parameter\"}";
        }
    }

    if (statusCode == HTTP_STATUS_404) {
        body = "{\"error\":\"Not Found\"}";
    }
}

}  // namespace

HUIServer::HUIServer() : mPort(HUI_DEFAULT_PORT), mRunning(false)
{
}

HUIServer::~HUIServer()
{
    Stop();
}

int HUIServer::Start(int port)
{
    mPort = port;
    mRunning = true;

    // Initialize shell session
    HUIShell::InitSession();

    EpollEventHandler* pEpoll = EpollEventHandler::GetInstance();
    if (pEpoll == nullptr) {
        SPR_LOGE("Failed to get EpollEventHandler\n");
        return -1;
    }

    std::list<std::shared_ptr<PTcpClient>> clients;
    auto tcpServer = std::make_shared<PTcpServer>([this, &clients](int cli, void* arg) {
        PTcpServer* pSrvObj = (PTcpServer*)arg;
        if (pSrvObj == nullptr) {
            SPR_LOGE("Server object is null\n");
            return;
        }

        auto tcpClient = std::make_shared<PTcpClient>(cli, [this, &clients](int sock, void* arg) {
            PTcpClient* pCliObj = (PTcpClient*)arg;
            if (pCliObj == nullptr) {
                SPR_LOGE("Client object is null\n");
                return;
            }

            std::string rBuf;
            int rc = pCliObj->Read(sock, rBuf);
            if (rc > 0) {
                HttpMsgRequest req;
                int32_t ret = req.Decode(rBuf);
                if (ret == 0) {
                    std::string uri = req.GetURI();
                    std::string body;
                    int32_t statusCode = HTTP_STATUS_404;
                    bool isHtmlResponse = false;
                    BuildHttpResponse(uri, body, statusCode, isHtmlResponse);

                    HttpMsgResponse rsp("HTTP/1.0", statusCode, body);
                    if (isHtmlResponse) {
                        rsp.SetHeader("Content-Type", "text/html; charset=UTF-8");
                    } else {
                        rsp.SetHeader("Content-Type", "application/json; charset=UTF-8");
                    }
                    rsp.SetHeader("Content-Length", std::to_string(body.size()));
                    rsp.SetHeader("Connection", "close");

                    std::string sBuf;
                    rsp.Encode(sBuf);
                    pCliObj->Write(sock, sBuf);
                }
            }

            if (rc <= 0) {
                clients.remove_if([sock](std::shared_ptr<PTcpClient>& v) {
                    return (v->GetEvtFd() == sock);
                });
            }
        });

        tcpClient->AsTcpClient();
        clients.push_back(tcpClient);
    });

    if (tcpServer->AsTcpServer(mPort, HUI_SERVER_BACKLOG) != 0) {
        SPR_LOGE("Bind port %d failed\n", mPort);
        return -1;
    }

    SPR_LOGI("HTTP UI Server started on port %d\n", mPort);
    pEpoll->EpollLoop();
    return 0;
}

void HUIServer::Stop()
{
    mRunning = false;
}
