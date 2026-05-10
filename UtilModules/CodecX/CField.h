/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CField.h
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
#ifndef __CFIELD_H__
#define __CFIELD_H__

#include "CNode.h"

class CField : public CNode {
public:
    explicit CField(const std::shared_ptr<CNode>& parent);
    CField(const CField& field);
    CField& operator = (const CField& field);
    CField(CField&& field) = delete;
    CField& operator = (CField&& field) = delete;
    ~CField();

    void SetLenReference(const std::string& lenReference);
    std::string GetLenReference();
    void SetLenMode(const std::string& lenMode);
    std::string GetLenMode();
    void SetLenFormula(const std::string& lenFormula);
    std::string GetLenFormula();
    void SetChildNodesTag(const std::string& childNodesTag);
    std::string GetChildNodesTag();
    int32_t AddNode(const std::shared_ptr<CNode>& node);
    int32_t DelNode(const std::shared_ptr<CNode>& node);
    std::shared_ptr<CNode> GetNode(const std::string& name);
    std::vector<std::shared_ptr<CNode>> GetChildNodes();

    std::shared_ptr<CNode> Clone();
    int32_t Decode(const std::vector<uint8_t>& bytes) override;
    int32_t Encode(std::vector<uint8_t>& bytes) override;

private:
    void RelinkChildren(const std::shared_ptr<CField>& pThisField);
    int32_t CalculateDynamicFieldSize();
    int32_t DecodeStaticField(const std::vector<uint8_t>& bytes);
    int32_t DecodeDynamicField(const std::vector<uint8_t>& bytes);

private:
    std::string mLenReference;  // 容量引用
    std::string mLenMode;       // 容量模式
    std::string mLenFormula;    // 容量公式
    std::string mChildNodesTag; // 子节点容器标签
    std::vector<std::shared_ptr<CNode>> mChildNodes;
};

#endif // __CFIELD_H__
