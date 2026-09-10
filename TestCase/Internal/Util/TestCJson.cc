/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestCJson.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : cJSON 解析/构建/打印/操作内部测试
 *  @date       : 2026/09/09
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/09/09 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include "cJSON.h"
#include "gtest/gtest.h"

// 辅助：解析字符串并返回 item（自动 delete 由测试负责）
static cJSON* Parse(const char* text) {
    return cJSON_Parse(text);
}

// 测试版本号返回非空
TEST(Util_cJSON, Version)
{
    EXPECT_NE(cJSON_Version(), nullptr);
    EXPECT_GT(strlen(cJSON_Version()), 0u);
}

// 测试解析 null 与各种非法输入
TEST(Util_cJSON, ParseNullAndInvalid)
{
    EXPECT_EQ(cJSON_Parse(nullptr), nullptr);
    EXPECT_EQ(cJSON_Parse(""), nullptr);
    EXPECT_EQ(cJSON_Parse("not json"), nullptr);
    EXPECT_EQ(cJSON_Parse("{"), nullptr);
    EXPECT_EQ(cJSON_Parse("["), nullptr);
    // 无效 JSON 片段
    cJSON* bad = cJSON_Parse("{]");
    EXPECT_EQ(bad, nullptr);
}

// 测试解析基础类型（数字/字符串/布尔/null）
TEST(Util_cJSON, ParseBasicTypes)
{
    // number
    cJSON* n = Parse("42");
    ASSERT_TRUE(n != nullptr);
    EXPECT_TRUE(cJSON_IsNumber(n));
    EXPECT_DOUBLE_EQ(cJSON_GetNumberValue(n), 42.0);
    cJSON_Delete(n);

    // string
    cJSON* s = Parse("\"hello\"");
    ASSERT_TRUE(s != nullptr);
    EXPECT_TRUE(cJSON_IsString(s));
    EXPECT_STREQ(cJSON_GetStringValue(s), "hello");
    cJSON_Delete(s);

    // bool
    cJSON* b = Parse("true");
    ASSERT_TRUE(b != nullptr);
    EXPECT_TRUE(cJSON_IsBool(b));
    EXPECT_TRUE(cJSON_IsTrue(b));
    cJSON_Delete(b);

    cJSON* bf = Parse("false");
    ASSERT_TRUE(bf != nullptr);
    EXPECT_TRUE(cJSON_IsFalse(bf));
    cJSON_Delete(bf);

    // null
    cJSON* nu = Parse("null");
    ASSERT_TRUE(nu != nullptr);
    EXPECT_TRUE(cJSON_IsNull(nu));
    cJSON_Delete(nu);
}

// 测试解析数组及其元素访问
TEST(Util_cJSON, ParseArray)
{
    cJSON* arr = Parse("[1, 2, 3, \"four\", true]");
    ASSERT_TRUE(arr != nullptr);
    EXPECT_TRUE(cJSON_IsArray(arr));
    EXPECT_EQ(cJSON_GetArraySize(arr), 5);

    EXPECT_DOUBLE_EQ(cJSON_GetNumberValue(cJSON_GetArrayItem(arr, 0)), 1.0);
    EXPECT_DOUBLE_EQ(cJSON_GetNumberValue(cJSON_GetArrayItem(arr, 1)), 2.0);
    EXPECT_DOUBLE_EQ(cJSON_GetNumberValue(cJSON_GetArrayItem(arr, 2)), 3.0);
    EXPECT_STREQ(cJSON_GetStringValue(cJSON_GetArrayItem(arr, 3)), "four");
    EXPECT_TRUE(cJSON_IsTrue(cJSON_GetArrayItem(arr, 4)));
    EXPECT_EQ(cJSON_GetArrayItem(arr, 100), nullptr);

    cJSON_Delete(arr);
}

