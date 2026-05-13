# CodecX - 轻量级通用协议编解码框架
基于 C++11 开发的嵌入式 / 车载 / 物联网专用二进制协议解析引擎，采用组合设计模式，提供无侵入、可扩展、标准化的自定义协议编解码能力。

## 核心特性
- 轻量无依赖：纯 C++11 实现，无第三方库侵入，适配嵌入式 Linux / 车载 IVI / 物联网设备
- 双容器模型：静态容器 + 动态容器，覆盖所有二进制协议场景
- 智能解析：支持协议动态长度解码
- 低侵入设计：无需修改业务代码，JSON 配置即编解码规则
- 高可扩展：支持自定义字段类型、自定义解析公式、多级嵌套结构
- 稳定可靠：经过车载协议、物联网串口协议等实战验证

## 核心概念

### 原子字段（Atom）
  最小数据单元，对应基础数据类型，无嵌套结构。

| 参数 |   说明   |可选值|
|------|----------|--|
| name | 字段名称 (唯一标识)| 自定义字符串|
| type | 数据类型 |u8/u16/u32/u64、s8/s16/s32/s64、leb128|
| value| 默认值 (可选)| 十进制/字符串|

示例：
```json
{"name": "head_flag", "type": "u16", "value": "$$"},
```

### 静态容器 （Static Field）
  固定定结构的嵌套容器，子字段数量 / 顺序固定，适用于协议头、固定结构体。

| 参数      |   说明     |
|-------    |---------- |
| name      | 容器名称   |
| type      | 固定为 static_field   |
| children  |子字段列表（原子 / 容器均可嵌套）| |

示例：
```json
{
    "name": "base_info",
    "type": "static_field",
    "children": [
        {"name": "hw_ver", "type": "u16"},
        {"name": "sw_ver", "type": "u16"},
        {"name": "chip_id", "type": "u32"}
    ]
},
```

### 动态容器 （Dynamic Field）
  长度可变的自适应容器，适用于数组、列表、变长字符串等动态结构。
|参数          |  说明 |
| --           | -- |
|name          |  容器名称 |
|type          |  固定为 `dynamic_field` |
|len_ref       |  长度引用，支持：同级字段名 / `fixed_n`（固定 n 字节） |
|len_mode      |  长度解析模式：`count`（元素个数）/ `bytes`（总字节数）/ `bit`（位长）|
|len_formula (可选) |  可选，长度计算公式，支持变量 len_ref（引用值）和 cur_pos（当前偏移） |
|child_template|  单个子元素的结构模板 |

示例：
```json
{
    "name": "battery_pack_codes",
    "type": "dynamic_field",
    "len_ref": "bms_pack_counts",
    "len_mode": "bytes",
    "len_formula": "len_ref * 24",
    "child_template": {"name": "pack_code", "type": "u8"}
}
```

## 典型场景示例
#### 场景 1：固定长度数组
适用场景：长度在协议中预先定义、不随报文变化的数组。

- 方式 1：静态枚举（适合≤10 个元素的小数组）
```json
{
    "name": "array",
    "type": "static_field",
    "children": [
        {"name": "", "type": "u8"},
        {"name": "", "type": "u8"},
        {"name": "", "type": "u8"}
    ]
},
```

- 方式 2：动态固定长度（适合大数组）
```json
{
    "name": "array",
    "type": "dynamic_field",
    "len_ref": "fixed_6",
    "len_mode": "bytes",
    "child_template": {"name": " ", "type": "u8"}
}
```

#### 场景 2：长度由报文字段指定的动态数组
适用场景：数组长度由前面的某个计数字段决定。
```json
// 1. 先定义长度计数字段
{"name": "data_len", "type": "u8"},

// 2. 动态数组引用该计数字段
{
    "name": "sensor_data",
    "type": "dynamic_field",
    "len_ref": "data_len",
    "len_mode": "bytes",
    "child_template": {"name": " ", "type": "u8"}
}
```

### 场景 3：长度需要公式计算的动态字段
适用场景：长度不是直接等于引用值，需要经过简单运算。

```json
// 示例：总长度减去已解析字节数，得到剩余数据长度
{"name": "data_length", "type": "u16"},
{
    "name": "battery_pack_codes",
    "type": "dynamic_field",
    "len_ref": "data_length",
    "len_mode": "bytes",
    "len_formula": "(data_length - curpos - 1) / 1",
    "child_template": {"name": "pack_code", "type": "bytes"}
}
```
注意：公式中只能使用 len_ref（len_ref 字段的值）和 cur_pos（当前解析偏移量）两个变量，不能直接使用其他字段名。


### 场景 4：嵌套动态数组
适用场景：数组元素本身包含动态结构，如 "网关下有多个设备，每个设备有多个传感器"。

```json
// 一级：设备数量
{"name": "device_cnt", "type": "u8"},
// 二级：设备列表
{
    "name": "device_list",
    "type": "dynamic_field",
    "len_ref": "device_cnt",
    "len_mode": "count",
    "child_template": {
        "name": "device",
        "type": "static_field",
        "children": [
            {"name": "dev_id", "type": "u16"},
            // 三级：每个设备的传感器数量
            {"name": "sensor_cnt", "type": "u8"},
            // 四级：每个设备的传感器列表
            {
                "name": "sensor_list",
                "type": "dynamic_field",
                "len_ref": "sensor_cnt",
                "len_mode": "count",
                "child_template": {
                    "name": "sensor",
                    "type": "static_field",
                    "children": [
                        {"name": "sensor_id", "type": "u8"},
                        {"name": "sensor_value", "type": "u16"}
                    ]
                }
            }
        ]
    }
}
```

### 场景 5：剩余字节全部解析
适用场景：协议末尾的所有剩余字节都属于某个字段，无需显式指定长度。
```json
{"name": "total_length", "type": "u16"},
{
    "name": "payload",
    "type": "dynamic_field",
    "len_ref": "total_length",
    "len_mode": "bytes",

    // 总长度减去当前已解析的位置，得到剩余所有字节
    "len_formula": "len_ref - cur_pos",
    "child_template": {"name": "byte", "type": "u8"}
}
```

### 场景 6：LEB128 变长整数
适用场景：Protobuf、MQTT 等协议使用的变长整数编码，节省小数值的传输空间。
```json
// 直接使用内置的leb128类型，自动解析变长整数
{"name": "varint_length", "type": "leb128"},
```
