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
#include <unordered_map>
#include <stdint.h>

enum class ESprMsgType : uint32_t
{
    MSG_TYPE_MIN,
    MSG_TYPE_U8VALUE = 1,
    MSG_TYPE_U16VALUE,
    MSG_TYPE_U32VALUE,
    MSG_TYPE_U8VEC,
    MSG_TYPE_U32VEC,
    MSG_TYPE_PTR,
    MSG_TYPE_MAX
};

class SprMsg
{
public:
    SprMsg();
    SprMsg(const char* buf, int size);
    void clear();

    void setU8Value(uint8_t value);
    void setU16Value(uint16_t value);
    void setU32Value(uint32_t value);
    void setU8Vec(const std::vector<uint8_t>& vec);
    void setU32Vec(const std::vector<uint32_t>& vec);
    void setDatas(std::shared_ptr<void> datas, uint32_t size);
    int8_t decode(const char* buf, int size);
    int8_t encode(std::string& enDatas) const;

    uint8_t getU8Value() { return mU8Value; }
    uint16_t getU16Value() { return mU16Value; }
    uint32_t getU32Value() { return mU32Value; }
    std::vector<uint8_t> getU8Vec() { return mU8Vec; }
    std::vector<uint32_t> getU16Vec() { return mU32Vec; }
    std::shared_ptr<void> getDatas() { return mDatas; }


private:
    uint32_t mTag;
    uint8_t mU8Value;
    uint16_t mU16Value;
    uint32_t mU32Value;
    uint32_t mDataSize;
    std::vector<uint8_t> mU8Vec;
    std::vector<uint32_t> mU32Vec;
    std::shared_ptr<void> mDatas;

    using CodecFunction = void (SprMsg::*)(std::string&);
    std::unordered_map<ESprMsgType, CodecFunction> mEnFuncs;
    std::unordered_map<ESprMsgType, CodecFunction> mDeFuncs;

    void encodeU8Value(std::string& enDatas);
    void encodeU16Value(std::string& enDatas);
    void encodeU32Value(std::string& enDatas);
    void encodeU8Vec(std::string& enDatas);
    void encodeU32Vec(std::string& enDatas);
    void encodeDatas(std::string& enDatas);

    void decodeU8Value(std::string& enDatas);
    void decodeU16Value(std::string& enDatas);
    void decodeU32Value(std::string& enDatas);
    void decodeU8Vec(std::string& enDatas);
    void decodeU32Vec(std::string& enDatas);
    void decodeDatas(std::string& enDatas);
};

#endif
