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

    int32_t SetSerial(int32_t serial);
    int32_t GetSerial(int32_t& serial);
    int32_t SetCapacity(int32_t capacity);
    int32_t GetCapacity(int32_t& capacity);
    int32_t AddNode(const std::shared_ptr<CNode>& node);
    int32_t DelNode(const std::shared_ptr<CNode>& node);
    int32_t GetNode(const std::string& name, std::shared_ptr<CNode>& node);

    std::shared_ptr<CNode> Clone();
    int32_t Decode(const std::vector<uint8_t>& bytes) override;
    int32_t Encode(std::vector<uint8_t>& bytes) override;

private:
    int32_t mSerial;            // 所在序列号，从0开始
    int32_t mCapacity;          // 节点容量
    std::string mLenReference;  // 容量引用
    std::vector<std::shared_ptr<CNode>> mChildNodes;
};

#endif // __CFIELD_H__
