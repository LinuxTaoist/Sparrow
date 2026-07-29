/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SecurePrimitives.cpp
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
#include "SecurePrimitives.h"

#include <cstddef>
#include <cstdint>

namespace {

struct InternalECPoint {
    int x;
    int y;
    bool inf;
};

int Mod(int v, int p)
{
    int r = v % p;
    return (r < 0) ? (r + p) : r;
}

int ModInv(int value, int p)
{
    int t = 0;
    int newT = 1;
    int r = p;
    int newR = Mod(value, p);

    while (newR != 0) {
        int q = r / newR;
        int tmpT = t - q * newT;
        t = newT;
        newT = tmpT;

        int tmpR = r - q * newR;
        r = newR;
        newR = tmpR;
    }

    if (r != 1) {
        return -1;
    }

    return Mod(t, p);
}

bool IsOnCurve(const SecurePrimitives::ECCurve& curve, const InternalECPoint& p)
{
    if (p.inf) {
        return true;
    }

    int left = Mod(p.y * p.y, curve.p);
    int right = Mod(p.x * p.x * p.x + curve.a * p.x + curve.b, curve.p);
    return left == right;
}

InternalECPoint PointAdd(const SecurePrimitives::ECCurve& curve, const InternalECPoint& p1, const InternalECPoint& p2)
{
    if (p1.inf) {
        return p2;
    }
    if (p2.inf) {
        return p1;
    }

    if (p1.x == p2.x && Mod(p1.y + p2.y, curve.p) == 0) {
        return InternalECPoint{0, 0, true};
    }

    int lambda = 0;
    if (p1.x == p2.x && p1.y == p2.y) {
        int denom = ModInv(2 * p1.y, curve.p);
        if (denom < 0) {
            return InternalECPoint{0, 0, true};
        }
        lambda = Mod((3 * p1.x * p1.x + curve.a) * denom, curve.p);
    } else {
        int denom = ModInv(p2.x - p1.x, curve.p);
        if (denom < 0) {
            return InternalECPoint{0, 0, true};
        }
        lambda = Mod((p2.y - p1.y) * denom, curve.p);
    }

    int xr = Mod(lambda * lambda - p1.x - p2.x, curve.p);
    int yr = Mod(lambda * (p1.x - xr) - p1.y, curve.p);
    return InternalECPoint{xr, yr, false};
}

InternalECPoint PointMul(const SecurePrimitives::ECCurve& curve, int k, const InternalECPoint& p)
{
    InternalECPoint result{0, 0, true};
    InternalECPoint addend = p;

    while (k > 0) {
        if ((k & 1) != 0) {
            result = PointAdd(curve, result, addend);
        }
        addend = PointAdd(curve, addend, addend);
        k >>= 1;
    }

    return result;
}

InternalECPoint FindBasePoint(const SecurePrimitives::ECCurve& curve)
{
    for (int x = curve.startX; x < curve.p; ++x) {
        int rhs = Mod(x * x * x + curve.a * x + curve.b, curve.p);
        for (int y = 0; y < curve.p; ++y) {
            if (Mod(y * y, curve.p) == rhs) {
                return InternalECPoint{x, y, false};
            }
        }
    }
    return InternalECPoint{0, 0, true};
}

// Cache the base point G
InternalECPoint& GetBasePointG()
{
    static const SecurePrimitives::ECCurve curve = {251, 1, 1, 2};
    static InternalECPoint g = FindBasePoint(curve);
    return g;
}

} // namespace

