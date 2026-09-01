/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SecureAlgorithms.h
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
#ifndef __SECURE_ALGORITHMS_H__
#define __SECURE_ALGORITHMS_H__

#include <vector>
#include <stdint.h>

namespace SecureAlgorithms {

/**
 * @brief Generate SM2-like key pair from seed
 *
 * @param seed Input seed bytes, stable seed gives stable keypair
 * @param publicKey Output public key bytes
 * @param privateKey Output private key bytes
 * @return 0 if success, -1 if failed
 */
int GenerateSM2KeyPair(const std::vector<uint8_t>& seed,
					   std::vector<uint8_t>& publicKey,
					   std::vector<uint8_t>& privateKey);

/**
 * @brief Sign data with SM2-like private key
 *
 * @param data Input bytes to sign
 * @param privateKey Input private key bytes
 * @param signR Output signature R bytes
 * @param signS Output signature S bytes
 * @return 0 if success, -1 if failed
 */
int SignWithSM2(const std::vector<uint8_t>& data,
				const std::vector<uint8_t>& privateKey,
				std::vector<uint8_t>& signR,
				std::vector<uint8_t>& signS);

/**
 * @brief Verify signature with SM2-like public key
 *
 * @param data Input bytes
 * @param publicKey Input public key bytes
 * @param signR Input signature R
 * @param signS Input signature S
 * @return 0 if verified, -1 if failed
 */
int VerifyWithSM2(const std::vector<uint8_t>& data,
				  const std::vector<uint8_t>& publicKey,
				  const std::vector<uint8_t>& signR,
				  const std::vector<uint8_t>& signS);

/**
 * @brief Encrypt data using SM2 algorithm
 *
 * @param plaintext Input data to be encrypted
 * @param ciphertext Output encrypted data
 * @return 0 if success, -1 if failed
 */
int EncryptWithSM2(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext);

/**
 * @brief Decrypt data using SM2 algorithm
 *
 * @param ciphertext Input encrypted data
 * @param plaintext Output decrypted data
 * @return 0 if success, -1 if failed
 */
int DecryptWithSM2(const std::vector<uint8_t>& ciphertext, std::vector<uint8_t>& plaintext);

/**
 * @brief Encrypt data using RSA algorithm
 *
 * @param plaintext Input data to be encrypted
 * @param ciphertext Output encrypted data
 * @return 0 if success, -1 if failed
 */
int EncryptWithRSA(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext);

/**
 * @brief Decrypt data using RSA algorithm
 *
 * @param ciphertext Input encrypted data
 * @param plaintext Output decrypted data
 * @return 0 if success, -1 if failed
 */
int DecryptWithRSA(const std::vector<uint8_t>& ciphertext, std::vector<uint8_t>& plaintext);

/**
 * @brief Encrypt data using ECC algorithm
 *
 * @param plaintext Input data to be encrypted
 * @param ciphertext Output encrypted data
 * @return 0 if success, -1 if failed
 */
int EncryptWithECC(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext);

/**
 * @brief Decrypt data using ECC algorithm
 *
 * @param ciphertext Input encrypted data
 * @param plaintext Output decrypted data
 * @return 0 if success, -1 if failed
 */
int DecryptWithECC(const std::vector<uint8_t>& ciphertext, std::vector<uint8_t>& plaintext);

} // namespace SecureAlgorithms

#endif //__SECURE_ALGORITHMS_H__
