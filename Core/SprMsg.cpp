/**
 *---------------------------------------------------------------------------------------------------------------------
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
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "SprMsg.h"
#include "Util/Convert.h"

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)
#define SPR_LOGD(fmt, args...) printf("%d SprMsg D: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%d SprMsg W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d SprMsg E: " fmt, __LINE__, ##args)

SprMsg::SprMsg()
{
    Init();
}

SprMsg::SprMsg(const SprMsg& srcMsg)
{
    CopyMsg(srcMsg);
}

SprMsg::SprMsg(uint32_t msgId)
{
    Init();
    mMsgId = msgId;
}

SprMsg::SprMsg(uint32_t to, uint32_t msgId)
{
    Init();
    mTo = to;
    mMsgId = msgId;
}

SprMsg::SprMsg(uint32_t from, uint32_t to, uint32_t msgId)
{
    Init();
    mFrom = from;
    mTo = to;
    mMsgId = msgId;
}

SprMsg::SprMsg(std::string datas)
{
    Init();
    Decode(datas);
}

SprMsg& SprMsg::operator=(const SprMsg &srcMsg)
{
    if (this == &srcMsg)
    {
        // do nothing
    }
    else
    {
        CopyMsg(srcMsg);
    }

    return *this;
}

int SprMsg::CopyMsg(const SprMsg& srcMsg)
{
    mFrom = srcMsg.mFrom;
    mTo = srcMsg.mTo;
    mMsgId = srcMsg.mMsgId;
    mTag = srcMsg.mTag;
    mBoolValue = srcMsg.mBoolValue;
    mU8Value = srcMsg.mU8Value;
    mU16Value = srcMsg.mU16Value;
    mU32Value = srcMsg.mU32Value;
    mStringLength = srcMsg.mStringLength;
    mString = srcMsg.mString;
    mU8VecLength = srcMsg.mU8VecLength;
    mU8Vec = srcMsg.mU8Vec;
    mU32VecLength = srcMsg.mU32VecLength;
    mU32Vec = srcMsg.mU32Vec;
    mDataSize = srcMsg.mDataSize;
    mDatas = srcMsg.mDatas;
    mEnFuncs = srcMsg.mEnFuncs;
    mDeFuncs = srcMsg.mDeFuncs;

    return 0;
}

void SprMsg::Init()
{
    mFrom = 0;
    mTo = 0;
    mMsgId = 0;
    mTag = 0;
    mBoolValue = false;
    mU8Value = 0;
    mU16Value = 0;
    mU32Value = 0;
    mStringLength = 0;
    mU8VecLength = 0;
    mU32VecLength = 0;
    mDataSize = 0;

    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_BOOLVALUE, &SprMsg::EncodeBoolValue));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U8VALUE,   &SprMsg::EncodeU8Value));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U16VALUE,  &SprMsg::EncodeU16Value));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U32VALUE,  &SprMsg::EncodeU32Value));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_STRING,    &SprMsg::EncodeString));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U8VEC,     &SprMsg::EncodeU8Vec));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U32VEC,    &SprMsg::EncodeU32Vec));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_PTR,       &SprMsg::EncodeDatas));

    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_BOOLVALUE, &SprMsg::DecodeBoolValue));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U8VALUE,   &SprMsg::DecodeU8Value));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U16VALUE,  &SprMsg::DecodeU16Value));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U32VALUE,  &SprMsg::DecodeU32Value));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_STRING,    &SprMsg::DecodeString));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U8VEC,     &SprMsg::DecodeU8Vec));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U32VEC,    &SprMsg::DecodeU32Vec));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_PTR,       &SprMsg::DecodeDatas));
}

void SprMsg::Clear()
{
    mTag = 0;
    mString.clear();
    mU8Vec.clear();
    mU32Vec.clear();
    mDatas.clear();
}

// Frame : mMsgId(4) + mTag(4) + length1 +  data1 ... lengthN + dataN
int8_t SprMsg::Decode(std::string& deDatas)
{
    int8_t ret = 0;

    Clear();
    DecodeFrom(deDatas);
    DecodeTo(deDatas);
    DecodeMsgId(deDatas);
    DecodeTag(deDatas);
    for (auto i = (int)ESprMsgType::MSG_TYPE_MIN; i < (int)ESprMsgType::MSG_TYPE_MAX; i++)
    {
        if (mTag & (1 << i))
        {
            ESprMsgType type = static_cast<ESprMsgType>(i);
            auto it = mDeFuncs.find(type);

            if (it != mDeFuncs.end()) {
                ((SprMsg*)this->*(it->second))(deDatas);
            } else {
                SPR_LOGW("Not find type: 0x%x!\n", (uint32_t)(type));
            }
        }
    }

    return ret;
}

int8_t SprMsg::Encode(std::string & enDatas) const
{
    int ret = 0;

    EncodeFrom(enDatas);
    EncodeTo(enDatas);
    EncodeMsgId(enDatas);
    EncodeTag(enDatas);
    for (uint32_t i = (uint32_t)ESprMsgType::MSG_TYPE_MIN; i < (uint32_t)ESprMsgType::MSG_TYPE_MAX; i++)
    {
        if (mTag & (1 << i))
        {
            ESprMsgType type = static_cast<ESprMsgType>(i);
            auto it = mEnFuncs.find(type);
            if (it != mEnFuncs.end()) {
                ((SprMsg*)this->*(it->second))(enDatas);
            }
            else {
                SPR_LOGW("Not find type: 0x%x! \n", (uint32_t)type);
            }
        }
    }

    return ret;
}

void SprMsg::SetFrom(uint32_t from)
{
    mFrom = from;
}

void SprMsg::SetTo(uint32_t to)
{
    mTo = to;
}

void SprMsg::SetMsgId(uint32_t msgId)
{
    mMsgId = msgId;
}

void SprMsg::SetBoolValue(bool value)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_BOOLVALUE);
    mBoolValue = value;
}

void SprMsg::SetU8Value(uint8_t value)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_U8VALUE);
    mU8Value = value;
}

void SprMsg::SetU16Value(uint16_t value)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_U16VALUE);
    mU16Value = value;
}

void SprMsg::SetU32Value(uint32_t value)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_U32VALUE);
    mU32Value = value;
}
void SprMsg::SetString(const std::string& str)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_STRING);
    mStringLength = str.size();
    mString = str;
}

void SprMsg::SetU8Vec(const std::vector<uint8_t>& vec)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_U8VEC);
    mU8VecLength = vec.size();
    mU8Vec.assign(vec.begin(), vec.end());
}

void SprMsg::SetU32Vec(const std::vector<uint32_t>& vec)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_U32VEC);
    mU32VecLength = vec.size();
    mU32Vec.assign(vec.begin(), vec.end());
}
void SprMsg::EncodeFrom(std::string& enDatas) const
{
    Convert::intToString(mFrom, enDatas);
}

void SprMsg::EncodeTo(std::string& enDatas) const
{
    Convert::intToString(mTo, enDatas);
}

void SprMsg::EncodeMsgId(std::string& enDatas) const
{
    Convert::intToString(mMsgId, enDatas);
}

void SprMsg::EncodeTag(std::string& enDatas) const
{
    Convert::intToString(mTag, enDatas);
}

void SprMsg::EncodeBoolValue(std::string& enDatas)
{
    enDatas.push_back((uint8_t)mBoolValue);
}

void SprMsg::EncodeU8Value(std::string& enDatas)
{
    enDatas.push_back(mU8Value);
}

void SprMsg::EncodeU16Value(std::string& enDatas)
{
    enDatas.push_back(0xFF & (mU16Value >> 8));
    enDatas.push_back(0xFF & mU16Value);
}

void SprMsg::EncodeU32Value(std::string& enDatas)
{
    enDatas.push_back(0xFF & (mU32Value >> 24));
    enDatas.push_back(0xFF & (mU32Value >> 16));
    enDatas.push_back(0xFF & (mU32Value >> 8));
    enDatas.push_back(0xFF & mU32Value);
}

void SprMsg::EncodeString(std::string& enDatas)
{
    enDatas.push_back(0xFF & (mStringLength >> 24));
    enDatas.push_back(0xFF & (mStringLength >> 16));
    enDatas.push_back(0xFF & (mStringLength >> 8));
    enDatas.push_back(0xFF & mStringLength);
    enDatas.append(mString);
}

void SprMsg::EncodeU8Vec(std::string& enDatas)
{
    enDatas.push_back(0xFF & (mU8VecLength >> 24));
    enDatas.push_back(0xFF & (mU8VecLength >> 16));
    enDatas.push_back(0xFF & (mU8VecLength >> 8));
    enDatas.push_back(0xFF & mU8VecLength);
    enDatas.insert(enDatas.end(), mU8Vec.begin(), mU8Vec.end());
}

void SprMsg::EncodeU32Vec(std::string& enDatas)
{
    enDatas.push_back(0xFF & (mU32VecLength >> 24));
    enDatas.push_back(0xFF & (mU32VecLength >> 16));
    enDatas.push_back(0xFF & (mU32VecLength >> 8));
    enDatas.push_back(0xFF & mU32VecLength);

    for (uint32_t i = 0; i < (uint32_t)mU32Vec.size(); i++)
    {
        uint32_t value = mU32Vec[i];
        enDatas.push_back(0xFF & (value >> 24));
        enDatas.push_back(0xFF & (value >> 16));
        enDatas.push_back(0xFF & (value >> 8));
        enDatas.push_back(0xFF & value);
    }
}

void SprMsg::EncodeDatas(std::string& enDatas)
{
    enDatas.push_back(0xFF & (mDataSize >> 24));
    enDatas.push_back(0xFF & (mDataSize >> 16));
    enDatas.push_back(0xFF & (mDataSize >> 8));
    enDatas.push_back(0xFF & mDataSize);
    enDatas.insert(enDatas.end(), mDatas.begin(), mDatas.end());
}

void SprMsg::DecodeFrom(std::string& deDatas)
{
    if (Convert::stringToInt(mFrom, deDatas) != 0)
    {
        SPR_LOGE("Decode from Fail!\n");
        return;
    }

    deDatas = deDatas.substr(sizeof(mFrom));
}

void SprMsg::DecodeTo(std::string& deDatas)
{
    if (Convert::stringToInt(mTo, deDatas) != 0)
    {
        SPR_LOGE("Decode to Fail!\n");
        return;
    }

    deDatas = deDatas.substr(sizeof(mTo));
}

void SprMsg::DecodeMsgId(std::string& deDatas)
{
    if (Convert::stringToInt(mMsgId, deDatas) != 0)
    {
        SPR_LOGE("Decode msgId Fail!\n");
        return;
    }

    deDatas = deDatas.substr(sizeof(mMsgId));
}

void SprMsg::DecodeTag(std::string& deDatas)
{
    if (Convert::stringToInt(mTag, deDatas) != 0)
    {
        SPR_LOGE("Decode tag Fail!\n");
        return;
    }

    deDatas = deDatas.substr(sizeof(mTag));
}

void SprMsg::DecodeBoolValue(std::string& deDatas)
{
    if (deDatas.size() < sizeof(mBoolValue))
    {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    mBoolValue = (deDatas[0] != 0);
    deDatas = deDatas.substr(sizeof(bool));
}

void SprMsg::DecodeU8Value(std::string& deDatas)
{
    if (deDatas.size() < sizeof(mU8Value))
    {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    mU8Value = deDatas[0];
    deDatas = deDatas.substr(sizeof(uint8_t));
}

void SprMsg::DecodeU16Value(std::string& deDatas)
{
    if (deDatas.size() < sizeof(mU16Value))
    {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    mU16Value = (deDatas[0] << 8) | deDatas[1];
    deDatas = deDatas.substr(sizeof(uint16_t));
}

void SprMsg::DecodeU32Value(std::string& deDatas)
{
    if (deDatas.size() < sizeof(mU32Value))
    {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    uint32_t value = 0;
    for (int i = 0; i < (int)sizeof(mU32Value); i++)
    {
        value <<= 8;
        value |= deDatas[i];
    }
    mU32Value = value;
    deDatas = deDatas.substr(sizeof(mU32Value));
}

void SprMsg::DecodeString(std::string& deDatas)
{
    if (Convert::stringToInt(mStringLength, deDatas) != 0)
    {
        SPR_LOGE("Decode string length Fail!\n");
        return;
    }

    if (deDatas.size() < sizeof(mStringLength) + mStringLength)
    {
        SPR_LOGE("deDatas is invalid! size = %zu, length = %zu\n", deDatas.size(), sizeof(mStringLength) + mStringLength);
        return;
    }

    mString = deDatas.substr(sizeof(mStringLength), mStringLength);
    deDatas = deDatas.substr(sizeof(mStringLength) + mStringLength);
}

void SprMsg::DecodeU8Vec(std::string& deDatas)
{
    if (Convert::stringToInt(mU8VecLength, deDatas) != 0)
    {
        SPR_LOGE("Decode vector size Fail!\n");
        return;
    }

    if (deDatas.size() < sizeof(mU8VecLength) + mU8VecLength)
    {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    mU8Vec.assign(deDatas.begin() + sizeof(mU8VecLength), deDatas.begin() + sizeof(mU8VecLength) + mU8VecLength);
    deDatas = deDatas.substr(sizeof(mU8VecLength) + mU8VecLength);
}

void SprMsg::DecodeU32Vec(std::string& deDatas)
{
    if (Convert::stringToInt(mU32VecLength, deDatas) != 0)
    {
        SPR_LOGE("Decode vector size Fail!\n");
        return;
    }

    if (deDatas.size() < sizeof(mU32VecLength) + mU32VecLength * sizeof(uint32_t))
    {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    for (uint32_t i = 0; i < mU32VecLength; i++)
    {
        uint32_t value = 0;
        for (uint32_t j = 0; j < sizeof(uint32_t); j++)
        {
            value <<= 8;
            value |= deDatas[sizeof(mU32VecLength) + i * sizeof(uint32_t) + j];
        }
        mU32Vec.push_back(value);
    }

    deDatas = deDatas.substr(sizeof(mU32VecLength) + mU32VecLength * sizeof(uint32_t));
}

void SprMsg::DecodeDatas(std::string& deDatas)
{
    if (deDatas.size() < sizeof(mDataSize))
    {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    if (Convert::stringToInt(mDataSize, deDatas) != 0)
    {
        SPR_LOGE("Decode vector size Fail!\n");
        return;
    }

    mDatas.assign(deDatas.begin() + sizeof(mDataSize), deDatas.begin() + sizeof(mDataSize) + mDataSize);
    deDatas = deDatas.substr(sizeof(mDataSize) + mDataSize);
}

