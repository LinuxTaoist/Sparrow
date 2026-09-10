/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSecureAlgorithms.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : SecureAlgorithms 密码算法（SM2/RSA/ECC）内部测试
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
#include "SecureAlgorithms.h"
#include "gtest/gtest.h"

using namespace SecureAlgorithms;

// 测试 SM2 密钥对生成的边界与正常情况
TEST(Util_SecureAlgorithms, GenerateSM2KeyPairBoundaryAndNormal)
{
    std::vector<uint8_t> pub;
    std::vector<uint8_t> priv;

    EXPECT_EQ(GenerateSM2KeyPair({}, pub, priv), -1);   // 空 seed

    EXPECT_EQ(GenerateSM2KeyPair({0x01, 0x02, 0x03}, pub, priv), 0);
    EXPECT_EQ(pub.size(), 32u);
    EXPECT_EQ(priv.size(), 32u);
}

// 测试 SM2 签名的边界与正常情况
TEST(Util_SecureAlgorithms, SignSM2BoundaryAndNormal)
{
    std::vector<uint8_t> signR;
    std::vector<uint8_t> signS;

    // privateKey 不足 8 字节
    EXPECT_EQ(SignWithSM2({0x01}, {0x01, 0x02}, signR, signS), -1);
    // data 为空
    EXPECT_EQ(SignWithSM2({}, {0, 0, 0, 0, 0, 0, 0, 1}, signR, signS), -1);

    // 正常签名
    std::vector<uint8_t> priv = {0, 0, 0, 0, 0, 0, 0, 5};
    EXPECT_EQ(SignWithSM2({0xde, 0xad, 0xbe, 0xef}, priv, signR, signS), 0);
    EXPECT_EQ(signR.size(), 32u);
    EXPECT_EQ(signS.size(), 32u);
}

// 测试 SM2 验签的边界情况
TEST(Util_SecureAlgorithms, VerifySM2Boundary)
{
    std::vector<uint8_t> pub = {0, 0, 0, 1, 0, 0, 0, 2};
    std::vector<uint8_t> signR = {0, 0, 0, 0, 0, 0, 0, 1};
    std::vector<uint8_t> signS = {0, 0, 0, 0, 0, 0, 0, 2};

    EXPECT_EQ(VerifyWithSM2({0x01}, {}, signR, signS), -1);   // 空 pub
    EXPECT_EQ(VerifyWithSM2({}, pub, signR, signS), -1);      // 空 data
    EXPECT_EQ(VerifyWithSM2({0x01}, pub, {0x01}, signS), -1); // 短 signR
    EXPECT_EQ(VerifyWithSM2({0x01}, pub, signR, {0x01}), -1); // 短 signS
}

// 测试 SM2 加解密往返一致性
TEST(Util_SecureAlgorithms, SM2EncryptDecryptRoundtrip)
{
    const std::vector<uint8_t> plaintext = {0xaa, 0xbb, 0xcc, 0xdd};
    std::vector<uint8_t> cipher;
    std::vector<uint8_t> decrypted;

    EXPECT_EQ(EncryptWithSM2(plaintext, cipher), 0);
    EXPECT_EQ(DecryptWithSM2(cipher, decrypted), 0);
    EXPECT_EQ(decrypted, plaintext);
}

// 测试 RSA 加解密往返一致性
TEST(Util_SecureAlgorithms, RSAEncryptDecryptRoundtrip)
{
    const std::vector<uint8_t> plaintext = {0x48, 0x65, 0x6c, 0x6c, 0x6f};  // "Hello"
    std::vector<uint8_t> cipher;
    std::vector<uint8_t> decrypted;

    EXPECT_EQ(EncryptWithRSA(plaintext, cipher), 0);
    EXPECT_EQ(cipher.size(), plaintext.size() * 2);
    EXPECT_EQ(DecryptWithRSA(cipher, decrypted), 0);
    EXPECT_EQ(decrypted, plaintext);

    // 奇数长度密文 -> 失败
    EXPECT_EQ(DecryptWithRSA({0x01}, decrypted), -1);
}

// 测试 ECC 加解密往返一致性
TEST(Util_SecureAlgorithms, ECCEncryptDecryptRoundtrip)
{
    const std::vector<uint8_t> plaintext = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    std::vector<uint8_t> cipher;
    std::vector<uint8_t> decrypted;

    EXPECT_EQ(EncryptWithECC(plaintext, cipher), 0);
    EXPECT_EQ(DecryptWithECC(cipher, decrypted), 0);
    EXPECT_EQ(decrypted, plaintext);
}
