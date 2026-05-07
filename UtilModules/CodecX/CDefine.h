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

#define TEXT_TYPE_U8        "u8"
#define TEXT_TYPE_U16       "u16"
#define TEXT_TYPE_U32       "u32"
#define TEXT_TYPE_U64       "u64"
#define TEXT_TYPE_S8        "s8"
#define TEXT_TYPE_S16       "s16"
#define TEXT_TYPE_S32       "s32"
#define TEXT_TYPE_S64       "s64"

#define TEXT_NAME           "name"
#define TEXT_TYPE           "type"
#define TEXT_LEN_REF        "len_ref"
#define TEXT_LEN_MODE       "len_mode"
#define TEXT_CHILDREN       "children"
#define TEXT_CHILD_TEMPLATE "child_template"
#define TEXT_STATIC_FIELD   "static_field"
#define TEXT_DYNAMIC_FIELD  "dynamic_field"

enum CEndianType {
    CENDIAN_LITTLE = 0,
    CENDIAN_BIG,
    CENDIAN_BUTT
};

#endif // __CDEFINE_H__