namespace SecurePrimitives {

int ModPow(int base, int exp, int mod)
{
    int64_t result = 1;
    int64_t b = Mod(base, mod);
    while (exp > 0) {
        if ((exp & 1) != 0) {
            result = (result * b) % mod;
        }
        b = (b * b) % mod;
        exp >>= 1;
    }
    return static_cast<int>(result);
}

// Modular inverse using extended Euclidean algorithm
int64_t ModularInverse(int64_t a, int64_t m)
{
    int64_t m0 = m;
    int64_t x0 = 0, x1 = 1;

    if (m == 1) return 0;

    a = a % m;
    if (a < 0) a += m;

    while (a > 1) {
        int64_t q = a / m;
        int64_t t = m;

        m = a % m;
        a = t;
        t = x0;

        x0 = x1 - q * x0;
        x1 = t;
    }

    if (x1 < 0) x1 += m0;
    return x1;
}

// Compute public key P = d * G
int ComputePublicKey(uint64_t d, std::vector<uint8_t>& publicKey)
{
    static const ECCurve curve = {251, 1, 1, 2};

    if (d == 0) {
        return -1;
    }

    InternalECPoint& g = GetBasePointG();
    if (g.inf) {
        return -1;
    }

    // Compute P = d * G
    InternalECPoint pub = PointMul(curve, static_cast<int>(d % 251), g);
    if (pub.inf) {
        return -1;
    }

    // Encode public key as 32 bytes: 16 bytes for x, 16 bytes for y
    publicKey.clear();
    publicKey.resize(32, 0);

    // Store x coordinate (lower 4 bytes)
    for (int i = 0; i < 4; ++i) {
        publicKey[i] = static_cast<uint8_t>((pub.x >> (i * 8)) & 0xFF);
    }

    // Store y coordinate (upper 4 bytes, offset 16)
    for (int i = 0; i < 4; ++i) {
        publicKey[16 + i] = static_cast<uint8_t>((pub.y >> (i * 8)) & 0xFF);
    }

    return 0;
}

// Scalar multiplication: result = k * G
int ScalarMultiplyPoint(uint64_t k, ECPoint& result)
{
    static const ECCurve curve = {251, 1, 1, 2};

    if (k == 0) {
        return -1;
    }

    InternalECPoint& g = GetBasePointG();
    if (g.inf) {
        return -1;
    }

    InternalECPoint pt = PointMul(curve, static_cast<int>(k % 251), g);
    if (pt.inf) {
        return -1;
    }

    result.x = pt.x;
    result.y = pt.y;
    result.inf = false;
    return 0;
}

// Scalar multiplication: result = k * p
int ScalarMultiplyPoint(uint64_t k, const ECPoint& p, ECPoint& result)
{
    static const ECCurve curve = {251, 1, 1, 2};

    if (k == 0 || p.inf) {
        return -1;
    }

    InternalECPoint internalP{p.x, p.y, false};
    InternalECPoint pt = PointMul(curve, static_cast<int>(k % 251), internalP);
    if (pt.inf) {
        return -1;
    }

    result.x = pt.x;
    result.y = pt.y;
    result.inf = false;
    return 0;
}

// Point addition: result = p1 + p2
int AddECPoints(const ECPoint& p1, const ECPoint& p2, ECPoint& result)
{
    static const ECCurve curve = {251, 1, 1, 2};

    InternalECPoint internal1{p1.x, p1.y, p1.inf};
    InternalECPoint internal2{p2.x, p2.y, p2.inf};

    InternalECPoint sum = PointAdd(curve, internal1, internal2);
    if (sum.inf) {
        return -1;
    }

    result.x = sum.x;
    result.y = sum.y;
    result.inf = false;
    return 0;
}

// Convert bytes to ECPoint
int BytesToECPoint(const std::vector<uint8_t>& bytes, ECPoint& pt)
{
    if (bytes.size() < 8) {
        return -1;
    }

    int x = 0;
    int y = 0;

    for (int i = 0; i < 4; ++i) {
        x = (x << 8) | static_cast<int>(bytes[i]);
    }

    for (int i = 0; i < 4; ++i) {
        y = (y << 8) | static_cast<int>(bytes[4 + i]);
    }

    pt.x = x % 251;
    pt.y = y % 251;
    pt.inf = false;
    return 0;
}

int EncryptByToyEC(const ECCurve& curve,
                   int privateKey,
                   const std::vector<uint8_t>& plaintext,
                   std::vector<uint8_t>& ciphertext)
{
    if (plaintext.empty()) {
        ciphertext.clear();
        return 0;
    }

    InternalECPoint g = FindBasePoint(curve);
    if (g.inf) {
        return -1;
    }

    InternalECPoint pub = PointMul(curve, privateKey, g);
    if (pub.inf || !IsOnCurve(curve, pub)) {
        return -1;
    }

    int seed = static_cast<int>(plaintext.size());
    for (size_t i = 0; i < plaintext.size(); ++i) {
        seed = Mod(seed + plaintext[i] + static_cast<int>(i), curve.p - 1);
    }

    int k = seed + 1;
    InternalECPoint c1{0, 0, true};
    InternalECPoint shared{0, 0, true};
    for (int attempt = 0; attempt < curve.p; ++attempt) {
        c1 = PointMul(curve, k, g);
        shared = PointMul(curve, k, pub);
        if (!c1.inf && !shared.inf) {
            break;
        }
        k = (k % (curve.p - 1)) + 1;
    }

    if (c1.inf || shared.inf) {
        return -1;
    }

    ciphertext.resize(2 + plaintext.size());
    ciphertext[0] = static_cast<uint8_t>(c1.x);
    ciphertext[1] = static_cast<uint8_t>(c1.y);
    for (size_t i = 0; i < plaintext.size(); ++i) {
        uint8_t ks = static_cast<uint8_t>(Mod(shared.x + 3 * shared.y + static_cast<int>(i) * 7, 256));
        ciphertext[i + 2] = static_cast<uint8_t>(plaintext[i] ^ ks);
    }

    return 0;
}

int DecryptByToyEC(const ECCurve& curve,
                   int privateKey,
                   const std::vector<uint8_t>& ciphertext,
                   std::vector<uint8_t>& plaintext)
{
    if (ciphertext.empty()) {
        plaintext.clear();
        return 0;
    }

    if (ciphertext.size() < 2) {
        return -1;
    }

    InternalECPoint c1{static_cast<int>(ciphertext[0]), static_cast<int>(ciphertext[1]), false};
    if (!IsOnCurve(curve, c1)) {
        return -1;
    }

    InternalECPoint shared = PointMul(curve, privateKey, c1);
    if (shared.inf) {
        return -1;
    }

    plaintext.resize(ciphertext.size() - 2);
    for (size_t i = 0; i < plaintext.size(); ++i) {
        uint8_t ks = static_cast<uint8_t>(Mod(shared.x + 3 * shared.y + static_cast<int>(i) * 7, 256));
        plaintext[i] = static_cast<uint8_t>(ciphertext[i + 2] ^ ks);
    }

    return 0;
}

} // namespace SecurePrimitives
