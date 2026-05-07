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

int main(int argc, char* argv[])
{
    char val = 0;
    bool run = true;
    do {
        SPR_LOGD("Input: ");
        std::cin >> val;
        switch(val) {
            case '1': {
                CFactory& theFactory = CFactory::GetInstance();
                std::shared_ptr<CNode> pRoot = theFactory.LoadJsonString(TEST_JSON_STR1);
                if (!pRoot)  {
                    SPR_LOGE("LoadJsonString failed! \n");
                    return -1;
                }

                theFactory.PrintAllNodes(pRoot);
                break;
            }
            case '2':
                break;
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
