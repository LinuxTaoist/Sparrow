/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMsg.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2023/11/25
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __SPR_MSG_H__
#define __SPR_MSG_H__

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <stdint.h>

enum class ESprMsgType : uint32_t
{
    MSG_TYPE_MIN  = 0,
    MSG_TYPE_BOOLVALUE,
    MSG_TYPE_U8VALUE,
    MSG_TYPE_U16VALUE,
    MSG_TYPE_U32VALUE,
    MSG_TYPE_STRING,
    MSG_TYPE_U8VEC,
    MSG_TYPE_U32VEC,
    MSG_TYPE_PTR,
    MSG_TYPE_MAX
};

class SprMsg
{
public:
    SprMsg();
    SprMsg(const SprMsg& srcMsg);
    explicit SprMsg(uint32_t msgId);
    SprMsg(uint32_t to, uint32_t msgId);
    SprMsg(uint32_t from, uint32_t to, uint32_t msgId);
    explicit SprMsg(std::string datas);
    SprMsg& operator=(const SprMsg &srcMsg); // Assignment Operator
    int CopyMsg(const SprMsg& srcMsg);

    void    Clear();
    int8_t  Decode(std::string& deDatas);
    int8_t  Encode(std::string& enDatas) const;

    void SetFrom(uint32_t from);
    void SetTo(uint32_t to);
    void SetMsgId(uint32_t msgId);
    void SetBoolValue(bool value);
    void SetU8Value(uint8_t value);
    void SetU16Value(uint16_t value);
    void SetU32Value(uint32_t value);
    void SetString(const std::string& str);
    void SetU8Vec(const std::vector<uint8_t>& vec);
    void SetU32Vec(const std::vector<uint32_t>& vec);

    template<typename T>
    void SetDatas(std::shared_ptr<T> datas, uint32_t size) {
        mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_PTR);
        mDataSize = size;
        const uint8_t* pData = reinterpret_cast<const uint8_t*>(datas.get());
        mDatas.assign(pData, pData + size);
    }

    uint32_t    GetFrom()       const { return mFrom; }
    uint32_t    GetTo()         const { return mTo; }
    uint32_t    GetMsgId()      const { return mMsgId; }
    bool        GetBoolValue()  const { return mBoolValue; }
    uint8_t     GetU8Value()    const { return mU8Value; }
    uint16_t    GetU16Value()   const { return mU16Value; }
    uint32_t    GetU32Value()   const { return mU32Value; }
    std::string GetString()     const { return mString; }
    std::vector<uint8_t>    GetU8Vec()  const { return mU8Vec;  }
    std::vector<uint32_t>   GetU32Vec() const { return mU32Vec; }

    template<typename T>
    std::shared_ptr<T> GetDatas() const {
        if (mDatas.size() < sizeof(T)) {
            return nullptr;
        }

        std::shared_ptr<T> pData = std::make_shared<T>(*const_cast<T*>(reinterpret_cast<const T*>(mDatas.data())));
        return pData;
    }

private:
    void Init();
    void EncodeFrom(std::string& enDatas) const;
    void EncodeTo(std::string& enDatas) const;
    void EncodeMsgId(std::string& enDatas) const;
    void EncodeTag(std::string& enDatas) const;
    void EncodeBoolValue(std::string& enDatas);
    void EncodeU8Value(std::string& enDatas);
    void EncodeU16Value(std::string& enDatas);
    void EncodeU32Value(std::string& enDatas);
    void EncodeString(std::string& enDatas);
    void EncodeU8Vec(std::string& enDatas);
    void EncodeU32Vec(std::string& enDatas);
    void EncodeDatas(std::string& enDatas);

    void DecodeFrom(std::string& deDatas);
    void DecodeTo(std::string& deDatas);
    void DecodeMsgId(std::string& deDatas);
    void DecodeTag(std::string& deDatas);
    void DecodeBoolValue(std::string& deDatas);
    void DecodeU8Value(std::string& deDatas);
    void DecodeU16Value(std::string& deDatas);
    void DecodeU32Value(std::string& deDatas);
    void DecodeString(std::string& deDatas);
    void DecodeU8Vec(std::string& deDatas);
    void DecodeU32Vec(std::string& deDatas);
    void DecodeDatas(std::string& deDatas);

private:
    uint32_t                mFrom;
    uint32_t                mTo;
    uint32_t                mMsgId;
    uint32_t                mTag;
    bool                    mBoolValue;
    uint8_t                 mU8Value;
    uint16_t                mU16Value;
    uint32_t                mU32Value;
    uint32_t                mStringLength;
    std::string             mString;
    uint32_t                mU8VecLength;
    std::vector<uint8_t>    mU8Vec;
    uint32_t                mU32VecLength;
    std::vector<uint32_t>   mU32Vec;
    uint32_t                mDataSize;
    std::vector<uint8_t>    mDatas;

    using CodecFunction = void (SprMsg::*)(std::string&);
    std::map<ESprMsgType, CodecFunction> mEnFuncs;
    std::map<ESprMsgType, CodecFunction> mDeFuncs;
};

#endif
