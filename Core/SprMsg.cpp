/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMsg.cpp
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
#include "SprMsg.h"
#include "Util/Convert.h"

#define SPR_LOGD(fmt, args...) printf("%d SprMsg D: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%d SprMsg W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d SprMsg E: " fmt, __LINE__, ##args)

SprMsg::SprMsg()
{
    mTag = 0;
    mU8Value = 0;
    mU16Value = 0;
    mU32Value = 0;
    mDataSize = 0;

    mEnFuncs = {
        { ESprMsgType::MSG_TYPE_U8VALUE,     &SprMsg::encodeU8Value},
        { ESprMsgType::MSG_TYPE_U16VALUE,    &SprMsg::encodeU16Value},
        { ESprMsgType::MSG_TYPE_U32VALUE,    &SprMsg::encodeU32Value},
        { ESprMsgType::MSG_TYPE_U8VEC,       &SprMsg::encodeU8Vec},
        { ESprMsgType::MSG_TYPE_U32VEC,      &SprMsg::encodeU32Vec},
        { ESprMsgType::MSG_TYPE_PTR,         &SprMsg::encodeDatas},
    };

    mDeFuncs = {
        { ESprMsgType::MSG_TYPE_U8VALUE,     &SprMsg::decodeU8Value},
        { ESprMsgType::MSG_TYPE_U16VALUE,    &SprMsg::decodeU16Value},
        { ESprMsgType::MSG_TYPE_U32VALUE,    &SprMsg::decodeU32Value},
        { ESprMsgType::MSG_TYPE_U8VEC,       &SprMsg::decodeU8Vec},
        { ESprMsgType::MSG_TYPE_U32VEC,      &SprMsg::decodeU32Vec},
        { ESprMsgType::MSG_TYPE_PTR,         &SprMsg::decodeDatas},
    };
}

SprMsg::SprMsg(const char* buf, int size)
{
    SprMsg();
    decode(buf, size);
}

void SprMsg::clear()
{
    mTag = 0;
    mU8Vec.clear();
    mU32Vec.clear();
}

void SprMsg::setU8Value(uint8_t value)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_U8VALUE);
    mU8Value = value;
}

void SprMsg::setU16Value(uint16_t value)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_U16VALUE);
    mU16Value = value;
}

void SprMsg::setU32Value(uint32_t value)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_U32VALUE);
    mU32Value = value;
}

void SprMsg::setU8Vec(const std::vector<uint8_t>& vec)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_U8VEC);
    mU8Vec.assign(vec.begin(), vec.end());
}

void SprMsg::setU32Vec(const std::vector<uint32_t>& vec)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_U32VALUE);
    mU32Vec.assign(vec.begin(), vec.end());
}

void SprMsg::setDatas(std::shared_ptr<void> datas, uint32_t size)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_PTR);
    mDataSize = size;
    mDatas = datas;
}

void SprMsg::encodeU8Value(std::string& enDatas)
{
    enDatas.push_back(mU8Value);
}

void SprMsg::encodeU16Value(std::string& enDatas)
{
    enDatas.push_back(0xFF & (mU16Value >> 8));
    enDatas.push_back(0xFF & mU16Value);
}

void SprMsg::encodeU32Value(std::string& enDatas)
{
    enDatas.push_back(0xFF & (mU32Value >> 24));
    enDatas.push_back(0xFF & (mU32Value >> 16));
    enDatas.push_back(0xFF & (mU32Value >> 8));
    enDatas.push_back(0xFF & mU32Value);
}

void SprMsg::encodeU8Vec(std::string& enDatas)
{
    enDatas.insert(enDatas.end(), mU8Vec.begin(), mU8Vec.end());
}

void SprMsg::encodeU32Vec(std::string& enDatas)
{
    for (uint32_t i = 0; i < (uint32_t)mU32Vec.size(); i++)
    {
        uint32_t value = mU32Vec[i];
        enDatas.push_back(0xFF & (value >> 24));
        enDatas.push_back(0xFF & (value >> 16));
        enDatas.push_back(0xFF & (value >> 8));
        enDatas.push_back(0xFF & value);
    }
}

