/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestCalculator.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/05/15
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/05/15 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <string.h>
#include "gtest/gtest.h"
#include "Calculator.h"

using namespace Calculator;

// 测试基础加法运算
TEST(Util_CalculateFromString, BasicAddition) {
    int32_t result;
    EXPECT_EQ(CalculateFromString("1+2", result), 0);
    EXPECT_EQ(result, 3);

    EXPECT_EQ(CalculateFromString("0+0", result), 0);
    EXPECT_EQ(result, 0);

    EXPECT_EQ(CalculateFromString("100+200+300", result), 0);
    EXPECT_EQ(result, 600);
}

// 测试基础减法运算
TEST(Util_CalculateFromString, BasicSubtraction) {
    int32_t result;
    EXPECT_EQ(CalculateFromString("5-3", result), 0);
    EXPECT_EQ(result, 2);

    EXPECT_EQ(CalculateFromString("10-5-2", result), 0);
    EXPECT_EQ(result, 3);

    EXPECT_EQ(CalculateFromString("0-5", result), 0);
    EXPECT_EQ(result, -5);
}

// 测试基础乘法运算
TEST(Util_CalculateFromString, BasicMultiplication) {
    int32_t result;
    EXPECT_EQ(CalculateFromString("4*5", result), 0);
    EXPECT_EQ(result, 20);

    EXPECT_EQ(CalculateFromString("2*3*4", result), 0);
    EXPECT_EQ(result, 24);

    EXPECT_EQ(CalculateFromString("0*100", result), 0);
    EXPECT_EQ(result, 0);
}

// 测试基础除法运算
TEST(Util_CalculateFromString, BasicDivision) {
    int32_t result;
    EXPECT_EQ(CalculateFromString("10/2", result), 0);
    EXPECT_EQ(result, 5);

    EXPECT_EQ(CalculateFromString("20/2/5", result), 0);
    EXPECT_EQ(result, 2);

    // 整数除法取整
    EXPECT_EQ(CalculateFromString("10/3", result), 0);
    EXPECT_EQ(result, 3);
}

// 测试运算优先级：乘除优先于加减
TEST(Util_CalculateFromString, OperationPriority) {
    int32_t result;
    EXPECT_EQ(CalculateFromString("1+2*3", result), 0);
    EXPECT_EQ(result, 7);

    EXPECT_EQ(CalculateFromString("10-4/2", result), 0);
    EXPECT_EQ(result, 8);

    EXPECT_EQ(CalculateFromString("2*3+4*5", result), 0);
    EXPECT_EQ(result, 26);

    EXPECT_EQ(CalculateFromString("10/2+3*4", result), 0);
    EXPECT_EQ(result, 17);

    EXPECT_EQ(CalculateFromString("100-20*3+10/5", result), 0);
    EXPECT_EQ(result, 42);
}

// 测试单层括号处理
TEST(Util_CalculateFromString, SingleBracket) {
    int32_t result;
    EXPECT_EQ(CalculateFromString("(1+2)*3", result), 0);
    EXPECT_EQ(result, 9);

    EXPECT_EQ(CalculateFromString("10/(2+3)", result), 0);
    EXPECT_EQ(result, 2);

    EXPECT_EQ(CalculateFromString("(10-5)*2", result), 0);
    EXPECT_EQ(result, 10);

    // 括号后紧跟运算符（之前的bug场景）
    EXPECT_EQ(CalculateFromString("(41-35-3)/1", result), 0);
    EXPECT_EQ(result, 3);
}

// 测试嵌套括号处理
TEST(Util_CalculateFromString, NestedBracket) {
    int32_t result;
    EXPECT_EQ(CalculateFromString("((1+2)*3)-4", result), 0);
    EXPECT_EQ(result, 5);

    EXPECT_EQ(CalculateFromString("2*(3+4*5)", result), 0);
    EXPECT_EQ(result, 46);

    EXPECT_EQ(CalculateFromString("(10-(2+3))*4", result), 0);
    EXPECT_EQ(result, 20);

    EXPECT_EQ(CalculateFromString("((100/2)+(3*4))/2", result), 0);
    EXPECT_EQ(result, 31);
}

// 测试多个括号并列
TEST(Util_CalculateFromString, MultipleBrackets) {
    int32_t result;
    EXPECT_EQ(CalculateFromString("(1+2)*(3+4)", result), 0);
    EXPECT_EQ(result, 21);

    EXPECT_EQ(CalculateFromString("(10-5)*(8/2)", result), 0);
    EXPECT_EQ(result, 20);

    EXPECT_EQ(CalculateFromString("(1+2*3)+(4*5-6)", result), 0);
    EXPECT_EQ(result, 21);
}

