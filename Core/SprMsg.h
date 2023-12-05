/**
 *------------------------------------------------------------------------------
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
 *------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *------------------------------------------------------------------------------
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
    MSG_TYPE_MIN,
    MSG_TYPE_U8VALUE = 1,
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
    SprMsg(uint32_t msgId);
    SprMsg(std::string datas);
    void clear();

    void setMsgId(uint32_t msgId);
    void setU8Value(uint8_t value);
    void setU16Value(uint16_t value);
    void setU32Value(uint32_t value);
    void setString(const std::string& str);
    void setU8Vec(const std::vector<uint8_t>& vec);
    void setU32Vec(const std::vector<uint32_t>& vec);
    template<typename T>
    void setDatas(std::shared_ptr<T> datas, uint32_t size) {
        mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_PTR);
        mDataSize = size;
        const uint8_t* pData = reinterpret_cast<const uint8_t*>(datas.get());
        mDatas.assign(pData, pData + size);
    }

    uint32_t getMsgId() { return mMsgId; }
    uint8_t getU8Value() { return mU8Value; }
    uint16_t getU16Value() { return mU16Value; }
    uint32_t getU32Value() { return mU32Value; }
    std::string getString() { return mString; }
    std::vector<uint8_t> getU8Vec() { return mU8Vec; }
    std::vector<uint32_t> getU32Vec() { return mU32Vec; }

    template<typename T>
    std::shared_ptr<T> getDatas() {
        if (mDatas.size() < sizeof(T)) {
            return nullptr;
        }
        std::shared_ptr<T> pData = std::make_shared<T>(*reinterpret_cast<T*>(mDatas.data()));
        return pData;
    }

    int8_t decode(std::string& deDatas);
    int8_t encode(std::string& enDatas) const;

private:
    uint32_t mMsgId;
    uint32_t mTag;
    uint8_t mU8Value;
    uint16_t mU16Value;
    uint32_t mU32Value;
    uint32_t mStringLength;
    std::string mString;
    uint32_t mU8VecLength;
    std::vector<uint8_t> mU8Vec;
    uint32_t mU32VecLength;
    std::vector<uint32_t> mU32Vec;
    uint32_t mDataSize;
    std::vector<uint8_t> mDatas;    // ptr serialization

    using CodecFunction = void (SprMsg::*)(std::string&);
    std::map<ESprMsgType, CodecFunction> mEnFuncs;
    std::map<ESprMsgType, CodecFunction> mDeFuncs;

    void init();
    void encodeMsgId(std::string& enDatas) const;
    void encodeTag(std::string& enDatas) const;
    void encodeU8Value(std::string& enDatas);
    void encodeU16Value(std::string& enDatas);
    void encodeU32Value(std::string& enDatas);
    void encodeString(std::string& enDatas);
    void encodeU8Vec(std::string& enDatas);
    void encodeU32Vec(std::string& enDatas);
    void encodeDatas(std::string& enDatas);

    void decodeMsgId(std::string& deDatas);
    void decodeTag(std::string& deDatas);
    void decodeU8Value(std::string& deDatas);
    void decodeU16Value(std::string& deDatas);
    void decodeU32Value(std::string& deDatas);
    void decodeString(std::string& deDatas);
    void decodeU8Vec(std::string& deDatas);
    void decodeU32Vec(std::string& deDatas);
    void decodeDatas(std::string& deDatas);
};

#endif
