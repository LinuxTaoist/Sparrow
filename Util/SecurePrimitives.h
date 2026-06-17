/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SecurePrimitives.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/06/16
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/06/16 | 1.0.0.1   | Copilot        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __SECURE_PRIMITIVES_H__
#define __SECURE_PRIMITIVES_H__

#include <vector>
#include <stdint.h>

namespace SecurePrimitives {

struct ECCurve {
    int p;
    int a;
    int b;
    int startX;
};

// Public elliptic curve point structure
struct ECPoint {
    int x;
    int y;
    bool inf;  // Point at infinity flag

    ECPoint() : x(0), y(0), inf(true) {}
    ECPoint(int _x, int _y) : x(_x), y(_y), inf(false) {}
    ECPoint(int _x, int _y, bool _inf) : x(_x), y(_y), inf(_inf) {}
};

int ModPow(int base, int exp, int mod);

// Modular inverse (extended Euclidean algorithm)
// Returns positive value on success, -1 on failure
int64_t ModularInverse(int64_t a, int64_t m);

// Compute public key from private key: P = d*G
int ComputePublicKey(uint64_t d, std::vector<uint8_t>& publicKey);

// Scalar multiplication on elliptic curve: result = k*G (using standard base point)
int ScalarMultiplyPoint(uint64_t k, ECPoint& result);

// Scalar multiplication: result = k*p
int ScalarMultiplyPoint(uint64_t k, const ECPoint& p, ECPoint& result);

// Elliptic curve point addition: result = p1 + p2
int AddECPoints(const ECPoint& p1, const ECPoint& p2, ECPoint& result);

// Convert bytes to ECPoint (expects at least 8 bytes, using first 4 for x, next 4 for y)
int BytesToECPoint(const std::vector<uint8_t>& bytes, ECPoint& pt);

int EncryptByToyEC(const ECCurve& curve,
                   int privateKey,
                   const std::vector<uint8_t>& plaintext,
                   std::vector<uint8_t>& ciphertext);

int DecryptByToyEC(const ECCurve& curve,
                   int privateKey,
                   const std::vector<uint8_t>& ciphertext,
                   std::vector<uint8_t>& plaintext);

} // namespace SecurePrimitives

#endif // __SECURE_PRIMITIVES_H__