// 测试空格处理（任意位置空格不影响结果）
TEST(Util_CalculateFromString, SpaceHandling) {
    int32_t result;
    EXPECT_EQ(CalculateFromString(" 1 + 2 * 3 ", result), 0);
    EXPECT_EQ(result, 7);

    EXPECT_EQ(CalculateFromString("( 1 + 2 ) * 3", result), 0);
    EXPECT_EQ(result, 9);

    EXPECT_EQ(CalculateFromString("10   /   ( 2 + 3 )", result), 0);
    EXPECT_EQ(result, 2);

    EXPECT_EQ(CalculateFromString("  ( ( 1 + 2 ) * 3 ) - 4  ", result), 0);
    EXPECT_EQ(result, 5);
}

// 测试负号处理
TEST(Util_CalculateFromString, NegativeNumber) {
    int32_t result;
    // 开头负号
    EXPECT_EQ(CalculateFromString("-1+2", result), 0);
    EXPECT_EQ(result, 1);

    // 括号后的负号
    EXPECT_EQ(CalculateFromString("(-1+2)*3", result), 0);
    EXPECT_EQ(result, 3);

    // 负号与运算符结合
    EXPECT_EQ(CalculateFromString("10-(-5)", result), 0);
    EXPECT_EQ(result, 15);

    // 负数参与运算
    EXPECT_EQ(CalculateFromString("-10*2+5", result), 0);
    EXPECT_EQ(result, -15);

    EXPECT_EQ(CalculateFromString("(-10)/(-2)", result), 0);
    EXPECT_EQ(result, 5);
}

// 测试边界值
TEST(Util_CalculateFromString, BoundaryValues) {
    int32_t result;
    // 零值运算
    EXPECT_EQ(CalculateFromString("0*0", result), 0);
    EXPECT_EQ(result, 0);

    EXPECT_EQ(CalculateFromString("0/100", result), 0);
    EXPECT_EQ(result, 0);

    // 大数运算
    EXPECT_EQ(CalculateFromString("1000000+2000000", result), 0);
    EXPECT_EQ(result, 3000000);

    EXPECT_EQ(CalculateFromString("1000000*1000", result), 0);
    EXPECT_EQ(result, 1000000000);
}

// 测试错误情况：空表达式
TEST(Util_CalculateFromString, ErrorEmptyExpression) {
    int32_t result;
    EXPECT_EQ(CalculateFromString("", result), -1);
    EXPECT_EQ(result, 0);
}

// 测试错误情况：无效字符
TEST(Util_CalculateFromString, ErrorInvalidCharacter) {
    int32_t result;
    EXPECT_EQ(CalculateFromString("1+2a", result), -2);
    EXPECT_EQ(CalculateFromString("10$5", result), -2);
    EXPECT_EQ(CalculateFromString("(1+2)#3", result), -2);
}

// 测试错误情况：除零错误
TEST(Util_CalculateFromString, ErrorDivideByZero) {
    int32_t result;
    EXPECT_EQ(CalculateFromString("10/0", result), -3);
    EXPECT_EQ(CalculateFromString("(1+2)/0", result), -3);
    EXPECT_EQ(CalculateFromString("10/(5-5)", result), -3);
}

// 测试错误情况：括号不匹配
TEST(Util_CalculateFromString, ErrorBracketMismatch) {
    int32_t result;
    // 左括号多
    EXPECT_EQ(CalculateFromString("(1+2", result), -4);
    EXPECT_EQ(CalculateFromString("((1+2)*3", result), -4);

    // 右括号多
    EXPECT_EQ(CalculateFromString("1+2)", result), -4);
    EXPECT_EQ(CalculateFromString("(1+2)*3)", result), -4);
}

// 测试错误情况：表达式以运算符结尾
TEST(Util_CalculateFromString, ErrorEndWithOperator) {
    int32_t result;
    EXPECT_EQ(CalculateFromString("1+2*", result), -2);
    EXPECT_EQ(CalculateFromString("(1+2)-", result), -2);
    EXPECT_EQ(CalculateFromString("10/2+", result), -2);
}

// 测试错误情况：连续运算符
TEST(Util_CalculateFromString, ErrorConsecutiveOperators) {
    int32_t result;

    EXPECT_EQ(CalculateFromString("1++2", result), -2);
    CalculateFromString("10--5", result);
    EXPECT_EQ(result, 15);
    EXPECT_EQ(CalculateFromString("2**3", result), -2);
    EXPECT_EQ(CalculateFromString("10//2", result), -2);
}

// 测试复杂综合表达式
TEST(Util_CalculateFromString, ComplexExpression) {
    int32_t result;
    EXPECT_EQ(CalculateFromString("(10+20)*(30-10)/2", result), 0);
    EXPECT_EQ(result, 300);

    EXPECT_EQ(CalculateFromString("100-((20+30)*2)+(40/4)", result), 0);
    EXPECT_EQ(result, 10);

    EXPECT_EQ(CalculateFromString("( (1+2)*3 + 4*5 ) / ( 6-1 )", result), 0);
    EXPECT_EQ(result, 5);

    // 实际协议中使用的复杂表达式
    EXPECT_EQ(CalculateFromString("(41 + 6 + 3 - 35 - 3) / 1", result), 0);
    EXPECT_EQ(result, 12);
}
