/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CField.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/12/19
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/12/19 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <algorithm>
#include <string.h>
#include "CLog.h"
#include "CDefine.h"
#include "CAtom.h"
#include "CField.h"

#define CLOG_TAG "CField"

CField::CField(const std::shared_ptr<CNode>& parent)
    : CNode(parent, true) {
}

CField::CField(const CField& field)
    : CNode(field)
    , mLenReference(field.mLenReference)
    , mLenMode(field.mLenMode)
    , mLenFormula(field.mLenFormula)
    , mChildNodesTag(field.mChildNodesTag) {
    for (const auto& child : field.mChildNodes) {
        if (child) {
            mChildNodes.push_back(child->Clone());
        }
    }
}

CField& CField::operator = (const CField& field) {
    if (this == &field) {
        return *this;
    }

    CNode::operator = (field);
    mLenReference = field.mLenReference;
    mLenMode = field.mLenMode;
    mLenFormula = field.mLenFormula;
    mChildNodesTag = field.mChildNodesTag;

    mChildNodes.clear();
    for (const auto& child : field.mChildNodes) {
        if (child) {
            mChildNodes.push_back(child->Clone());
        }
    }

    return *this;
}

CField::~CField() {
}

void CField::SetLenReference(const std::string& lenReference) {
    mLenReference = lenReference;
}

std::string CField::GetLenReference() {
    return mLenReference;
}

void CField::SetLenMode(const std::string& lenMode) {
    mLenMode = lenMode;
}

std::string CField::GetLenMode() {
    return mLenMode;
}

void CField::SetLenFormula(const std::string& lenFormula) {
    mLenFormula = lenFormula;
}

std::string CField::GetLenFormula() {
    return mLenFormula;
}

void CField::SetChildNodesTag(const std::string& childNodesTag) {
    mChildNodesTag = childNodesTag;
}

std::string CField::GetChildNodesTag() {
    return mChildNodesTag;
}

int32_t CField::AddNode(const std::shared_ptr<CNode>& node) {
    mChildNodes.emplace_back(node);
    return 0;
}

int32_t CField::DelNode(const std::shared_ptr<CNode>& node) {
    std::string target = node->GetName();
    auto it = std::find_if(mChildNodes.begin(), mChildNodes.end(),
        [&](const std::shared_ptr<CNode>& ptr) {
        std::string name = ptr->GetName();
        return (name == target);
    });

    if (it != mChildNodes.end()) {
        mChildNodes.erase(it);
        return 0;
    }

    return -1;
}

std::shared_ptr<CNode> CField::GetNode(const std::string& name) {
    const auto it = std::find_if(mChildNodes.begin(), mChildNodes.end(),
        [&](const std::shared_ptr<CNode>& ptr) {
        return (ptr->GetName() == name);
    });

    std::shared_ptr<CNode> pNode = (it != mChildNodes.end()) ? (*it) : nullptr;
    return pNode;
}

std::vector<std::shared_ptr<CNode>> CField::GetChildNodes() {
    return mChildNodes;
}

std::shared_ptr<CNode> CField::Clone() {
    std::shared_ptr<CField> pClone = std::make_shared<CField>(*this);
    if (!pClone) {
        CLOGE("Node[%s] pClone is nullptr!\n", GetName().c_str());
        return nullptr;
    }

    pClone->RelinkChildren(pClone);
    return pClone;
}

std::shared_ptr<CNode> CField::GetNodeByPath(const std::string& path) {
    if (path.empty()) {
        CLOGE("Node[%s] path is empty!\n", GetName().c_str());
        return nullptr;
    }

    size_t start = 0;
    size_t end = path.find('/');
    std::vector<std::string> parts;
    while (end != std::string::npos) {
        std::string part = path.substr(start, end - start);
        if (!part.empty() && part != ".") { // 忽略空段和当前节点标记
            parts.push_back(part);
        }
        start = end + 1;
        end = path.find('/', start);
    }
    std::string lastPart = path.substr(start);
    if (!lastPart.empty() && lastPart != ".") {
        parts.push_back(lastPart);
    }

    if (parts.empty()) {
        CLOGE("Node[%s] path is empty!\n", GetName().c_str());
        return nullptr;
    }

    std::shared_ptr<CNode> pNode = nullptr;
    for (const std::string& part : parts) {
        if (part == "..") {
            pNode = pNode ? pNode->GetParentNode() : GetParentNode();
        } else {
            if (pNode) {
                std::shared_ptr<CField> pFieldNode = std::dynamic_pointer_cast<CField>(pNode);
                if (!pFieldNode) {
                    CLOGE("Node[%s] is not a container, cannot find child: %s\n",
                          pNode->GetName().c_str(), part.c_str());
                    return nullptr;
                }
                pNode = pFieldNode->GetNode(part);
            } else {
                pNode = GetNode(part);
            }
        }

        if (!pNode) {
            CLOGE("Node[%s] Node found: %s in path %s\n", GetName().c_str(), part.c_str(), path.c_str());
            return nullptr;
        }
    }

    return pNode;
}

