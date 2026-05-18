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
| len_ref | 长度引用，支持：<br>- 相对路径：同级`../字段名`、多级`../../字段名`<br>- 固定长度：`fixed_n`（n为字节数） |
|len_mode      |  长度模式，支持：<br> - `count`: 元素个数 <br> - `bytes`:  总字节数 <br> - `bit`:  位长 <br> - `condition`: 条件模式（非 0 解析，为 0 跳过）|
| len_formula (可选) | 长度计算公式，支持：<br> - 变量：`len_ref`（引用值）、`cur_pos`（当前偏移）<br>- 特殊值：`!` 表示对`len_ref`取反 |

示例：
```json
{
    "name": "battery_pack_codes",
    "type": "dynamic_field",
    "len_ref": "../bms_pack_counts",
    "len_mode": "bytes",
    "len_formula": "len_ref * 24",
    "child_template": {"name": "pack_code", "type": "u8"}
}
```

## 使用方法
`CodecX` 提供开箱即用的命令行工具 `propdump`，支持配置校验、协议解析与结果可视化，无需编写任何代码即可快速验证协议定义。

### 命令总览
```shell
./propdump <配置文件> [数据文件]
```
- 仅传配置文件：打印协议结构树，校验配置合法性
- 传配置 + 数据文件：解析二进制 / 十六进制数据，输出结构化结果

### 打印协议配置（校验配置合法性）
功能：解析 JSON 配置文件，输出完整的协议层级结构，自动检查语法错误与字段冲突。

```shell
$ ./propdump proto_config.json
 102 CFactory I: --------------  Print Config Details  -------------
 103 CFactory I: - name    : simple_device
 104 CFactory I: - type    : static_field
 113 CFactory I: - children [7]
  64 CFactory I:   ├─ dev_id (u32)
  64 CFactory I:   ├─ dev_type (u8)
  64 CFactory I:   ├─ status (u8)
  64 CFactory I:   ├─ voltage (u16)
  70 CFactory I:   ├─ base_info (static_field)
  64 CFactory I:   │   ├─ hw_ver (u16)
  64 CFactory I:   │   ├─ sw_ver (u16)
  64 CFactory I:   │   └─ chip_id (u32)
  64 CFactory I:   ├─ sensor_cnt (u8)
  70 CFactory I:   └─ sensor_list (dynamic_field)
  76 CFactory I:       ├─ len_ref : ../sensor_cnt
  77 CFactory I:       ├─ len_mode: count
  90 CFactory I:       └─ child_template: single_sensor (static_field)
  70 CFactory I:           └─ single_sensor (static_field)
  64 CFactory I:               ├─ sensor_id (u8)
  64 CFactory I:               ├─ sensor_type (u8)
  64 CFactory I:               ├─ data_len (u8)
  70 CFactory I:               └─ sensor_data (dynamic_field)
  76 CFactory I:                   ├─ len_ref : ../data_len
  77 CFactory I:                   ├─ len_mode: bytes
  90 CFactory I:                   └─ child_template:   (u8)
  64 CFactory I:                       └─   (u8)
 119 CFactory I: ---------------------------------------------------
```

###  解析协议数据
支持两种数据格式输入，输出格式完全一致：
- 十六进制文本文件（每行一个报文，空格分隔字节）
- 原始二进制文件（连续二进制流，自动拆帧）
```shell
# 解析十六进制文本文件
./propdump proto_config.json proto_bytes.txt

# 解析原始二进制文件
./propdump proto_config.json proto_bytes.bin
```

```shell
 $ ./propdump proto_config.json proto_bytes.txt
 222 CFactory I: ===================================================
 223 CFactory I: Protocol Parsing | simple_device | Total: 29 bytes
 224 CFactory I: ===================================================
 170 CFactory I:
 171 CFactory I: Frame 000 | Offset: 0
 172 CFactory I: ---------------------------------------------------
 157 CFactory I: [00] simple_device
 110 CFactory I:     [00-03] dev_id: 78 56 34 12
 110 CFactory I:     [04-04] dev_type: 05
 110 CFactory I:     [05-05] status: 01
 110 CFactory I:     [06-07] voltage: b8 0b
 157 CFactory I:     [08] base_info
 110 CFactory I:         [08-09] hw_ver: 00 01
 110 CFactory I:         [10-11] sw_ver: 01 02
 110 CFactory I:         [12-15] chip_id: dd cc bb aa
 110 CFactory I:     [16-16] sensor_cnt: 02
 150 CFactory I:     sensor_list[2]
 157 CFactory I:         [17] single_sensor
 110 CFactory I:             [17-17] sensor_id: 01
 110 CFactory I:             [18-18] sensor_type: 03
 110 CFactory I:             [19-19] data_len: 04
 142 CFactory I:             [20-23] sensor_data: 10  20  30  40
 157 CFactory I:         [24] single_sensor
 110 CFactory I:             [24-24] sensor_id: 02
 110 CFactory I:             [25-25] sensor_type: 05
 110 CFactory I:             [26-26] data_len: 02
 142 CFactory I:             [27-28] sensor_data: 50  60
 174 CFactory I: ---------------------------------------------------
 270 CFactory I: ===================================================
 271 CFactory I: Summary | Frame: 0 | Processed: 29/29
 272 CFactory I: ===================================================
```

