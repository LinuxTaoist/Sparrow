/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : 13_Codecx.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/05/05
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/05/05 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <CFactory.h>
#include <iostream>
#include <memory>

#define SPR_LOGD(fmt, args...) printf("%d DebugCodecX D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d DebugCodecX E: " fmt, __LINE__, ##args)

#define TEST_JSON_STR1 "{                                 \
    \"name\": \"iot_devices\",                           \
    \"type\": \"static_field\",                          \
    \"children\": [                                      \
        {\"name\": \"gateway_id\",  \"type\": \"u32\"},   \
        {\"name\": \"report_time\", \"type\": \"u32\"},  \
        {\"name\": \"dev_cnt\",     \"type\": \"u16\"},  \
        {                                                 \
            \"name\": \"device_list\",                    \
            \"type\": \"dynamic_field\",                  \
            \"len_ref\": \"iot_devices.dev_cnt\",         \
            \"len_mode\": \"count\",                      \
            \"child_template\": {                         \
                \"name\": \"single_device\",              \
                \"type\": \"static_field\",               \
                \"children\": [                           \
                    {\"name\": \"dev_type\", \"type\": \"u8\"}, \
                    {\"name\": \"dev_name_len\", \"type\": \"u8\"}, \
                    {                                    \
                        \"name\": \"dev_name\",           \
                        \"type\": \"dynamic_field\",      \
                        \"len_ref\": \"dev_name_len\",    \
                        \"len_mode\": \"byte\",           \
                        \"child_template\": {\"name\": \"index\", \"type\": \"u8\"} \
                    },                                   \
                    {\"name\": \"dev_desc_len\", \"type\": \"u8\"}, \
                    {                                    \
                        \"name\": \"dev_desc\",           \
                        \"type\": \"dynamic_field\",      \
                        \"len_ref\": \"dev_desc_len\",    \
                        \"len_mode\": \"byte\",           \
                        \"child_template\": {\"name\": \"index\", \"type\": \"u8\"} \
                    },                                   \
                    {\"name\": \"dev_vendor\", \"type\": \"u8\"}, \
                    {\"name\": \"custom_attr_cnt\", \"type\": \"u8\"}, \
                    {                                    \
                        \"name\": \"custom_attr_list\",   \
                        \"type\": \"dynamic_field\",     \
                        \"len_ref\": \"custom_attr_cnt\", \
                        \"len_mode\": \"count\",         \
                        \"child_template\": {             \
                            \"name\": \"single_attr\",    \
                            \"type\": \"static_field\",   \
                            \"children\": [               \
                                {\"name\": \"product_date\", \"type\": \"u32\"}, \
                                {\"name\": \"product_id\", \"type\": \"u16\"} \
                            ]                            \
                        }                                \
                    }                                    \
                ]                                        \
            }                                            \
        },                                               \
        {\"name\": \"crc8\", \"type\": \"u8\"}           \
    ]                                                    \
}"

#define TEST_JSON_STR2 "{                               \
    \"name\": \"simple_device\",                        \
    \"type\": \"static_field\",                         \
    \"children\": [                                     \
        {\"name\": \"dev_id\",    \"type\": \"u32\"},   \
        {\"name\": \"dev_type\",  \"type\": \"u8\"},    \
        {\"name\": \"status\",    \"type\": \"u8\"},    \
        {\"name\": \"voltage\",   \"type\": \"u16\"},   \
        {                                                 \
            \"name\": \"base_info\", \"type\": \"static_field\", \
            \"children\": [                               \
                {\"name\": \"hw_ver\",  \"type\": \"u16\"},  \
                {\"name\": \"sw_ver\",  \"type\": \"u16\"},  \
                {\"name\": \"chip_id\", \"type\": \"u32\"}   \
            ]                                            \
        },                                               \
        {\"name\": \"sensor_cnt\", \"type\": \"u8\"},   \
        {                                                 \
            \"name\": \"sensor_list\", \"type\": \"dynamic_field\", \
            \"len_ref\": \"sensor_cnt\",   \
            \"len_mode\": \"count\",                     \
            \"child_template\": {                         \
                \"name\": \"single_sensor\", \"type\": \"static_field\", \
                \"children\": [                           \
                    {\"name\": \"sensor_id\",   \"type\": \"u8\"}, \
                    {\"name\": \"sensor_type\", \"type\": \"u8\"}, \
                    {\"name\": \"data_len\",    \"type\": \"u8\"}, \
                    {                                     \
                        \"name\": \"sensor_data\", \"type\": \"dynamic_field\", \
                        \"len_ref\": \"data_len\",        \
                        \"len_mode\": \"byte\",           \
                        \"child_template\": {\"name\": \"data_byte\", \"type\": \"u8\"} \
                    }                                     \
                ]                                         \
            }                                             \
        }                                                 \
    ]                                                     \
}"

