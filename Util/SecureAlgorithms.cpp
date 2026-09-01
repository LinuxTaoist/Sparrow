/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SecureAlgorithms.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/08/03
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/08/03 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "SecureAlgorithms.h"
#include "SecurePrimitives.h"

#include <cstddef>

namespace {

// FNV-1a hash to produce 32-byte digest
std::vector<uint8_t> ComputeHash(const std::vector<uint8_t>& data)
{
    std::vector<uint8_t> hash(32, 0);
    uint64_t h = 14695981039346656037ULL;
    for (std::size_t i = 0; i < data.size(); ++i) {
        h ^= static_cast<uint64_t>(data[i]);
        h *= 1099511628211ULL;
        hash[i % 32] ^= static_cast<uint8_t>((h >> ((i % 8) * 8)) & 0xFF);
    }
    return hash;
}

// Convert 32 bytes to big-endian integer (mod n)
// For 32-byte input, interpret as unsigned big integer modulo n
uint64_t BytesToInt64(const std::vector<uint8_t>& bytes, int start, uint64_t mod)
{
    uint64_t result = 0;
    for (int i = 0; i < 8 && start + i < static_cast<int>(bytes.size()); ++i) {
        result = (result << 8) | static_cast<uint64_t>(bytes[start + i]);
    }
    return result % mod;
}

// Convert 64-bit integer to 8 bytes (big-endian)
void Int64ToBytes(uint64_t value, std::vector<uint8_t>& bytes, int start)
{
    for (int i = 7; i >= 0; --i) {
        bytes[start + (7 - i)] = static_cast<uint8_t>((value >> (i * 8)) & 0xFF);
    }
}

// Seed to deterministic private key (scalar d)
// Uses hash-based derivation
uint64_t DerivePrivateKey(const std::vector<uint8_t>& seed, uint64_t n)
{
    std::vector<uint8_t> hash = ComputeHash(seed);
    uint64_t d = BytesToInt64(hash, 0, n);
    if (d == 0) {
        d = 1;
    }
    return d;
}

// Pseudo-random k for ECDSA signing (based on message and private key)
uint64_t DeriveK(const std::vector<uint8_t>& msgHash, uint64_t privateKey, uint64_t n)
{
    uint64_t e = BytesToInt64(msgHash, 0, n);
    uint64_t k = (e + privateKey * 12345) % n;
    if (k == 0) {
        k = 1;
    }
    return k;
}

} // namespace

