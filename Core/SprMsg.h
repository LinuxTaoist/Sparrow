/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMsg.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
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
    MSG_TYPE_I8VALUE,
    MSG_TYPE_U16VALUE,
    MSG_TYPE_I16VALUE,
    MSG_TYPE_U32VALUE,
    MSG_TYPE_I32VALUE,
    MSG_TYPE_U64VALUE,
    MSG_TYPE_I64VALUE,
    MSG_TYPE_STRING,
    MSG_TYPE_U8VEC,
    MSG_TYPE_I8VEC,
    MSG_TYPE_U16VEC,
    MSG_TYPE_I16VEC,
    MSG_TYPE_U32VEC,
    MSG_TYPE_I32VEC,
    MSG_TYPE_U64VEC,
    MSG_TYPE_I64VEC,
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
    SprMsg(uint32_t to, const SprMsg& srcMsg);
    SprMsg(uint32_t from, uint32_t to, uint32_t msgId);
    explicit SprMsg(std::string datas);
    SprMsg& operator=(const SprMsg &srcMsg); // Assignment Operator
    int32_t CopyMsg(const SprMsg& srcMsg);

    void    Clear();
    int32_t Decode(std::string& deDatas);
    int32_t Encode(std::string& enDatas) const;
    void    SetSize(int32_t size) { mSize = size; }
    int32_t GetSize() const { return mSize; }
    std::string DumpBrief();
    std::string DumpDetails();

    void SetFrom(uint32_t from);
    void SetTo(uint32_t to);
    void SetMsgId(uint32_t msgId);
    void SetBoolValue(bool value);
    void SetU8Value(uint8_t value);
    void SetI8Value(int8_t value);
    void SetU16Value(uint16_t value);
    void SetI16Value(int16_t value);
    void SetU32Value(uint32_t value);
    void SetI32Value(int32_t value);
    void SetU64Value(uint64_t value);
    void SetI64Value(int64_t value);
    void SetString(const std::string& str);
    void SetU8Vec(const std::vector<uint8_t>& vec);
    void SetI8Vec(const std::vector<int8_t>& vec);
    void SetU16Vec(const std::vector<uint16_t>& vec);
    void SetI16Vec(const std::vector<int16_t>& vec);
    void SetU32Vec(const std::vector<uint32_t>& vec);
    void SetI32Vec(const std::vector<int32_t>& vec);
    void SetU64Vec(const std::vector<uint64_t>& vec);
    void SetI64Vec(const std::vector<int64_t>& vec);

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
    int8_t      GetI8Value()    const { return mI8Value; }
    uint16_t    GetU16Value()   const { return mU16Value; }
    int16_t     GetI16Value()   const { return mI16Value; }
    uint32_t    GetU32Value()   const { return mU32Value; }
    int32_t     GetI32Value()   const { return mI32Value; }
    uint64_t    GetU64Value()   const { return mU64Value; }
    int64_t     GetI64Value()   const { return mI64Value; }
    std::string GetString()     const { return mString; }
    std::vector<uint8_t>    GetU8Vec()  const { return mU8Vec;  }
    std::vector<int8_t>     GetI8Vec()  const { return mI8Vec;  }
    std::vector<uint16_t>   GetU16Vec() const { return mU16Vec; }
    std::vector<int16_t>    GetI16Vec() const { return mI16Vec; }
    std::vector<uint32_t>   GetU32Vec() const { return mU32Vec; }
    std::vector<int32_t>    GetI32Vec() const { return mI32Vec; }
    std::vector<uint64_t>   GetU64Vec() const { return mU64Vec; }
    std::vector<int64_t>    GetI64Vec() const { return mI64Vec; }

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
    void InitCodecFuncs();
    void ResetMembers();
    int32_t EncodeFrom(std::string& enDatas) const;
    int32_t EncodeTo(std::string& enDatas) const;
    int32_t EncodeMsgId(std::string& enDatas) const;
    int32_t EncodeTag(std::string& enDatas) const;

    // 编解码函数声明
    int32_t EncodeBoolValue(std::string& enDatas);
    int32_t EncodeU8Value(std::string& enDatas);
    int32_t EncodeI8Value(std::string& enDatas);
    int32_t EncodeU16Value(std::string& enDatas);
    int32_t EncodeI16Value(std::string& enDatas);
    int32_t EncodeU32Value(std::string& enDatas);
    int32_t EncodeI32Value(std::string& enDatas);
    int32_t EncodeU64Value(std::string& enDatas);
    int32_t EncodeI64Value(std::string& enDatas);
    int32_t EncodeU8Vec(std::string& enDatas);
    int32_t EncodeI8Vec(std::string& enDatas);
    int32_t EncodeU16Vec(std::string& enDatas);
    int32_t EncodeI16Vec(std::string& enDatas);
    int32_t EncodeU32Vec(std::string& enDatas);
    int32_t EncodeI32Vec(std::string& enDatas);
    int32_t EncodeU64Vec(std::string& enDatas);
    int32_t EncodeI64Vec(std::string& enDatas);
    int32_t EncodeString(std::string& enDatas);
    int32_t EncodeDatas(std::string& enDatas);

    int32_t DecodeFrom(std::string& deDatas);
    int32_t DecodeTo(std::string& deDatas);
    int32_t DecodeMsgId(std::string& deDatas);
    int32_t DecodeTag(std::string& deDatas);
    int32_t DecodeBoolValue(std::string& deDatas);
    int32_t DecodeU8Value(std::string& deDatas);
    int32_t DecodeI8Value(std::string& deDatas);
    int32_t DecodeU16Value(std::string& deDatas);
    int32_t DecodeI16Value(std::string& deDatas);
    int32_t DecodeU32Value(std::string& deDatas);
    int32_t DecodeI32Value(std::string& deDatas);
    int32_t DecodeU64Value(std::string& deDatas);
    int32_t DecodeI64Value(std::string& deDatas);
    int32_t DecodeU8Vec(std::string& deDatas);
    int32_t DecodeI8Vec(std::string& deDatas);
    int32_t DecodeU16Vec(std::string& deDatas);
    int32_t DecodeI16Vec(std::string& deDatas);
    int32_t DecodeU32Vec(std::string& deDatas);
    int32_t DecodeI32Vec(std::string& deDatas);
    int32_t DecodeU64Vec(std::string& deDatas);
    int32_t DecodeI64Vec(std::string& deDatas);
    int32_t DecodeString(std::string& deDatas);
    int32_t DecodeDatas(std::string& deDatas);

    std::string DumpMemberString(ESprMsgType type, uint32_t limitLen) const;

