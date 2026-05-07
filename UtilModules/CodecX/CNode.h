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
    explicit CNode(const std::shared_ptr<CNode>& pParent, bool isField = true);
    CNode(const CNode& node);
    CNode& operator = (const CNode& node);
    CNode(CNode&& node) = delete;
    CNode& operator = (CNode&& node) = delete;
    virtual ~CNode();

    void    SetField(bool isField);
    bool    IsField();
    void    ResetDePos(int32_t val = 0);
    void    DePosAdd(int32_t offset);
    int32_t GetDePos();
    void    ResetEnPos(int32_t val = 0);
    void    EnPosAdd(int32_t offset);
    int32_t GetEnPos();
    void    SetSIndex(int32_t sIndex);
    int32_t GetSIndex();
    void    SetEIndex(int32_t eIndex);
    int32_t GetEIndex();
    void    SetLength(int32_t length);
    int32_t GetLength();
    void    SetEndian(CEndianType endian);
    int32_t GetEndian();
    void    SetName(const std::string& name);
    std::string GetName();
    void    SetType(const std::string& type);
    std::string GetType();

    virtual std::shared_ptr<CNode> Clone() = 0;
    virtual int32_t Decode(const std::vector<uint8_t>& bytes) = 0;
    virtual int32_t Encode(std::vector<uint8_t>& bytes) = 0;

protected:
    bool mIsField;
    int32_t mDePos;
    int32_t mEnPos;
    int32_t mSIndex;
    int32_t mEIndex;
    int32_t mLength;
    CEndianType mEndian;
    std::string mName;
    std::string mType;
    std::shared_ptr<CNode> mParentNode;
};

#endif // __CNODE_H__