namespace SecureAlgorithms {

int GenerateSM2KeyPair(const std::vector<uint8_t>& seed,
                       std::vector<uint8_t>& publicKey,
                       std::vector<uint8_t>& privateKey)
{
    if (seed.empty()) {
        return -1;
    }

    // Derive private key from seed using hash-based derivation
    // Using order n = 251 (order of base point G on the elliptic curve)
    const uint64_t n = 251;
    uint64_t d = DerivePrivateKey(seed, n);

    // Store private key as 8 bytes (little-endian for 64-bit)
    privateKey.clear();
    privateKey.resize(32, 0);
    Int64ToBytes(d, privateKey, 0);

    // Compute public key using ECC point multiplication: P = d * G
    // This is computed in SecurePrimitives
    return SecurePrimitives::ComputePublicKey(d, publicKey);
}

int SignWithSM2(const std::vector<uint8_t>& data,
                const std::vector<uint8_t>& privateKey,
                std::vector<uint8_t>& signR,
                std::vector<uint8_t>& signS)
{
    if (privateKey.size() < 8 || data.empty()) {
        return -1;
    }

    // Extract private key scalar d from bytes
    uint64_t d = BytesToInt64(privateKey, 0, 251);
    if (d == 0) {
        d = 1;
    }

    // Hash the message
    std::vector<uint8_t> msgHash = ComputeHash(data);
    uint64_t e = BytesToInt64(msgHash, 0, 251);

    // Generate k (ephemeral key)
    uint64_t k = DeriveK(msgHash, d, 251);

    // Compute r = (e + x1) mod n where (x1, y1) = k * G
    SecurePrimitives::ECPoint pt;
    if (SecurePrimitives::ScalarMultiplyPoint(k, pt) != 0) {
        return -1;
    }

    uint64_t x1 = static_cast<uint64_t>(pt.x) % 251;
    uint64_t r = (e + x1) % 251;
    if (r == 0) {
        r = 1;
    }

    // Compute s = k^-1 * (e + r*d) mod n
    int64_t k_inv = SecurePrimitives::ModularInverse(static_cast<int64_t>(k), 251);
    if (k_inv < 0) {
        return -1;
    }

    uint64_t s = (static_cast<uint64_t>(k_inv) * ((e + r * d) % 251)) % 251;
    if (s == 0) {
        s = 1;
    }

    // Format signature as (r || s) in 32 bytes each
    signR.clear();
    signR.resize(32, 0);
    Int64ToBytes(r, signR, 0);

    signS.clear();
    signS.resize(32, 0);
    Int64ToBytes(s, signS, 0);

    return 0;
}

int VerifyWithSM2(const std::vector<uint8_t>& data,
                  const std::vector<uint8_t>& publicKey,
                  const std::vector<uint8_t>& signR,
                  const std::vector<uint8_t>& signS)
{
    if (publicKey.empty() || signR.size() < 8 || signS.size() < 8 || data.empty()) {
        return -1;
    }

    // Extract signature components
    uint64_t r = BytesToInt64(signR, 0, 251);
    uint64_t s = BytesToInt64(signS, 0, 251);

    if (r == 0 || s == 0) {
        return -1;
    }

    // Hash the message
    std::vector<uint8_t> msgHash = ComputeHash(data);
    uint64_t e = BytesToInt64(msgHash, 0, 251);

    // Compute w = s^-1 mod n
    int64_t s_inv = SecurePrimitives::ModularInverse(static_cast<int64_t>(s), 251);
    if (s_inv < 0) {
        return -1;
    }
    uint64_t w = static_cast<uint64_t>(s_inv);

    // Compute u1 = e*w mod n
    uint64_t u1 = (e * w) % 251;

    // Compute u2 = r*w mod n
    uint64_t u2 = (r * w) % 251;

    // Compute (x, y) = u1*G + u2*P
    // First compute u1*G
    SecurePrimitives::ECPoint pt1;
    if (SecurePrimitives::ScalarMultiplyPoint(u1, pt1) != 0) {
        return -1;
    }

    // Compute u2*P (using publicKey which is stored as point coordinates)
    SecurePrimitives::ECPoint pubPt;
    if (SecurePrimitives::BytesToECPoint(publicKey, pubPt) != 0) {
        return -1;
    }

    SecurePrimitives::ECPoint pt2;
    if (SecurePrimitives::ScalarMultiplyPoint(u2, pubPt, pt2) != 0) {
        return -1;
    }

    // Add the two points: u1*G + u2*P
    SecurePrimitives::ECPoint result;
    if (SecurePrimitives::AddECPoints(pt1, pt2, result) != 0) {
        return -1;
    }

    // Verify r == x mod n
    uint64_t x = static_cast<uint64_t>(result.x) % 251;
    if (x == r) {
        return 0;
    }

    return -1;
}

int EncryptWithSM2(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext)
{
    static const SecurePrimitives::ECCurve kSm2LikeCurve = {251, 1, 1, 2};
    static const int kSm2PrivateKey = 97;
    return SecurePrimitives::EncryptByToyEC(kSm2LikeCurve, kSm2PrivateKey, plaintext, ciphertext);
}

int DecryptWithSM2(const std::vector<uint8_t>& ciphertext, std::vector<uint8_t>& plaintext)
{
    static const SecurePrimitives::ECCurve kSm2LikeCurve = {251, 1, 1, 2};
    static const int kSm2PrivateKey = 97;
    return SecurePrimitives::DecryptByToyEC(kSm2LikeCurve, kSm2PrivateKey, ciphertext, plaintext);
}

int EncryptWithRSA(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext)
{
    // Toy RSA demo parameters: n=3233, e=17 (matching d=2753)
    static const int kRsaN = 3233;
    static const int kRsaE = 17;

    ciphertext.resize(plaintext.size() * 2);
    for (std::size_t i = 0; i < plaintext.size(); ++i) {
        int c = SecurePrimitives::ModPow(static_cast<int>(plaintext[i]), kRsaE, kRsaN);
        ciphertext[2 * i] = static_cast<uint8_t>((c >> 8) & 0xFF);
        ciphertext[2 * i + 1] = static_cast<uint8_t>(c & 0xFF);
    }

    return 0;
}

int DecryptWithRSA(const std::vector<uint8_t>& ciphertext, std::vector<uint8_t>& plaintext)
{
    static const int kRsaN = 3233;
    static const int kRsaD = 2753;

    if ((ciphertext.size() % 2) != 0) {
        return -1;
    }

    plaintext.resize(ciphertext.size() / 2);
    for (std::size_t i = 0; i < plaintext.size(); ++i) {
        int c = (static_cast<int>(ciphertext[2 * i]) << 8) |
                static_cast<int>(ciphertext[2 * i + 1]);
        int m = SecurePrimitives::ModPow(c, kRsaD, kRsaN);
        if (m < 0 || m > 255) {
            return -1;
        }
        plaintext[i] = static_cast<uint8_t>(m);
    }

    return 0;
}

int EncryptWithECC(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext)
{
    static const SecurePrimitives::ECCurve kEccCurve = {251, 2, 3, 5};
    static const int kEccPrivateKey = 131;
    return SecurePrimitives::EncryptByToyEC(kEccCurve, kEccPrivateKey, plaintext, ciphertext);
}

int DecryptWithECC(const std::vector<uint8_t>& ciphertext, std::vector<uint8_t>& plaintext)
{
    static const SecurePrimitives::ECCurve kEccCurve = {251, 2, 3, 5};
    static const int kEccPrivateKey = 131;
    return SecurePrimitives::DecryptByToyEC(kEccCurve, kEccPrivateKey, ciphertext, plaintext);
}

} // namespace SecureAlgorithms