// 测试解析嵌套对象及其字段
TEST(Util_cJSON, ParseNestedObject)
{
    const char* text = "{\"name\":\"sparrow\",\"version\":1.8,\"tags\":[\"a\",\"b\"],\"meta\":{\"ok\":true}}";
    cJSON* obj = Parse(text);
    ASSERT_TRUE(obj != nullptr);
    EXPECT_TRUE(cJSON_IsObject(obj));

    cJSON* name = cJSON_GetObjectItem(obj, "name");
    EXPECT_STREQ(cJSON_GetStringValue(name), "sparrow");

    cJSON* ver = cJSON_GetObjectItem(obj, "version");
    EXPECT_DOUBLE_EQ(cJSON_GetNumberValue(ver), 1.8);

    cJSON* tags = cJSON_GetObjectItem(obj, "tags");
    EXPECT_EQ(cJSON_GetArraySize(tags), 2);

    cJSON* meta = cJSON_GetObjectItem(obj, "meta");
    EXPECT_TRUE(cJSON_IsTrue(cJSON_GetObjectItem(meta, "ok")));

    // 不存在的 key
    EXPECT_EQ(cJSON_GetObjectItem(obj, "nope"), nullptr);

    cJSON_Delete(obj);
}

// 测试创建对象并打印/重解析
TEST(Util_cJSON, CreateAndPrint)
{
    cJSON* root = cJSON_CreateObject();
    ASSERT_TRUE(root != nullptr);

    cJSON_AddItemToObject(root, "name", cJSON_CreateString("unit-test"));
    cJSON_AddItemToObject(root, "count", cJSON_CreateNumber(3));
    cJSON_AddItemToObject(root, "ok", cJSON_CreateBool(1));
    cJSON_AddItemToObject(root, "nothing", cJSON_CreateNull());

    cJSON* arr = cJSON_CreateArray();
    cJSON_AddItemToArray(arr, cJSON_CreateNumber(1));
    cJSON_AddItemToArray(arr, cJSON_CreateNumber(2));
    cJSON_AddItemToObject(root, "list", arr);

    char* printed = cJSON_Print(root);
    ASSERT_TRUE(printed != nullptr);
    char* unformatted = cJSON_PrintUnformatted(root);
    ASSERT_TRUE(unformatted != nullptr);

    // 打印结果能重新解析
    cJSON* reparsed = cJSON_Parse(printed);
    ASSERT_TRUE(reparsed != nullptr);
    EXPECT_TRUE(cJSON_IsObject(reparsed));
    EXPECT_STREQ(cJSON_GetStringValue(cJSON_GetObjectItem(reparsed, "name")), "unit-test");

    cJSON_Delete(reparsed);
    free(printed);
    free(unformatted);
    cJSON_Delete(root);
}

// 测试缓冲打印与预分配打印
TEST(Util_cJSON, PrintBufferedAndPreallocated)
{
    cJSON* obj = cJSON_CreateObject();
    cJSON_AddItemToObject(obj, "k", cJSON_CreateString("v"));

    char* buffered = cJSON_PrintBuffered(obj, 128, 0);
    ASSERT_TRUE(buffered != nullptr);
    cJSON_Delete(obj);
    free(buffered);

    // PrintPreallocated：缓冲区足够
    cJSON* obj2 = cJSON_CreateObject();
    cJSON_AddItemToObject(obj2, "a", cJSON_CreateNumber(1));
    char buf[256] = {};
    EXPECT_TRUE(cJSON_PrintPreallocated(obj2, buf, sizeof(buf), 0));
    EXPECT_GT(strlen(buf), 0u);
    cJSON_Delete(obj2);

    // PrintPreallocated：缓冲区不足 -> false
    cJSON* obj3 = cJSON_CreateString("a fairly long string value for preallocated buffer overflow test");
    char small[8] = {};
    EXPECT_FALSE(cJSON_PrintPreallocated(obj3, small, sizeof(small), 0));
    cJSON_Delete(obj3);
}

// 测试添加/删除对象成员
TEST(Util_cJSON, AddItemAndDelete)
{
    cJSON* obj = cJSON_CreateObject();
    cJSON_AddItemToObject(obj, "a", cJSON_CreateNumber(1));
    cJSON_AddItemToObject(obj, "b", cJSON_CreateNumber(2));
    cJSON_AddItemToObjectCS(obj, "c", cJSON_CreateNumber(3));

    EXPECT_EQ(cJSON_GetObjectItem(obj, "a") != nullptr, 1);
    EXPECT_EQ(cJSON_GetObjectItem(obj, "A") != nullptr, 1);   // 大小写不敏感默认
    EXPECT_EQ(cJSON_GetObjectItemCaseSensitive(obj, "A"), nullptr);

    cJSON_DeleteItemFromObject(obj, "b");
    EXPECT_EQ(cJSON_GetObjectItem(obj, "b"), nullptr);

    cJSON_DeleteItemFromObjectCaseSensitive(obj, "c");
    EXPECT_EQ(cJSON_GetObjectItem(obj, "c"), nullptr);

    cJSON_Delete(obj);
}