private:
    int32_t                 mSize;
    uint32_t                mFrom;
    uint32_t                mTo;
    uint32_t                mMsgId;
    uint32_t                mTag;
    bool                    mBoolValue;
    uint8_t                 mU8Value;
    int8_t                  mI8Value;
    uint16_t                mU16Value;
    int16_t                 mI16Value;
    uint32_t                mU32Value;
    int32_t                 mI32Value;
    uint64_t                mU64Value;
    int64_t                 mI64Value;
    uint32_t                mStringLength;
    std::string             mString;
    uint32_t                mU8VecLength;
    std::vector<uint8_t>    mU8Vec;
    uint32_t                mI8VecLength;
    std::vector<int8_t>     mI8Vec;
    uint32_t                mU16VecLength;
    std::vector<uint16_t>   mU16Vec;
    uint32_t                mI16VecLength;
    std::vector<int16_t>    mI16Vec;
    uint32_t                mU32VecLength;
    std::vector<uint32_t>   mU32Vec;
    uint32_t                mI32VecLength;
    std::vector<int32_t>    mI32Vec;
    uint32_t                mU64VecLength;
    std::vector<uint64_t>   mU64Vec;
    uint32_t                mI64VecLength;
    std::vector<int64_t>    mI64Vec;
    uint32_t                mDataSize;
    std::vector<uint8_t>    mDatas;

    using CodecFunction = int32_t (SprMsg::*)(std::string&);
    std::map<ESprMsgType, CodecFunction> mEnFuncs;
    std::map<ESprMsgType, CodecFunction> mDeFuncs;
};

#endif // __SPR_MSG_H__
