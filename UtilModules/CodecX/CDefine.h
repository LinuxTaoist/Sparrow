/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CDefine.h
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
#ifndef __CDEFINE_H__
#define __CDEFINE_H__

#define TEXT_HEAD_FLAG_TAG          "head_flag"
#define TEXT_NAME_TAG               "name"
#define TEXT_VALUE_TAG              "value"
#define TEXT_CHILDREN_TAG           "children"
#define TEXT_CHILD_TEMPLATE_TAG     "child_template"

#define TEXT_LEN_REF_TAG            "len_ref"
#define TEXT_LEN_REF_FIXED          "fixed_"

#define TEXT_LEN_MODE_TAG           "len_mode"
#define TEXT_LEN_MODE_COUNT         "count"
#define TEXT_LEN_MODE_BIT           "bit"
#define TEXT_LEN_MODE_BYTES         "bytes"

#define TEXT_LEN_FORMULA_TAG        "len_formula"
#define TEXT_LEN_FORMULA_CURPOS     "curpos"

#define TEXT_TYPE_TAG               "type"
#define TEXT_TYPE_U8                "u8"
#define TEXT_TYPE_U16               "u16"
#define TEXT_TYPE_U32               "u32"
#define TEXT_TYPE_U64               "u64"
#define TEXT_TYPE_S8                "s8"
#define TEXT_TYPE_S16               "s16"
#define TEXT_TYPE_S32               "s32"
#define TEXT_TYPE_S64               "s64"
#define TEXT_TYPE_LEB128            "leb128"
#define TEXT_TYPE_SFIELD            "static_field"
#define TEXT_TYPE_DFIELD            "dynamic_field"
#define TEXT_TYPE_FIELD_SUFFIX      "_field"

enum CEndianType {
    CENDIAN_LITTLE = 0,
    CENDIAN_BIG,
    CENDIAN_BUTT
};

#endif // __CDEFINE_H__
