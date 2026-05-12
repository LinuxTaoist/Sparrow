#include <fstream>
#include <errno.h>
#include <string.h>
#include <algorithm>
#include "CLog.h"
#include "CAtom.h"
#include "CField.h"
#include "CUtils.h"
#include "CFactory.h"
#include "CJConfigParser.h"

#define CLOG_TAG "CFactory"

CFactory::CFactory() {

}

CFactory::~CFactory() {

}

CFactory& CFactory::GetInstance() {
    static CFactory factory;
    return factory;
}

std::shared_ptr<CNode> CFactory::CreateCfgParserByCfgFile(const std::string& cfgPath) {
    std::string text;
    int32_t ret = CUtils::ReadFile(cfgPath, text);
    if (ret <= 0) {
        return nullptr;
    }

    return CreateCfgParserByCfgString(text);
}

std::shared_ptr<CNode> CFactory::CreateCfgParserByCfgString(const std::string& cfgString) {
    CJConfigParser theCfgParser(cfgString);
    return theCfgParser.CJsonToNode();
}

std::shared_ptr<CNode> CFactory::CreateDataParserByCfgParser(const std::shared_ptr<CNode>& pCfgParser, const std::vector<uint8_t>& bytes) {
    std::shared_ptr<CNode> pDataParser = pCfgParser->Clone();
    if (!pDataParser) {
        CLOGE("Create parser failed!\n");
        return nullptr;
    }

    int32_t ret = pDataParser->Decode(bytes);
    return (ret == -1) ? nullptr : pDataParser;
}

static void PrintCfgNode(const std::shared_ptr<CNode>& pNode, int level) {
    if (!pNode) {
        CLOGE("pNode is nullptr! \n");
        return;
    }

    std::string indent(level * 4, ' ');
    CLOGI("%s - name    : %s\n", indent.c_str(), pNode->GetName().c_str());
    CLOGI("%s - type    : %s\n", indent.c_str(), pNode->GetType().c_str());
    if (!pNode->IsField()) {
        std::shared_ptr<CAtom> pAtom = std::dynamic_pointer_cast<CAtom>(pNode);
        CLOGI("%s - value   : %s\n", indent.c_str(), pAtom->DumpHexValue().c_str());
        return;
    }

    std::shared_ptr<CField> pField = std::dynamic_pointer_cast<CField>(pNode);
    if (pField->GetType() == TEXT_TYPE_DFIELD) {
        CLOGI("%s - len_ref : %s\n", indent.c_str(), pField->GetLenReference().c_str());
        CLOGI("%s - len_mode: %s\n", indent.c_str(), pField->GetLenMode().c_str());
        CLOGI("%s - len_formula: %s\n", indent.c_str(), pField->GetLenFormula().c_str());
    }

    std::vector<std::shared_ptr<CNode>> childNodes = pField->GetChildNodes();
    if (!childNodes.empty()) {
        CLOGI("%s - %s [%d]\n", indent.c_str(),
            pField->GetChildNodesTag().c_str(),
            static_cast<int32_t>(childNodes.size()));

        int32_t i = 0;
        for (auto& pChild : childNodes) {
            CLOGI("  %s - %d     \n", indent.c_str(), i++);
            PrintCfgNode(pChild, level + 1);
        }
    }
}

void CFactory::PrintConfigDetails(const std::shared_ptr<CNode>& pNode) {
    CLOGI("--------------  Print Config Details  -------------\n");
    PrintCfgNode(pNode, 0);
    CLOGI("---------------------------------------------------\n");
}

static void PrintDataNode(const std::shared_ptr<CNode>& pNode, int level, int& offset) {
    if (!pNode) {
        return;
    }

    const std::string indent(level * 4, ' ');
    const std::string& name = pNode->GetName();
    const std::string& type = pNode->GetType();
    int start = offset;

    if (!pNode->IsField()) {
        auto pAtom = std::dynamic_pointer_cast<CAtom>(pNode);
        std::string hex = pAtom->DumpHexValue();
        int size = hex.empty() ? 0 : (hex.size() + 1) / 3;

        CLOGI("%s[%02d-%02d] %s: %s\n", indent.c_str(), start, start+size-1, name.c_str(), hex.c_str());
        offset += size;
        return;
    }

    auto field = std::dynamic_pointer_cast<CField>(pNode);
    auto children = field->GetChildNodes();

    std::string lenMode = field->GetLenMode();
    if (lenMode == TEXT_LEN_MODE_BYTES) {
        const size_t TRUNCATE_THRESHOLD = 10;   // 超过此字节数自动截断
        const size_t LEADING_BYTES    = 4;      // 显示前N个字节
        const size_t TRAILING_BYTES   = 2;      // 显示后N个字节

        size_t totalBytes = children.size();
        std::string hexString;
        auto appendHexRange = [&](size_t start, size_t end) {
            for (size_t i = start; i < end; i++) {
                auto pAtom = std::dynamic_pointer_cast<CAtom>(children[i]);
                hexString += pAtom->DumpHexValue() + " ";
            }
        };

        if (totalBytes <= TRUNCATE_THRESHOLD) {
            appendHexRange(0, totalBytes);
        } else {
            appendHexRange(0, LEADING_BYTES);
            hexString += "... ";
            appendHexRange(totalBytes - TRAILING_BYTES, totalBytes);
            hexString += "(" + std::to_string(totalBytes) + " bytes)";
        }

        CLOGI("%s[%02d-%02d] %s: %s\n",
            indent.c_str(), start, (int32_t)(start + totalBytes - 1),
            name.c_str(), hexString.c_str());
        offset += totalBytes;
        return;
    }

    if (type == TEXT_TYPE_DFIELD) {
        CLOGI("%s%s[%zu]\n", indent.c_str(), name.c_str(), children.size());
        for (auto& child : children) {
            PrintDataNode(child, level + 1, offset);
        }
        return;
    }

    CLOGI("%s[%02d] %s\n", indent.c_str(), start, name.c_str());
    for (auto& child : children) {
        PrintDataNode(child, level + 1, offset);
    }
}