void CField::RelinkChildren(const std::shared_ptr<CField>& pParentField) {
    for (auto& child : mChildNodes) {
        if (!child) {
            continue;
        }

        child->SetParentNode(pParentField);
        std::shared_ptr<CField> pChildField = std::dynamic_pointer_cast<CField>(child);
        if (pChildField) {
            pChildField->RelinkChildren(pChildField);
        }
    }
}

int32_t CField::DecodeStaticField(const std::vector<uint8_t>& bytes) {
    int32_t ret = 0;

    // CLOGD("Node[%s] Decode static field, size = %d \n", GetName().c_str(), (int32_t)mChildNodes.size());
    for (auto& node : mChildNodes) {
        int32_t len = node->Decode(bytes);
        if (len < 0) {
            CLOGE("Node[%s] Decode failed, ret = %d\n", node->GetName().c_str(), len);
            return -1;
        }
    }

    return ret;
}

int32_t CField::CalculateDynamicFieldSize() {
    int32_t len = 0;
    std::string lenRef = GetLenReference();
    std::string lenMode = GetLenMode();
    std::string lenFormula = GetLenFormula();
    if (lenRef.compare(0, strlen(TEXT_LEN_REF_FIXED), TEXT_LEN_REF_FIXED) == 0) {
        // 模式1: 固定长度模式 (len_ref = "fixed_N")
        // 配置示例:
        // {
        //     "name": "msg_time",
        //     "type": "dynamic_field",
        //     "len_ref": "fixed_8",
        //     "len_mode": "bytes",
        //     "child_template": {"name": " ", "type": "u8"}
        // }
        len = atoi(lenRef.substr(strlen(TEXT_LEN_REF_FIXED)).c_str());
        if (len < 0) {
            CLOGE("Node[%s] Invalid fixed value: %s\n", GetName().c_str(), lenRef.c_str());
            return -1;
        }
    } else {
        // 模式2: 引用其他节点长度模式
        std::shared_ptr<CNode> pLenNode = GetNodeByPath(lenRef);
        if (!pLenNode) {
            CLOGE("Node[%s] pLenNode is nullptr!\n", GetName().c_str());
            return -1;
        }

        if (pLenNode->IsField()) {
            // 子模式2.1: 引用Filed节点 (计算其所有子节点数值之和)
            // 配置示例:
            // {"name": "bms_count", "type": "u8"},
            // {
            //     "name": "bms_pack_counts",
            //     "type": "dynamic_field",
            //     "len_ref": "bms_count",
            //     "len_mode": "bytes",
            //     "child_template": {"name": "pack_count", "type": "u8"}
            // },
            // {
            //     "name": "battery_pack_codes",
            //     "type": "dynamic_field",
            //     "len_ref": "bms_pack_counts",
            //     "len_mode": "bytes",
            //     "len_formula": "len_ref * 24",
            //     "child_template": {"name": "pack_code", "type": "u8"}
            // }
            std::shared_ptr<CField> pLenField = std::dynamic_pointer_cast<CField>(pLenNode);
            int32_t ret = pLenField->CalculateChildNodesValueSum(len);
            if (ret == -1) {
                CLOGE("Node[%s] CalculateChildNodesValueSum failed!\n", pLenField->GetName().c_str());
                return -1;
            }
        } else {
            // 子模式2.2: 引用Atom节点 (直接获取数值)
            // 配置示例:
            // {"name": "vin_len", "type": "u16"},
            // {
            //     "name": "vin_code",
            //     "type": "dynamic_field",
            //     "len_ref": "vin_len",
            //     "len_mode": "bytes",
            //     "child_template": {"name": " ", "type": "u8"}
            // }
            std::shared_ptr<CAtom> pLenAtom = std::dynamic_pointer_cast<CAtom>(pLenNode);
            if (!pLenAtom) {
                CLOGE("Node[%s] pLenAtom is nullptr!\n", GetName().c_str());
                return -1;
            }

            int32_t ret = pLenAtom->GetIntValue(len);
            if (ret == -1) {
                CLOGE("Node[%s] GetIntValue failed!\n", pLenAtom->GetName().c_str());
                return -1;
            }
        }
    }

    if (lenMode == TEXT_LEN_MODE_BIT) {
        len = (len + 7) / 8;
    } else if (lenMode == TEXT_LEN_MODE_CONDITION) {
        len = (len == 0) ? 0 : 1;
    }

    if (!lenFormula.empty()) {
        if (lenFormula == TEXT_LEN_FORMULA_INVERT) {    // len_formula = "!"
            len = !len;
        } else {
            std::string numExpr;
            size_t pos = 0;
            while (pos < lenFormula.size()) {
                int32_t curPosTextLen = strlen(TEXT_LEN_FORMULA_CURPOS);
                int32_t lenRefTextLen = strlen(TEXT_LEN_REF_TAG);
                if (lenFormula.compare(pos, curPosTextLen, TEXT_LEN_FORMULA_CURPOS) == 0) {
                    numExpr += std::to_string(GetDePos());
                    pos += curPosTextLen;
                } else if (lenFormula.compare(pos, lenRefTextLen, TEXT_LEN_REF_TAG) == 0) {
                    numExpr += std::to_string(len);
                    pos += lenRefTextLen;
                } else {
                    numExpr += lenFormula[pos];
                    pos++;
                }
            }

            int32_t exprResult = 0;
            int32_t ret = CUtils::CalculateFromString(numExpr, exprResult);
            if (ret < 0 || exprResult < 0) {
                CLOGE("Node[%s] Calculate failed! [%s -> %s] ret = %d, result = %d\n",
                        GetName().c_str(), lenFormula.c_str(), numExpr.c_str(), ret, exprResult);
                return -1;
            }
            len = exprResult;
        }
    }

    return len;
}

