# CodecX - 轻量级通用协议编解码框架

CodecX 是一个基于 C++11 的配置驱动协议解析模块，适用于嵌入式、车载、串口和物联网二进制协议。它用节点树描述协议结构，并按配置完成解析、打印和访问。

本文按“概念 -> 配置 -> 工具 -> 接入 -> 场景”的顺序说明 CodecX 的使用方式。

## 1. 核心特性

- 纯 C++11 实现，依赖简单，适合现有工程直接集成
- 配置驱动，无需为每个协议手写结构体解析代码
- 支持静态字段、动态字段、多级嵌套、变长字段、条件字段
- 支持相对路径、绝对路径、固定长度三种长度引用方式
- 支持简单公式运算，可表达剩余长度、条件反转等场景
- 自带 `propdump` 工具，可直接校验配置并解析报文
- 支持十六进制文本和原始二进制两类输入文件

## 2. 整体模型

CodecX 的协议树包含两类节点：

- 原子字段 `Atom`：叶子节点，表示一个基础值
- 容器字段 `Field`：非叶子节点，表示一组子字段

容器字段又分两种：

- `static_field`：固定结构，子节点数量和顺序固定
- `dynamic_field`：动态结构，子节点个数由 `len_ref`、`len_mode`、`len_formula` 联合决定

建模原则：固定结构用 `static_field`，数组或变长区用 `dynamic_field`，基础值用 `Atom`。

## 3. 快速开始

### 3.1 最小使用流程

1. 写一份 JSON 协议配置
2. 用 `propdump` 检查配置结构
3. 输入报文数据并查看结果

命令格式如下：

```shell
./propdump <config_path> [bytes_path] [option]
```

说明：

- 仅传 `config_path`：打印配置结构并检查配置
- 传 `config_path` 和 `bytes_path`：按配置解析数据并打印结果
- 可选参数：`-d` 或 `--debug`，建议放在位置参数后面使用

示例：

```shell
# 仅检查配置
./propdump proto_config.json

# 解析十六进制文本文件
./propdump proto_config.json proto_bytes.txt

# 解析原始二进制文件
./propdump proto_config.json proto_bytes.bin

# 打开调试日志
./propdump proto_config.json proto_bytes.txt --debug
```

### 3.2 一个完整的最小示例

下面示例表示一个设备信息报文：前面是固定头，后面是按数量展开的传感器列表，每个传感器包含一个变长数据区。

```json
{
    "name": "simple_device",
    "type": "static_field",
    "children": [
        {"name": "dev_id", "type": "u32"},
        {"name": "dev_type", "type": "u8"},
        {"name": "status", "type": "u8"},
        {"name": "voltage", "type": "u16"},
        {
            "name": "base_info",
            "type": "static_field",
            "children": [
                {"name": "hw_ver", "type": "u16"},
                {"name": "sw_ver", "type": "u16"},
                {"name": "chip_id", "type": "u32"}
            ]
        },
        {"name": "sensor_cnt", "type": "u8"},
        {
            "name": "sensor_list",
            "type": "dynamic_field",
            "len_ref": "../sensor_cnt",
            "len_mode": "count",
            "child_template": {
                "name": "single_sensor",
                "type": "static_field",
                "children": [
                    {"name": "sensor_id", "type": "u8"},
                    {"name": "sensor_type", "type": "u8"},
                    {"name": "data_len", "type": "u8"},
                    {
                        "name": "sensor_data",
                        "type": "dynamic_field",
                        "len_ref": "../data_len",
                        "len_mode": "bytes",
                        "child_template": {"name": "byte", "type": "u8"}
                    }
                ]
            }
        }
    ]
}
```

如果配套数据文件内容为：

```text
78 56 34 12 05 01 b8 0b 00 01 01 02 dd cc bb aa 02 01 03 04 10 20 30 40 02 05 02 50 60
```

那么输出会类似：

```shell
 249 CFactory I: Protocol Parsing | simple_device | Total: 29 bytes
 198 CFactory I: Frame 000 | Offset: 0
 184 CFactory I: [00] simple_device
 138 CFactory I:     [00-03] dev_id: 78 56 34 12
 138 CFactory I:     [04-04] dev_type: 05
 138 CFactory I:     [05-05] status: 01
 138 CFactory I:     [06-07] voltage: b8 0b
 184 CFactory I:     [08] base_info
 138 CFactory I:         [08-09] hw_ver: 00 01
 138 CFactory I:         [10-11] sw_ver: 01 02
 138 CFactory I:         [12-15] chip_id: dd cc bb aa
 138 CFactory I:     [16-16] sensor_cnt: 02
 177 CFactory I:     sensor_list[2]
 184 CFactory I:         [17] single_sensor
 138 CFactory I:             [17-17] sensor_id: 01
 138 CFactory I:             [18-18] sensor_type: 03
 138 CFactory I:             [19-19] data_len: 04
 169 CFactory I:             [20-23] sensor_data: 10  20  30  40
```

