/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestConfig.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/06/20
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/06/20 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <unistd.h>
#include <atomic>
#include <map>
#include <string>
#include "gtest/gtest.h"
#include "Config.h"


namespace {

bool WaitFile(const std::string& path, int32_t timeoutMs)
{
    int32_t loop = timeoutMs / 50;
    for (int32_t i = 0; i < loop; ++i) {
        if (access(path.c_str(), F_OK) == 0) {
            return true;
        }
        usleep(50 * 1000);
    }

    return false;
}

std::string MakeUniqueNamespace(const char* base)
{
    static std::atomic<uint32_t> seq(0);
    return std::string(base) + "_" + std::to_string(getpid()) + "_" + std::to_string(seq.fetch_add(1));
}

} // namespace

class TestConfigApi : public ::testing::Test
{
public:
    static void SetUpTestCase()
    {
        ASSERT_TRUE(WaitFile("/tmp/bindermanagersrv", 3000));
        ASSERT_TRUE(WaitFile("/tmp/configmanagersrv", 3000));
        usleep(200 * 1000);
    }
};

TEST_F(TestConfigApi, SetAndGetUserValue)
{
    Config* cfg = Config::GetInstance();
    ASSERT_TRUE(cfg != nullptr);
    std::string nameSpace = MakeUniqueNamespace("cloud");

    int32_t revision = 0;
    ASSERT_EQ(cfg->SetValueWithScope(nameSpace, "cursor", "100", CONFIG_SCOPE_USER, revision), 0);
    EXPECT_GT(revision, 0);

    std::string value;
    int32_t scope = CONFIG_SCOPE_DEFAULT;
    int32_t queryRevision = 0;
    ASSERT_EQ(cfg->GetValue(nameSpace, "cursor", value, "", scope, queryRevision), 0);
    EXPECT_EQ(value, "100");
    EXPECT_EQ(scope, CONFIG_SCOPE_USER);
    EXPECT_EQ(queryRevision, revision);
}

TEST_F(TestConfigApi, LayeredFallbackWorks)
{
    Config* cfg = Config::GetInstance();
    ASSERT_TRUE(cfg != nullptr);
    std::string nameSpace = MakeUniqueNamespace("net");

    ASSERT_EQ(cfg->SetValueWithScope(nameSpace, "apn", "default_apn", CONFIG_SCOPE_DEFAULT), 0);
    ASSERT_EQ(cfg->SetValueWithScope(nameSpace, "apn", "factory_apn", CONFIG_SCOPE_FACTORY), 0);

    std::string value;
    int32_t scope = CONFIG_SCOPE_DEFAULT;
    ASSERT_EQ(cfg->GetValue(nameSpace, "apn", value, "", scope), 0);
    EXPECT_EQ(value, "factory_apn");
    EXPECT_EQ(scope, CONFIG_SCOPE_FACTORY);

    ASSERT_EQ(cfg->SetValueWithScope(nameSpace, "apn", "user_apn", CONFIG_SCOPE_USER), 0);
    ASSERT_EQ(cfg->GetValue(nameSpace, "apn", value, "", scope), 0);
    EXPECT_EQ(value, "user_apn");
    EXPECT_EQ(scope, CONFIG_SCOPE_USER);
}

TEST_F(TestConfigApi, ListNamespaceReturnsEffectiveValues)
{
    Config* cfg = Config::GetInstance();
    ASSERT_TRUE(cfg != nullptr);
    std::string nameSpace = MakeUniqueNamespace("veh");

    ASSERT_EQ(cfg->SetValueWithScope(nameSpace, "vin", "VIN_DEFAULT", CONFIG_SCOPE_DEFAULT), 0);
    ASSERT_EQ(cfg->SetValueWithScope(nameSpace, "vin", "VIN_USER", CONFIG_SCOPE_USER), 0);
    ASSERT_EQ(cfg->SetValueWithScope(nameSpace, "mode", "eco", CONFIG_SCOPE_USER), 0);

    std::map<std::string, std::string> items;
    ASSERT_EQ(cfg->ListNamespace(nameSpace, items), 0);
    EXPECT_EQ(items["vin"], "VIN_USER");
    EXPECT_EQ(items["mode"], "eco");
}

TEST_F(TestConfigApi, BackupAndMetaWork)
{
    Config* cfg = Config::GetInstance();
    ASSERT_TRUE(cfg != nullptr);
    std::string nameSpace = MakeUniqueNamespace("ota");

    int32_t writeRevision = 0;
    ASSERT_EQ(cfg->SetValueWithScope(nameSpace, "policy", "strict", CONFIG_SCOPE_USER, writeRevision), 0);

    int32_t scope = CONFIG_SCOPE_DEFAULT;
    int32_t metaRevision = 0;
    ASSERT_EQ(cfg->GetMeta(nameSpace, "policy", scope, metaRevision), 0);
    EXPECT_EQ(scope, CONFIG_SCOPE_USER);
    EXPECT_EQ(metaRevision, writeRevision);

    ASSERT_EQ(cfg->Backup(), 0);
    EXPECT_EQ(access("/tmp/config_manager.db.bak", F_OK), 0);
}

TEST_F(TestConfigApi, TypedValueApisWork)
{
    Config* cfg = Config::GetInstance();
    ASSERT_TRUE(cfg != nullptr);
    std::string nameSpace = MakeUniqueNamespace("typed");

    ASSERT_EQ(cfg->SetIntValue("i32", static_cast<int32_t>(-123), CONFIG_SCOPE_USER, nameSpace), 0);

    int32_t i32 = 0;
    ASSERT_EQ(cfg->GetIntValue("i32", i32, static_cast<int32_t>(7), nameSpace), 0);
    EXPECT_EQ(i32, -123);

    ASSERT_EQ(cfg->SetInt64Value("i64", static_cast<int64_t>(1234567890123LL), CONFIG_SCOPE_USER, nameSpace), 0);
    int64_t i64 = 0;
    ASSERT_EQ(cfg->GetInt64Value("i64", i64, static_cast<int64_t>(1), nameSpace), 0);
    EXPECT_EQ(i64, 1234567890123LL);

    ASSERT_EQ(cfg->SetBoolValue("flag", true, CONFIG_SCOPE_USER, nameSpace), 0);
    bool flag = false;
    ASSERT_EQ(cfg->GetBoolValue("flag", flag, false, nameSpace), 0);
    EXPECT_TRUE(flag);

    ASSERT_EQ(cfg->SetFloatValue("f32", 3.25f, CONFIG_SCOPE_USER, nameSpace), 0);
    float f32 = 0.0f;
    ASSERT_EQ(cfg->GetFloatValue("f32", f32, 0.0f, nameSpace), 0);
    EXPECT_NEAR(f32, 3.25f, 0.0001f);

    ASSERT_EQ(cfg->SetDoubleValue("f64", 9.875, CONFIG_SCOPE_USER, nameSpace), 0);
    double f64 = 0.0;
    ASSERT_EQ(cfg->GetDoubleValue("f64", f64, 0.0, nameSpace), 0);
    EXPECT_NEAR(f64, 9.875, 0.0000001);
}