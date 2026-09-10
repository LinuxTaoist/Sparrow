/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSecurePrimitives.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : SecurePrimitives 椭圆曲线密码原语内部测试
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
#include <cstdint>
#include <vector>
#include "SecurePrimitives.h"
#include "gtest/gtest.h"

using namespace SecurePrimitives;

// 默认曲线 {p=251, a=1, b=1, startX=2}
static const ECCurve kDefCurve = {251, 1, 1, 2};

// 测试模幂运算的基础与边界情况
TEST(Util_SecurePrimitives, ModPowBasicAndBoundary)
{
    EXPECT_EQ(ModPow(2, 10, 1000), 24);     // 1024 % 1000
    EXPECT_EQ(ModPow(3, 4, 7), 4);          // 81 % 7
    EXPECT_EQ(ModPow(5, 0, 7), 1);          // exp=0
    EXPECT_EQ(ModPow(-2, 3, 5), 2);         // (-8) % 5 -> 2
    EXPECT_EQ(ModPow(1, 1000000, 2), 1);    // 1^anything
}

// 测试模逆运算的基础与边界情况
TEST(Util_SecurePrimitives, ModularInverseBasicAndBoundary)
{
    EXPECT_EQ(ModularInverse(3, 7), 5);     // 3*5=15 ≡ 1 mod 7
    EXPECT_EQ(ModularInverse(4, 251), 63);  // 4*63=252 ≡ 1 mod 251
    EXPECT_EQ(ModularInverse(1, 1), 0);     // m==1 -> 0
    EXPECT_EQ(ModularInverse(-3, 7), 2);    // -3 ≡ 4, 4*2=8 ≡ 1
    EXPECT_EQ(ModularInverse(10, 251), 226); // 10*226=2260 ≡ 1 mod 251
}

// 测试由私钥计算公钥的边界与正常情况
TEST(Util_SecurePrimitives, ComputePublicKeyBoundaryAndNormal)
{
    std::vector<uint8_t> pub;
    EXPECT_EQ(ComputePublicKey(0, pub), -1);   // d == 0

    EXPECT_EQ(ComputePublicKey(5, pub), 0);
    EXPECT_EQ(pub.size(), 32u);
    // x 存于 [0..3], y 存于 [16..19]
    EXPECT_FALSE(pub[0] == 0 && pub[1] == 0 && pub[2] == 0 && pub[3] == 0);
}

// 测试基于基点的标量乘法
TEST(Util_SecurePrimitives, ScalarMultiplyByBasePoint)
{
    ECPoint g;
    ASSERT_EQ(ScalarMultiplyPoint(1, g), 0);
    EXPECT_FALSE(g.inf);

    ECPoint r;
    EXPECT_EQ(ScalarMultiplyPoint(0, r), -1);   // k == 0

    EXPECT_EQ(ScalarMultiplyPoint(2, r), 0);    // 2G
    EXPECT_FALSE(r.inf);
}

// 测试指定点的标量乘法
TEST(Util_SecurePrimitives, ScalarMultiplyWithExplicitPoint)
{
    ECPoint g;
    ASSERT_EQ(ScalarMultiplyPoint(1, g), 0);

    ECPoint r;
    EXPECT_EQ(ScalarMultiplyPoint(0, g, r), -1);   // k == 0

    ECPoint inf;                                   // 默认无穷远点
    EXPECT_EQ(ScalarMultiplyPoint(3, inf, r), -1); // p.inf

    EXPECT_EQ(ScalarMultiplyPoint(2, g, r), 0);    // 2*G
    ECPoint g2;
    ASSERT_EQ(ScalarMultiplyPoint(2, g2), 0);
    EXPECT_EQ(r.x, g2.x);
    EXPECT_EQ(r.y, g2.y);
}

// 测试椭圆曲线点加法的正确性
TEST(Util_SecurePrimitives, AddECPointsCorrectness)
{
    ECPoint g;
    ASSERT_EQ(ScalarMultiplyPoint(1, g), 0);

    // G + infinity == G
    ECPoint inf;
    ECPoint sum;
    EXPECT_EQ(AddECPoints(g, inf, sum), 0);
    EXPECT_EQ(sum.x, g.x);
    EXPECT_EQ(sum.y, g.y);

    // G + G == 2G
    ECPoint g2;
    ASSERT_EQ(ScalarMultiplyPoint(2, g2), 0);
    EXPECT_EQ(AddECPoints(g, g, sum), 0);
    EXPECT_EQ(sum.x, g2.x);
    EXPECT_EQ(sum.y, g2.y);

    // 无穷远点 + 无穷远点 -> 返回 -1
    EXPECT_EQ(AddECPoints(inf, inf, sum), -1);
}

// 测试字节流转椭圆曲线点的边界与正常情况
TEST(Util_SecurePrimitives, BytesToECPointBoundaryAndNormal)
{
    ECPoint pt;
    std::vector<uint8_t> shortBytes = {0x01, 0x02, 0x03};
    EXPECT_EQ(BytesToECPoint(shortBytes, pt), -1);   // < 8 字节

    std::vector<uint8_t> bytes = {0, 0, 0, 5, 0, 0, 0, 7};
    EXPECT_EQ(BytesToECPoint(bytes, pt), 0);
    EXPECT_EQ(pt.x, 5);
    EXPECT_EQ(pt.y, 7);
    EXPECT_FALSE(pt.inf);
}

// 测试玩具椭圆曲线加解密往返一致性
TEST(Util_SecurePrimitives, ToyECEncryptDecryptRoundtrip)
{
    const std::vector<uint8_t> plaintext = {0x01, 0x02, 0x03, 0x04, 0x05};
    std::vector<uint8_t> cipher;
    std::vector<uint8_t> decrypted;

    EXPECT_EQ(EncryptByToyEC(kDefCurve, 97, plaintext, cipher), 0);
    EXPECT_EQ(cipher.size(), 2 + plaintext.size());
    EXPECT_EQ(DecryptByToyEC(kDefCurve, 97, cipher, decrypted), 0);
    EXPECT_EQ(decrypted, plaintext);
}

// 测试玩具椭圆曲线空输入与边界情况
TEST(Util_SecurePrimitives, ToyECEmptyAndBoundary)
{
    std::vector<uint8_t> out;

    EXPECT_EQ(EncryptByToyEC(kDefCurve, 97, {}, out), 0);   // 空明文
    EXPECT_TRUE(out.empty());

    EXPECT_EQ(DecryptByToyEC(kDefCurve, 97, {}, out), 0);   // 空密文
    EXPECT_TRUE(out.empty());

    EXPECT_EQ(DecryptByToyEC(kDefCurve, 97, {0x01}, out), -1); // < 2 字节
}