## 4. 配置文件说明

### 4.1 根节点约定

配置根节点建议使用 `static_field`，对应一个报文入口。

推荐写法：

```json
{
    "name": "my_protocol",
    "type": "static_field",
    "children": [
    ]
}
```

### 4.2 通用字段

| 字段 | 是否必须 | 说明 |
| --- | --- | --- |
| `name` | 是 | 节点名称，建议同级唯一 |
| `type` | 是 | 节点类型 |
| `value` | 否 | 原子字段默认值，常用于固定头或常量字段 |

`value` 常见写法：

- 数字，例如 `43605`
- 字符串，例如 `"$$"`、`"02"`

示例：

```json
{"name": "head_flag", "type": "u16", "value": 43605}
```

### 4.3 原子字段 Atom

原子字段是最小数据单元，没有子节点。

当前内置类型如下：

| 类型 | 说明 |
| --- | --- |
| `u8` `u16` `u32` `u64` | 无符号整数 |
| `s8` `s16` `s32` `s64` | 有符号整数 |
| `leb128` | 变长整数，适合 Protobuf / MQTT 一类场景 |

示例：

```json
{"name": "msg_id", "type": "u16"}
```

### 4.4 静态容器 static_field

`static_field` 表示固定结构，常见场景：

- 协议头
- 地址段
- 版本信息区
- 固定顺序的组合字段

配置项：