- 解析二进制文件字节流
```shell
$ ./propdump proto_config.json proto_bytes.bin
 222 CFactory I: ===================================================
 223 CFactory I: Protocol Parsing | simple_device | Total: 29 bytes
 224 CFactory I: ===================================================
 170 CFactory I:
 171 CFactory I: Frame 000 | Offset: 0
 172 CFactory I: ---------------------------------------------------
 157 CFactory I: [00] simple_device
 110 CFactory I:     [00-03] dev_id: 78 56 34 12
 110 CFactory I:     [04-04] dev_type: 05
 110 CFactory I:     [05-05] status: 01
 110 CFactory I:     [06-07] voltage: b8 0b
 157 CFactory I:     [08] base_info
 110 CFactory I:         [08-09] hw_ver: 00 01
 110 CFactory I:         [10-11] sw_ver: 01 02
 110 CFactory I:         [12-15] chip_id: dd cc bb aa
 110 CFactory I:     [16-16] sensor_cnt: 02
 150 CFactory I:     sensor_list[2]
 157 CFactory I:         [17] single_sensor
 110 CFactory I:             [17-17] sensor_id: 01
 110 CFactory I:             [18-18] sensor_type: 03
 110 CFactory I:             [19-19] data_len: 04
 142 CFactory I:             [20-23] sensor_data: 10  20  30  40
 157 CFactory I:         [24] single_sensor
 110 CFactory I:             [24-24] sensor_id: 02
 110 CFactory I:             [25-25] sensor_type: 05
 110 CFactory I:             [26-26] data_len: 02
 142 CFactory I:             [27-28] sensor_data: 50  60
 174 CFactory I: ---------------------------------------------------
 270 CFactory I: ===================================================
 271 CFactory I: Summary | Frame: 0 | Processed: 29/29
 272 CFactory I: ===================================================
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
    "len_ref": "../data_len",
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
    "len_ref": "../data_length",
    "len_mode": "bytes",
    "len_formula": "(len_ref - cur_pos - 1) / 1",
    "child_template": {"name": "pack_code", "type": "bytes"}
}
```
注意：公式中只能使用 len_ref（len_ref 字段的值）和 cur_pos（当前解析偏移量）两个变量，不能直接使用其他字段名。


### 场景 4：嵌套动态数组
适用场景：数组元素本身包含动态结构，如 "网关下有多个设备，每个设备有多个传感器"。

```json
// 一级：设备数量
{"name": "device_cnt", "type": "u8"},
// 二级：设备列表（引用父级同级字段）
{
    "name": "device_list",
    "type": "dynamic_field",
    "len_ref": "../device_cnt",
    "len_mode": "count",
    "child_template": {
        "name": "device",
        "type": "static_field",
        "children": [
            {"name": "dev_id", "type": "u16"},
            // 三级：每个设备的传感器数量
            {"name": "sensor_cnt", "type": "u8"},
            // 四级：每个设备的传感器列表（引用当前父级子字段）
            {
                "name": "sensor_list",
                "type": "dynamic_field",
                "len_ref": "../sensor_cnt",
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
    "len_ref": "../total_length",
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

### 场景 7： 分包/多帧协议
适用场景：大数据分包传输（如固件升级、日志上报），每个包包含包序号和总包数。
```json
{
    "name": "fragment_frame",
    "type": "static_field",
    "children": [
        {"name": "head_flag", "type": "u16", "value": 43605},  // 0xAA 0x55
        {"name": "total_packets", "type": "u16"},
        {"name": "packet_seq", "type": "u16"},
        {"name": "data_len", "type": "u8"},
        {
            "name": "fragment_data",
            "type": "dynamic_field",
            "len_ref": "../data_len",
            "len_mode": "bytes",
            "child_template": {"name": "byte", "type": "u8"}
        },
        {"name": "crc8", "type": "u8"}
    ]
}
```

### 场景 8：条件可选容器
适用场景：根据前置标志位决定是否解析可选结构，如 `"是否包含扩展协议头"`、`"是否携带附加元数据"` 等二选一逻辑。
```json
{
    "name": "standard_message",
    "type": "static_field",
    "children": [
        {"name": "start_flag", "type": "u16", "value": 43605}, // 0xAA55
        {"name": "has_ext_header", "type": "u8"}, // 条件标志位

        // 条件1：有扩展头时解析
        {
            "name": "with_ext_header",
            "type": "dynamic_field",
            "len_ref": "../has_ext_header",
            "len_mode": "condition",
            "child_template": {
                "name": "ext_header",
                "type": "static_field",
                "children": [
                    {"name": "ext_version", "type": "u8"},
                    {"name": "timestamp", "type": "u32"},
                    {"name": "priority", "type": "u8"}
                ]
            }
        },

        // 条件2：无扩展头时解析（可放默认值或空结构）
        {
            "name": "without_ext_header",
            "type": "dynamic_field",
            "len_ref": "../has_ext_header",
            "len_mode": "condition",
            "len_formula": "!", // 条件取反
            "child_template": {
                "name": "default_header",
                "type": "static_field",
                "children": [
                    {"name": "default_priority", "type": "u8", "value": "02"}
                ]
            }
        }
    ]
}
```