// 测试数组元素的增删操作
TEST(Util_cJSON, ArrayManipulation)
{
    cJSON* arr = cJSON_CreateArray();
    cJSON_AddItemToArray(arr, cJSON_CreateNumber(10));
    cJSON_AddItemToArray(arr, cJSON_CreateNumber(20));
    cJSON_AddItemToArray(arr, cJSON_CreateNumber(30));
    EXPECT_EQ(cJSON_GetArraySize(arr), 3);

    cJSON_DeleteItemFromArray(arr, 1);
    EXPECT_EQ(cJSON_GetArraySize(arr), 2);
    EXPECT_DOUBLE_EQ(cJSON_GetNumberValue(cJSON_GetArrayItem(arr, 1)), 30.0);

    // 越界删除不崩溃
    cJSON_DeleteItemFromArray(arr, 100);

    cJSON_Delete(arr);
}

// 测试按长度/选项解析
TEST(Util_cJSON, ParseWithLengthAndOpts)
{
    // ParseWithLength：在 buffer 中截断
    const char* text = "[1,2,3] trailing-garbage";
    cJSON* item = cJSON_ParseWithLength(text, 7);
    ASSERT_TRUE(item != nullptr);
    EXPECT_EQ(cJSON_GetArraySize(item), 3);
    cJSON_Delete(item);

    // ParseWithOpts：返回解析结束位置
    const char* text2 = "{} extra";
    const char* end = nullptr;
    cJSON* item2 = cJSON_ParseWithOpts(text2, &end, 0);
    ASSERT_TRUE(item2 != nullptr);
    EXPECT_EQ(end, text2 + 2);
    cJSON_Delete(item2);

    // require_null_terminated
    cJSON* item3 = cJSON_ParseWithOpts("{\"a\":1}", nullptr, 1);
    ASSERT_TRUE(item3 != nullptr);
    cJSON_Delete(item3);
}

// 测试转义与 Unicode 字符串解析
TEST(Util_cJSON, EscapeAndUnicodeString)
{
    const char* text = "\"line1\\nline2\\t\\\"quoted\\\" \\u0041\"";
    cJSON* s = Parse(text);
    ASSERT_TRUE(s != nullptr);
    EXPECT_TRUE(cJSON_IsString(s));

    char* printed = cJSON_Print(s);
    ASSERT_TRUE(printed != nullptr);
    cJSON* reparsed = cJSON_Parse(printed);
    ASSERT_TRUE(reparsed != nullptr);
    EXPECT_STREQ(cJSON_GetStringValue(reparsed), cJSON_GetStringValue(s));

    free(printed);
    cJSON_Delete(reparsed);
    cJSON_Delete(s);
}

// 测试各种数字格式解析
TEST(Util_cJSON, NumberFormats)
{
    // 整数、负数、浮点、科学计数法
    cJSON* a = Parse("0");
    EXPECT_DOUBLE_EQ(cJSON_GetNumberValue(a), 0.0);
    cJSON_Delete(a);

    cJSON* b = Parse("-3.14");
    EXPECT_DOUBLE_EQ(cJSON_GetNumberValue(b), -3.14);
    cJSON_Delete(b);

    cJSON* c = Parse("1e3");
    EXPECT_DOUBLE_EQ(cJSON_GetNumberValue(c), 1000.0);
    cJSON_Delete(c);

    cJSON* d = Parse("2.5e-2");
    EXPECT_DOUBLE_EQ(cJSON_GetNumberValue(d), 0.025);
    cJSON_Delete(d);
}

// 测试 IsInvalid 对空指针与 Invalid 类型的判断
TEST(Util_cJSON, IsInvalidOnNull)
{
    // cJSON_IsInvalid(nullptr) 返回 false（null 不是 Invalid 类型）
    EXPECT_FALSE(cJSON_IsInvalid(nullptr));

    // 构造一个 Invalid 类型的 item 直接验证
    cJSON invalidItem = {};
    invalidItem.type = cJSON_Invalid;
    EXPECT_TRUE(cJSON_IsInvalid(&invalidItem));
}