| 字段 | 是否必须 | 说明 |
| --- | --- | --- |
| `name` | 是 | 容器名称 |
| `type` | 是 | 固定为 `static_field` |
| `children` | 是 | 子节点数组，可嵌套 `Atom` / `static_field` / `dynamic_field` |

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
}
```

### 4.5 动态容器 dynamic_field

`dynamic_field` 表示可重复、可变长或按条件存在的结构。

配置项：

| 字段 | 是否必须 | 说明 |
| --- | --- | --- |
| `name` | 是 | 容器名称 |
| `type` | 是 | 固定为 `dynamic_field` |
| `len_ref` | 是 | 长度引用来源 |
| `len_mode` | 是 | 长度解释方式 |
| `child_template` | 是 | 子模板，动态字段按该模板克隆展开 |
| `len_formula` | 否 | 对 `len_ref` 再做一次计算 |

示例：

```json
{
    "name": "sensor_data",
    "type": "dynamic_field",
    "len_ref": "../data_len",
    "len_mode": "bytes",
    "child_template": {"name": "byte", "type": "u8"}
}
```

### 4.6 children 和 child_template 的区别

- `children` 用于 `static_field`
- `child_template` 用于 `dynamic_field`

两者含义不同：

- `children`：固定子节点列表
- `child_template`：重复展开模板

## 5. 长度控制规则

### 5.1 len_ref 的三种写法

`len_ref` 用于指定动态字段的长度来源。当前支持三种形式。

#### 1. 相对路径

适合同层、父层和祖先层引用。

```json
"len_ref": "../data_len"
"len_ref": "../../header/payload_len"
```

规则：

- `..` 表示父节点
- 支持多级回退
- 回退之后可以继续向下找子节点

#### 2. 绝对路径

适合跨层级引用，尤其是深层嵌套场景。

```json
"len_ref": "/simple_device/sensor_cnt"
"len_ref": "/simple_device/sensor_list/single_sensor/data_len"
```

当前兼容两种绝对路径写法：

- 带根节点名：`/root_name/field/...`
- 不带根节点名：`/field/...`

建议优先使用带根节点名的形式。

#### 3. 固定长度

适合固定长度数组或字节区。

```json
"len_ref": "fixed_8"
```

表示长度固定为 8。

### 5.2 len_mode 的含义

`len_mode` 决定 `len_ref` 的解释方式。

| 模式 | 说明 | 典型场景 |
| --- | --- | --- |
| `count` | 按元素个数展开 | 设备列表、传感器列表 |
| `bytes` | 按字节长度展开 | 变长字节流、payload、原始数据区 |
| `bit` | 按位数换算成字节数 | 位图、bitmask 区域 |
| `condition` | 非 0 解析，为 0 跳过 | 可选头、可选附加字段 |

说明：

- `count`：长度值是多少，就克隆多少个模板节点
- `bytes`：常用于 `child_template` 为 `u8` 的字节流场景
- `bit` 会先按 `(bit_len + 7) / 8` 转成字节数
- `condition` 会把非 0 当作 1，把 0 当作 0

### 5.3 len_formula 的语法

当 `len_ref` 不是最终长度时，可通过 `len_formula` 再计算一次。

当前实现支持的变量和语法如下：

- `len_ref`：引用到的原始长度值
- `curpos`：当前解析偏移位置
- 运算符：`+ - * / ( )`
- 特殊值：`!`

注意：变量名是 `curpos`，不是 `cur_pos`。

示例：

```json
{
    "name": "payload",
    "type": "dynamic_field",
    "len_ref": "../total_length",
    "len_mode": "bytes",
    "len_formula": "len_ref - curpos",
    "child_template": {"name": "byte", "type": "u8"}
}
```

条件反转示例：

```json
{
    "name": "without_ext_header",
    "type": "dynamic_field",
    "len_ref": "../has_ext_header",
    "len_mode": "condition",
    "len_formula": "!",
    "child_template": {
        "name": "default_header",
        "type": "static_field",
        "children": [
            {"name": "default_priority", "type": "u8", "value": "02"}
        ]
    }
}
```

约束：

- 公式中不能直接写其他字段名
- 公式结果必须是非负整数
- 当前是整数四则运算，不支持浮点计算

## 6. 特殊字段与行为

### 6.1 head_flag 的作用

如果根节点的直接子节点中存在名为 `head_flag` 的原子字段，CodecX 会把它当作帧头标记。

行为如下：

- 扫描输入字节流
- 每次找到帧头后，从该位置尝试解析一帧
- 适合连续帧、日志回放、抓包回放

示例：

```json
{
    "name": "fragment_frame",
    "type": "static_field",
    "children": [
        {"name": "head_flag", "type": "u16", "value": 43605},
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

如果没有 `head_flag`，则把整份输入当作一帧处理。

### 6.2 条件字段

`len_mode = condition` 用于表达“是否存在”。

```json
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
            {"name": "timestamp", "type": "u32"}
        ]
    }
}
```

规则：

- `len_ref == 0`：不解析该模板
- `len_ref != 0`：解析一次该模板

### 6.3 LEB128 变长整数

对于 MQTT、Protobuf、TLV 等使用变长整数编码的协议，可直接使用 `leb128`。

```json
{"name": "varint_length", "type": "leb128"}
```

## 7. propdump 使用说明

### 7.1 配置检查

```shell
./propdump proto_config.json
```

重点检查：

- 根节点名称和类型
- `dynamic_field` 的 `len_ref` / `len_mode` / `child_template`
- 嵌套层级是否符合预期

### 7.2 数据解析

```shell
./propdump proto_config.json proto_bytes.txt
```

输入文件支持：

- 十六进制文本，例如 `AA 55 01 02`
- 原始二进制文件

CodecX 会自动判断输入是十六进制文本还是原始二进制，再转成字节流解析。

### 7.3 调试日志

```shell
./propdump proto_config.json proto_bytes.txt --debug
```

调试日志适合定位：

- 字段的起始偏移
- 动态字段的长度计算结果
- 失败前最后一个成功字段

## 8. 代码中如何接入

也可以直接在业务代码中调用 CodecX。

### 8.1 从配置文件创建协议树

```cpp
#include "CFactory.h"

CFactory& factory = CFactory::GetInstance();
std::shared_ptr<CNode> cfg = factory.CreateCfgParserByCfgFile("proto_config.json");
```

### 8.2 按配置解析一帧数据

```cpp
#include "CFactory.h"

std::vector<uint8_t> bytes = {0x78, 0x56, 0x34, 0x12};
CFactory& factory = CFactory::GetInstance();

std::shared_ptr<CNode> cfg = factory.CreateCfgParserByCfgFile("proto_config.json");
std::shared_ptr<CNode> data = factory.CreateDataParserByCfgParser(cfg, bytes);
```

### 8.3 访问解析结果

常见访问流程：

1. 把返回根节点转成 `CField`
2. 用 `GetNode()` 获取目标子节点
3. 把目标节点转成 `CAtom`
4. 用 `GetIntValue()` / `GetVecValue()` / `GetStrValue()` 取值

示例：

```cpp
#include "CAtom.h"
#include "CField.h"

std::shared_ptr<CField> root = std::dynamic_pointer_cast<CField>(data);
std::shared_ptr<CAtom> sensorCnt = std::dynamic_pointer_cast<CAtom>(root->GetNode("sensor_cnt"));

int32_t cnt = 0;
if (sensorCnt) {
    sensorCnt->GetIntValue(cnt);
}
```

## 9. 典型场景示例

### 场景 1：固定长度数组

适用场景：长度写死，不依赖报文字段。

方式 1：静态枚举，适合元素较少时使用。

```json
{
    "name": "array",
    "type": "static_field",
    "children": [
        {"name": "", "type": "u8"},
        {"name": "", "type": "u8"},
        {"name": "", "type": "u8"}
    ]
}
```

方式 2：固定长度动态展开，适合元素较多时减少配置量。

```json
{
    "name": "array",
    "type": "dynamic_field",
    "len_ref": "fixed_6",
    "len_mode": "bytes",
    "child_template": {"name": "byte", "type": "u8"}
}
```

### 场景 2：长度由前置字段决定

```json
{"name": "data_len", "type": "u8"},
{
    "name": "sensor_data",
    "type": "dynamic_field",
    "len_ref": "../data_len",
    "len_mode": "bytes",
    "child_template": {"name": "byte", "type": "u8"}
}
```

### 场景 3：长度需要公式换算

```json
{"name": "total_length", "type": "u16"},
{
    "name": "payload",
    "type": "dynamic_field",
    "len_ref": "../total_length",
    "len_mode": "bytes",
    "len_formula": "(len_ref - curpos - 1) / 1",
    "child_template": {"name": "byte", "type": "u8"}
}
```

### 场景 4：嵌套动态数组

```json
{"name": "device_cnt", "type": "u8"},
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
            {"name": "sensor_cnt", "type": "u8"},
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

### 场景 5：通过绝对路径跨层引用长度

```json
{
    "name": "sensor_list",
    "type": "dynamic_field",
    "len_ref": "/simple_device/sensor_cnt",
    "len_mode": "count",
    "child_template": {
        "name": "single_sensor",
        "type": "static_field",
        "children": [
            {"name": "data_len", "type": "u8"},
            {
                "name": "sensor_data",
                "type": "dynamic_field",
                "len_ref": "/simple_device/sensor_list/single_sensor/data_len",
                "len_mode": "bytes",
                "child_template": {"name": "byte", "type": "u8"}
            }
        ]
    }
}
```

### 场景 6：剩余字节全部解析

```json
{"name": "total_length", "type": "u16"},
{
    "name": "payload",
    "type": "dynamic_field",
    "len_ref": "../total_length",
    "len_mode": "bytes",
    "len_formula": "len_ref - curpos",
    "child_template": {"name": "byte", "type": "u8"}
}
```

### 场景 7：条件可选结构

```json
{
    "name": "standard_message",
    "type": "static_field",
    "children": [
        {"name": "has_ext_header", "type": "u8"},
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
                    {"name": "timestamp", "type": "u32"}
                ]
            }
        }
    ]
}
```

## 10. 最佳实践

### 10.1 建模建议

- 根节点优先使用 `static_field`
- 固定结构优先用 `static_field`
- 数组、列表、payload 区优先用 `dynamic_field`
- `bytes` 模式建议配合 `u8` 模板使用
- 同级字段命名建议唯一

### 10.2 长度引用建议

- 同层或父层引用优先用相对路径
- 跨多层引用优先用绝对路径
- 复杂协议优先使用带根节点名的绝对路径

### 10.3 调试建议

- 新配置先跑一次“仅配置打印”
- 再用最短的一帧真实报文做验证
- 动态字段出问题时，优先检查 `len_ref`、`len_mode`、`len_formula`
- 如果是流式日志或多帧拼接数据，优先考虑是否需要 `head_flag`

## 11. 常见问题

### 11.1 为什么动态字段没有解析出来？

优先检查：

- `len_ref` 路径是否写对
- `len_mode` 是否与实际语义一致
- `len_formula` 是否写成了 `cur_pos`，而不是实现实际使用的 `curpos`
- `child_template` 是否存在

### 11.2 为什么配置能打印，数据解析却失败？

这通常说明：

- 配置结构本身可以解析
- 但字段长度、字段顺序或字段类型与真实报文不一致

排查顺序建议：

1. 看失败前最后一个成功字段
2. 检查该字段之后的长度字段是否配置正确
3. 检查是否少了 `head_flag` 或帧头值写错

### 11.3 应该什么时候用绝对路径？

当动态字段嵌套较深，或者长度字段来自根节点下的其他分支时，优先用绝对路径。

