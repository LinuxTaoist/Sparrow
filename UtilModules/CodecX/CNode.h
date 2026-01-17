/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CNode.h
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
#ifndef __CNODE_H__
#define __CNODE_H__

#include <string>
#include <vector>
#include <memory>
#include "CDefine.h"

class CNode {
public:
    CNode(bool isField = true);
    CNode(const CNode& node);
    CNode& operator=(const CNode& node);
    CNode(CNode&& node);
    CNode& operator=(CNode&& node);
    virtual ~CNode();

    bool IsField();
    int32_t SetSIndex(int32_t sIndex);
    int32_t GetSIndex(int32_t& sIndex);
    int32_t SetEIndex(int32_t eIndex);
    int32_t GetEIndex(int32_t& eIndex);
    int32_t SetLength(int32_t length);
    int32_t GetLength(int32_t& length);
    int32_t SetEndian(CEndianType endian);
    int32_t GetEndian(CEndianType& endian);
    int32_t SetName(const std::string& name);
    int32_t GetName(std::string& name);
    int32_t SetText(const std::string& text);
    int32_t GetText(std::string& text);
    int32_t AddNode(const std::shared_ptr<CNode>& node);
    int32_t DelNode(const std::shared_ptr<CNode>& node);

    virtual std::shared_ptr<CNode> Clone() = 0;
    virtual int32_t Decode(const std::vector<uint8_t>& bytes) = 0;
    virtual int32_t Encode(std::vector<uint8_t>& bytes) = 0;

protected:
    bool mIsField;
    int32_t mSIndex;
    int32_t mEIndex;
    int32_t mLength;
    CEndianType mEndian;
    std::string mName;
    std::string mText;
    std::vector<std::shared_ptr<CNode>> mChildNodes;
};

#endif // __CNODE_H__