int32_t CField::CalculateChildNodesValueSum(int32_t& sum) {
    for (auto& childNode : mChildNodes) {
        std::shared_ptr<CAtom> pAtom = std::dynamic_pointer_cast<CAtom>(childNode);
        if (!pAtom) {
            continue;
        }

        int32_t value = 0;
        int32_t ret = pAtom->GetIntValue(value);
        if (ret == -1) {
            CLOGE("Node[%s] GetIntValue failed!\n", pAtom->GetName().c_str());
            return -1;
        }
        sum += value;
    }

    return 0;
}

int32_t CField::DecodeDynamicField(const std::vector<uint8_t>& bytes) {
    if (mChildNodes.empty()) {
        CLOGE("Node[%s] mChildNodes is empty!\n", GetName().c_str());
        return -1;
    }

    int32_t count = CalculateDynamicFieldSize();
    if (count < 0) {
        CLOGE("Node[%s] CalculateDynamicFieldSize failed!\n", GetName().c_str());
        return -1;
    }

    // CLOGD("Node[%s] Decode dynamic field, count = %d \n", GetName().c_str(), count);
    int32_t ret = 0;
    std::shared_ptr<CNode> pTmpNode = mChildNodes[0];
    mChildNodes.clear();
    for (int i = 0; i < count; i++) {
        std::shared_ptr<CNode> pCloneNode = pTmpNode->Clone();
        int32_t len = pCloneNode->Decode(bytes);
        if (len == -1) {
            CLOGE("Node[%s] Decode failed!\n", GetName().c_str());
            return -1;
        }

        mChildNodes.emplace_back(pCloneNode);
        ret += len;
    }

    return ret;
}

int32_t CField::Decode(const std::vector<uint8_t>& bytes) {
    int32_t ret = 0;

    // 首次解码，重置起始位置
    if (!GetParentNode()) {
        ResetDePos();
    }

    if (GetType() == TEXT_TYPE_DFIELD) {
        ret = DecodeDynamicField(bytes);
    } else if (GetType() == TEXT_TYPE_SFIELD) {
        ret = DecodeStaticField(bytes);
    } else {
        CLOGE("Node[%s] Invalid type %s!\n", GetName().c_str(), GetType().c_str());
    }

    return ret;
}

int32_t CField::Encode(std::vector<uint8_t>& bytes) {
    int32_t ret = 0;

    // 首次编码，重置起始位置
    if (!GetParentNode()) {
        ResetEnPos();
    }

    for (auto& node : mChildNodes) {
        int32_t len = node->Encode(bytes);
        if (len < 0) {
            CLOGE("Node[%s] Encode failed! len = %d\n", node->GetName().c_str(), len);
            break;
        }

        ret += len;
    }

    return ret;
}