static int32_t PrintFrameDetails(const std::shared_ptr<CNode>& pFrame, int32_t frameNum, size_t offset) {
    if (!pFrame) {
        CLOGE("Frame is nullptr!\n");
        return -1;
    }

    int32_t frameSize = 0;
    CLOGI("\n");
    CLOGI("Frame %03d | Offset: %zu\n", frameNum, offset);
    CLOGI("---------------------------------------------------\n");
    PrintDataNode(pFrame, 0, frameSize);
    CLOGI("---------------------------------------------------\n");

    return frameSize;
}

bool CFactory::GetFrameHeader(const std::shared_ptr<CNode>& pCfgParser, std::vector<uint8_t>& headers) {
    if (!pCfgParser) {
        CLOGE("pCfgParser is nullptr!\n");
        return false;
    }

    std::shared_ptr<CField> pCfgRootField = std::dynamic_pointer_cast<CField>(pCfgParser);
    if (!pCfgRootField) {
        CLOGE("pCfgRootField is nullptr!\n");
        return false;
    }

    std::shared_ptr<CNode> pHeadFlagNode = pCfgRootField->GetNode(TEXT_HEAD_FLAG_TAG);
    if (!pHeadFlagNode || pHeadFlagNode->IsField()) {
        // CLOGW("pHeadFlagNode is invalid!\n");
        return false;
    }

    std::shared_ptr<CAtom> pHeadFlagAtom = std::dynamic_pointer_cast<CAtom>(pHeadFlagNode);
    if (!pHeadFlagAtom) {
        CLOGE("pHeadFlagAtom is nullptr!\n");
        return false;
    }

    std::vector<uint8_t> frameHeader;
    int32_t ret = pHeadFlagAtom->GetVecValue(frameHeader);
    if (ret < 0 || frameHeader.empty()) {
        CLOGE("Get frame header failed! ret = %d, size = %d\n", ret, (int32_t)frameHeader.size());
        return false;
    }

    headers.clear();
    headers.assign(frameHeader.begin(), frameHeader.end());
    return true;
}

void CFactory::PrintProtocolDetails(const std::shared_ptr<CNode>& pCfgParser, const std::vector<uint8_t>& bytes) {
    if (!pCfgParser || bytes.empty()) {
        CLOGE("Invalid param! pCfgParser = %p, bytes.size() = %d\n", pCfgParser.get(), (int32_t)bytes.size());
        return;
    }

    std::string protocolName = pCfgParser->GetName().empty() ? "customer" : pCfgParser->GetName();
    CLOGI("===================================================\n");
    CLOGI("Protocol Parsing | %s | Total: %zu bytes\n", protocolName.c_str(), bytes.size());
    CLOGI("===================================================\n");

    int32_t frameCount = 0;
    int32_t proccedCount = 0;
    std::vector<uint8_t> frameHeader;
    bool hasFrameHeader = GetFrameHeader(pCfgParser, frameHeader);
    if (hasFrameHeader && !frameHeader.empty()) {
        auto currentIt = bytes.begin();
        const auto endIt = bytes.end();
        const size_t headerLen = frameHeader.size();

        while (currentIt != endIt) {
            auto frameIt = std::search(currentIt, endIt, frameHeader.begin(), frameHeader.end());
            if (frameIt == endIt) {
                break;
            }

            size_t offset = std::distance(bytes.begin(), frameIt);
            size_t remaining = std::distance(frameIt, endIt);
            if (remaining < headerLen) {
                break;
            }

            std::vector<uint8_t> frameBuf(frameIt, endIt);
            std::shared_ptr<CNode> pFrame = CreateDataParserByCfgParser(pCfgParser, frameBuf);
            if (pFrame) {
                frameCount++;
                int32_t frameSize = PrintFrameDetails(pFrame, frameCount, offset);
                if (frameSize > 0 && static_cast<size_t>(frameSize) <= remaining) {
                    currentIt = frameIt + frameSize;
                } else {
                    CLOGD("Frame %d size invalid (%d bytes), skipping header\n", frameCount, frameSize);
                    currentIt = frameIt + headerLen;
                }
            }
        }

        proccedCount = (int32_t)std::distance(bytes.begin(), currentIt);
        if (currentIt != endIt) {
            CLOGI("Remaining unparsed bytes: %zu\n", std::distance(currentIt, endIt));
        }
    } else {
        std::shared_ptr<CNode> pFrame = CreateDataParserByCfgParser(pCfgParser, bytes);
        proccedCount = PrintFrameDetails(pFrame, 0, 0);
    }

    CLOGI("===================================================\n");
    CLOGI("Summary | Frame: %d | Processed: %d/%zu\n", frameCount, proccedCount, bytes.size());
    CLOGI("===================================================\n");
}

void CFactory::PrintProtocolDetails(const std::string& cfgPath, const std::string& bytesPath) {
    std::shared_ptr<CNode> pCfgParser = CreateCfgParserByCfgFile(cfgPath);
    if (!pCfgParser) {
        CLOGE("Load config failed: %s\n", cfgPath.c_str());
        return;
    }

    std::vector<uint8_t> hexBytes;
    int32_t ret = CUtils::ReadHexTextToHexVector(bytesPath, hexBytes);
    if (ret <= 0) {
        CLOGE("Read bytes failed: %s\n", bytesPath.c_str());
        return;
    }

    PrintProtocolDetails(pCfgParser, hexBytes);
}