// 协议解析（总长度：42 字节）：
// [0-3]    dev_id      : 0x12345678 -> [0x78, 0x56, 0x34, 0x12]
// [4]      dev_type    : 0x05
// [5]      status      : 0x01
// [6-7]    voltage     : 0x0BB8 (3000mV) -> [0xB8, 0x0B]
// [8-9]    base_info.hw_ver : 0x0100 -> [0x00, 0x01]
// [10-11]  base_info.sw_ver : 0x0201 -> [0x01, 0x02]
// [12-15]  base_info.chip_id: 0xAABBCCDD -> [0xDD, 0xCC, 0xBB, 0xAA]
// [16]     sensor_cnt  : 0x02 (2个传感器)
// [17-30]  sensor[0]   :
//          [17]    sensor_id   : 0x01
//          [18]    sensor_type : 0x03 (温度)
//          [19]    data_len    : 0x04
//          [20-23] sensor_data : [0x10, 0x20, 0x30, 0x40]
// [31-41]  sensor[1]   :
//          [31]    sensor_id   : 0x02
//          [32]    sensor_type : 0x05 (湿度)
//          [33]    data_len    : 0x02
//          [34-35] sensor_data : [0x50, 0x60]
static const std::vector<uint8_t> TEST_BIN_BYTES2 = {
    // dev_id, dev_type, status, voltage
    0x78, 0x56, 0x34, 0x12, 0x05, 0x01, 0xB8, 0x0B,
    // base_info (static_field 嵌套)
    0x00, 0x01, 0x01, 0x02, 0xDD, 0xCC, 0xBB, 0xAA,
    // sensor_cnt
    0x02,
    // sensor[0] (dynamic_field 嵌套 static_field)
    0x01, 0x03, 0x04, 0x10, 0x20, 0x30, 0x40,
    // sensor[1] (dynamic_field 嵌套 dynamic_field)
    0x02, 0x05, 0x02, 0x50, 0x60
};

int main(int argc, char* argv[])
{
    char val = 0;
    bool run = true;
    do {
        SPR_LOGD("Input: \n");
        std::cin >> val;
        switch(val) {
            case '1': {
                CFactory& theFactory = CFactory::GetInstance();
                std::shared_ptr<CNode> pParser = theFactory.CreateParserWithJString(TEST_JSON_STR1);
                if (!pParser)  {
                    SPR_LOGE("CreateParserWithJString failed! \n");
                    return -1;
                }

                theFactory.PrintAllNodes(pParser);
                break;
            }
            case '2': {
                CFactory& theFactory = CFactory::GetInstance();
                std::shared_ptr<CNode> pParser = theFactory.CreateParserWithJString(TEST_JSON_STR2);
                if (!pParser)  {
                    SPR_LOGE("CreateParserWithJString failed! \n");
                    return -1;
                }
                pParser->Decode(TEST_BIN_BYTES2);
                theFactory.PrintAllNodes(pParser);
                break;
            }
            case 'q':
            case 'Q':
                run = false;
                break;
            default:
                break;
        }
    } while(run);

    return 0;
}