void SprMsg::encodeDatas(std::string& enDatas)
{
    const uint8_t* pData = reinterpret_cast<const uint8_t*>(mDatas.get());
    for (uint32_t i = 0; i < mDataSize; i++)
    {
        if (pData != nullptr)
        {
            enDatas.push_back(*pData);
        }
        else
        {
            SPR_LOGE("pData is nullptr!\n");
        }
    }
}

int8_t SprMsg::decode(const char* buf, int size)
{
    int8_t ret = 0;

    if (size < (int)sizeof(uint32_t))
    {
        SPR_LOGE("param is invaild!\n");
        return -1;
    }

    uint32_t tag = 0;
    ret = Convert::charPtrToUint32(tag, buf, sizeof(uint32_t));
    if (ret != 0)
    {
        SPR_LOGE("charPtrToUint32 fail!\n");
        return -1;
    }

    buf += sizeof(uint32_t);
    size -= sizeof(uint32_t);

    std::string enDatas(buf, buf + size);
    for (uint32_t i = (uint32_t)ESprMsgType::MSG_TYPE_MIN; i < (uint32_t)ESprMsgType::MSG_TYPE_MAX; i++)
    {
        if (tag & (1 << i))
        {
            ESprMsgType type = static_cast<ESprMsgType>(i);
            auto it = mDeFuncs.find(type);

            if (it != mDeFuncs.end()) {
                ((SprMsg*)this->*(it->second))(enDatas);
            }
            else {
                SPR_LOGW("Not find type: 0x%x!\n", static_cast<uint32_t>(type));
            }
        }
    }

    return ret;
}

int8_t SprMsg::encode(std::string & enDatas) const
{
    int ret = 0;

    Convert::uint32ToString(mTag, enDatas);
    for (uint32_t i = (uint32_t)ESprMsgType::MSG_TYPE_MIN; i < (uint32_t)ESprMsgType::MSG_TYPE_MAX; i++)
    {
        if (mTag & (1 << i))
        {
            ESprMsgType type = static_cast<ESprMsgType>(1 << i);
            auto it = mEnFuncs.find(type);
            if (it != mEnFuncs.end()) {
                ((SprMsg*)this->*(it->second))(enDatas);
            }
            else
            {
                SPR_LOGW("Not find type: 0x%x! \n", (uint32_t)type);
            }
        }
    }

    return ret;
}

void SprMsg::decodeU8Value(std::string& enDatas)
{
    if (enDatas.empty())
    {
        SPR_LOGE("enDatas is empty!\n");
        return;
    }

    mU8Value = enDatas[0];
    enDatas = enDatas.substr(sizeof(uint8_t));
}

void SprMsg::decodeU16Value(std::string& enDatas)
{
    if (enDatas.size() < sizeof(uint16_t))
    {
        SPR_LOGE("enDatas is invalid!\n");
        return;
    }

    mU16Value = (enDatas[0] << 8) | enDatas[1];
    enDatas = enDatas.substr(sizeof(uint16_t));
}

void SprMsg::decodeU32Value(std::string& enDatas)
{
    if (enDatas.size() < sizeof(uint32_t))
    {
        SPR_LOGE("enDatas is invalid!\n");
        return;
    }

    uint32_t value = 0;
    for (int i = 0; i < (int)sizeof(uint32_t); i++)
    {
        value <<= 8;
        value |= enDatas[i];
    }
    mU32Value = value;
    enDatas = enDatas.substr(sizeof(uint32_t));
}

void SprMsg::decodeU8Vec(std::string& enDatas)
{
    mU8Vec.clear();
    mU8Vec.assign(enDatas.begin(), enDatas.end());
    enDatas.clear();
}

void SprMsg::decodeU32Vec(std::string& enDatas)
{
    mU32Vec.clear();

    const int elementSize = sizeof(uint32_t);
    int numElements = enDatas.size() / elementSize;

    for (int i = 0; i < numElements; i++)
    {
        uint32_t value = 0;
        for (int j = 0; j < elementSize; j++)
        {
            value <<= 8;
            value |= enDatas[i * elementSize + j];
        }
        mU32Vec.push_back(value);
    }

    enDatas.clear();
}

void SprMsg::decodeDatas(std::string& enDatas)
{
    mDatas = std::make_shared<std::vector<uint8_t>>(enDatas.begin(), enDatas.end());
    mDataSize = enDatas.size();
    enDatas.clear();
}

