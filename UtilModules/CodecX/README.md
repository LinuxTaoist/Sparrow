# CodecX 使用说明

## 介绍
`CodecX` 是一款自研轻量级通用数据协议编解码开源框架，基于 `C++11` 标准开发，采用组合设计模式，面向嵌入式 Linux、车载 IVI、物联网等场景，提供标准化、可扩展、低侵入的自定义二进制 / 结构化协议编解码能力。

## 规则
###  元素字段
- name: 表示字段名称
- type: 表示字段类型 u8, u16, u32, u64, s8, s16, s32, s64


###  静态容器：`{"name": "", "type": ""}`
```json
{
    "name": "...",
    "type": "...",
    "children": [
        {"name": "...", "type": "..."},
        ...
    ]
},
```
- **name**: 表示静态容器名称
- **type**: `static_field`
- **children**: 表示静态容器中子字段模板

###  动态容器
```json
{
    "name": "...",
    "type": "...",
    "len_ref": "...",
    "len_mode": "...",
    "child_template": {...}
},
```
- **name**: 表示动态容器名称
- **type**: `dynamic_field`
- **len_ref**: 表示动态容器中元素个数或元素总字节数的引用字段
- **len_mode**:
    - `count`:      \
      表示 `len_ref` 是一个计数器，`size = len_ref`
    - `bit` (扩展):   \
      表示 `len_ref` 是一个位长度，`size = len_ref / 8`
    - `(remain - a) / b`    \
      表示一个计算公式，`size = (len_ref - a) / b`
    - `fixed_x`:   \
      表示使用x字节的固定长度，`size = x`

- **child_template**: 表示动态容器中元素的模板

## 配置
- 带注释
```json
{
    "name": "iot_devices",
    "type": "static_field",
    "children": [
        /* 上报描述 (静态) */
        {"name": "gateway_id",  "type": "u32"},
        {"name": "report_time", "type": "u32"},
        {"name": "dev_cnt",     "type": "u16"},

        /* 设备信息列表 (动态) */
        {
            "name": "device_list",
            "type": "dynamic_field",
            "len_ref": "iot_devices.dev_cnt",
            "len_mode": "count",
            "child_template": {
                "name": "single_device",
                "type": "static_field",
                "children": [
                    /* 通用字段1: 设备类型 （固定长度） */
                    {"name": "dev_type", "type": "u8"},

                    /* 通用字段2: 设备名长度（固定长度） */
                    /* 通用字段3: 设备名（动态长度） */
                    {"name": "dev_name_len", "type": "u8"},
                    {
                        "name": "dev_name",
                        "type": "dynamic_field",
                        "len_ref": "dev_name_len",
                        "len_mode": "byte",
                        "child_template": {"name": "index", "type": "u8"}
                    },

                    /* 通用字段4: 设备描述长度（固定长度） */
                    /* 通用字段5: 设备描述（动态长度） */
                    {"name": "dev_desc_len", "type": "u8"},
                    {
                        "name": "dev_desc",
                        "type": "dynamic_field",
                        "len_ref": "dev_desc_len",
                        "len_mode": "byte",
                        "child_template": {"name": "index", "type": "u8"}
                    },

                    /* 通用字段6: 设备厂商ID（固定长度）*/
                    {"name": "dev_vendor", "type": "u8"},

                    /* 通用字段7: 定制化属性数量（固定长度） */
                    /* 定制字段8: 定制化属性 （动态/可选，取决于custom_attr_cnt） */
                    {"name": "custom_attr_cnt", "type": "u8"},
                    {
                        "name": "custom_attr_list",
                        "type": "dynamic_field",
                        "len_ref": "custom_attr_cnt",
                        "len_mode": "count",
                        "child_template": {
                            "name": "single_attr",
                            "type": "static_field",
                            "children": [
                                // 定制化字段8 + (2n + 1): 生产日期（固定长度）
                                // 定制化字段8 + (2n + 2): 生产ID（固定长度）
                                {"name": "product_date", "type": "u32"},
                                {"name": "product_id", "type": "u16"}
                            ]
                        }
                    }
                ]
            }
        },

        /* CRC校验 （静态） */
        {"name": "crc8", "type": "u8"}
    ]
}
```
- 不带注释
```json
{
    "name": "iot_devices",
    "type": "static_field",
    "children": [
        {"name": "gateway_id",  "type": "u32"},
        {"name": "report_time", "type": "u32"},
        {"name": "dev_cnt",     "type": "u16"},

        {
            "name": "device_list",
            "type": "dynamic_field",
            "len_ref": "iot_devices.dev_cnt",
            "len_mode": "count",
            "child_template": {
                "name": "single_device",
                "type": "static_field",
                "children": [
                    {"name": "dev_type", "type": "u8"},

                    {"name": "dev_name_len", "type": "u8"},
                    {
                        "name": "dev_name",
                        "type": "dynamic_field",
                        "len_ref": "dev_name_len",
                        "len_mode": "byte",
                        "child_template": {"name": "index", "type": "u8"}
                    },

                    {"name": "dev_desc_len", "type": "u8"},
                    {
                        "name": "dev_desc",
                        "type": "dynamic_field",
                        "len_ref": "dev_desc_len",
                        "len_mode": "byte",
                        "child_template": {"name": "index", "type": "u8"}
                    },

                    {"name": "dev_vendor", "type": "u8"},

                    {"name": "custom_attr_cnt", "type": "u8"},
                    {
                        "name": "custom_attr_list",
                        "type": "dynamic_field",
                        "len_ref": "custom_attr_cnt",
                        "len_mode": "count",
                        "child_template": {
                            "name": "single_attr",
                            "type": "static_field",
                            "children": [
                                {"name": "product_date", "type": "u32"},
                                {"name": "product_id", "type": "u16"}
                            ]
                        }
                    }
                ]
            }
        },

        {"name": "crc8", "type": "u8"}
    ]
}
```
## 使用
TODO