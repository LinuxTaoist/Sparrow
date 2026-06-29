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
#include "CUtils.h"
#include "CDefine.h"

class CNode : public std::enable_shared_from_this<CNode> {
public:
    explicit CNode(const std::shared_ptr<CNode>& pParent, bool isField = true);
    CNode(const CNode& node);
    CNode& operator = (const CNode& node);
    CNode(CNode&& node) = delete;
    CNode& operator = (CNode&& node) = delete;
    virtual ~CNode();

    bool IsField();
    void SetName(const std::string& name);
    std::string GetName();
    void SetType(const std::string& type);
    std::string GetType();
    std::shared_ptr<CNode> GetParentNode();
    std::shared_ptr<CNode> GetRootNode();
    void SetParentNode(const std::shared_ptr<CNode>& pParent);

    virtual std::shared_ptr<CNode> Clone() = 0;
    virtual std::shared_ptr<CNode> GetNodeByPath(const std::string& path) = 0;
    virtual int32_t Decode(const std::vector<uint8_t>& bytes) = 0;
    virtual int32_t Encode(std::vector<uint8_t>& bytes) = 0;

    int32_t SetStrValue(const std::string& name, const std::string& value);
    int32_t GetStrValue(const std::string& name, std::string& value);

    template <typename T>
    int32_t SetIntValue(const std::string& name, T value) {
        std::vector<uint8_t> tmpValue;
        int32_t ret = CUtils::IToV(value, tmpValue);
        if (ret != -1) {
            ret = SetValue(name, tmpValue);
        }

        return ret;
    }

    template <typename T>
    int32_t GetIntValue(const std::string& name, T& value) {
        std::vector<uint8_t> tmpValue;
        int32_t ret = GetValue(name, tmpValue);
        if (ret != -1) {
            ret = CUtils::VToI(tmpValue, value);
        }

        return ret;
    }

    template <typename T>
    int32_t SetVecValue(const std::string& name, const std::vector<T>& value) {
        std::vector<uint8_t> tmpValue;
        int32_t ret = CUtils::VToV(value, tmpValue);
        if (ret != -1) {
            ret = SetValue(name, tmpValue);
        }

        return ret;
    }

    template <typename T>
    int32_t GetVecValue(const std::string& name, std::vector<T>& value) {
        std::vector<uint8_t> tmpValue;
        int32_t ret = GetValue(name, tmpValue);
        if (ret != -1) {
            ret = CUtils::VToV(tmpValue, value);
        }
        return ret;
    }

protected:
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

    virtual int32_t SetValue(const std::string& name, const std::vector<uint8_t>& value) = 0;
    virtual int32_t GetValue(const std::string& name, std::vector<uint8_t>& value) = 0;

private:
    bool mIsField;
    static int32_t mDePos;
    static int32_t mEnPos;
    int32_t mSIndex;
    int32_t mEIndex;
    int32_t mLength;
    CEndianType mEndian;
    std::string mName;
    std::string mType;
    std::weak_ptr<CNode> mParentNode;
};

#endif // __CNODE_H__
