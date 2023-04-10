// Copyright 2023 Chengfang Financial Technology Co., Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef YACL_CRYPTO_PRIMITIVES_TPRE_TPRE_H_
#define YACL_CRYPTO_PRIMITIVES_TPRE_TPRE_H_

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "yacl/crypto/primitives/tpre/capsule.h"
#include "yacl/crypto/primitives/tpre/keys.h"

namespace yacl::crypto {
/**
 * The TPRE class encapsulates the methods of threshold proxy
 * re-encryption, including encryption, decryption, re-encryption and
 * multi-party decryption
 *
 * Here, let's assume that there are N+2 participants, including Alice, the data
 * holder, Bob, the data requester, and multiple proxy re-encryption nodes
 * P_0,..., P_{N-1}.
 *
 * Assume m is a data to shared. Alice and Bob will perform the following
 * calculation steps:
 *
 * 1. Alice generates the ciphertext c of plaintext m via
 *    Encrypt(pk_A, m,...)；Here，Alice can decryt c to m by Decrypt(sk_A,
 *    c,...)
 *
 * 2. Alice entrusts P_1,..., P_t re-encrypts c to obtain capsule set =
 *    {cfrag_1,...,cfrag_t}. Only t Proxys participate here, reflecting the idea
 *    of threshold. In the re-encryption phase, you need to use the
 *    re-encryption key The re-encryption key is generated by calling the
 *    re-encryption key generation algorithm Keys::GenerateReKey by entering
 *    Alice's private key and Bob's public key.
 *
 * 3. Inputs capsule set and secret key of Bob, decrypts the capsule set to
 *    plaintext.
 *
 * For the description of capsule, you can see in the Capsule class.
 */
class TPRE {
 public:
  TPRE(){};
  ~TPRE(){};

  //   /// @brief Rename the re-encryption ciphertext set,  which contains
  //   capsule
  //   /// fragments set(CFrags set) and enc_data
  //   typedef std::pair<std::vector<std::unique_ptr<Capsule::CFrag>>,
  //                     std::vector<uint8_t>>
  //       std::pair<std::vector<std::unique_ptr<Capsule::CFrag>>,
  //                     std::vector<uint8_t>>;

  /// @brief TPRE's encryption algorithm
  /// @param ecc_group, Algebraic operations are on elliptic curves
  /// @param pk_A, the public key of Alice, Alice refers to the authorizer
  /// @param iv, random value
  /// @param plaintext
  /// @return capsule and encrypted data
  std::pair<Capsule::CapsuleStruct, std::vector<uint8_t>> Encrypt(
      const std::unique_ptr<EcGroup>& ecc_group, const Keys::PublicKey& pk_A,
      ByteContainerView iv, ByteContainerView plaintext) const;

  /// @brief TPRE's decryption algorithm
  /// @param ecc_group, Algebraic operations are on elliptic curves
  /// @param capsule_struct, reference Capsule class.
  /// @param iv, random value
  /// @param enc_data, decrypted data
  /// @param sk_A, the secret key of Alice, Alice refers to the authorizer
  /// @return plaintext
  std::string Decrypt(const std::unique_ptr<EcGroup>& ecc_group,
                      const Capsule::CapsuleStruct& capsule_struct,
                      ByteContainerView iv,
                      const std::vector<uint8_t>& enc_data,
                      const Keys::PrivateKey& sk_A) const;

  /// @brief TPRE's threshold proxy re-encryption algorithm
  /// @param ecc_group, Algebraic operations are on elliptic curves
  /// @param kfrag, the re-encryption key of proxy
  /// @param ciphertext, capsule and encrypted data
  /// @return ciphertext fragments
  std::pair<Capsule::CFrag, std::vector<uint8_t>> ReEncrypt(
      const std::unique_ptr<EcGroup>& ecc_group, const Keys::KFrag& kfrag,
      const std::pair<Capsule::CapsuleStruct, std::vector<uint8_t>>& ciphertext)
      const;

  /// @brief TPRE's threshold decryption algorithm
  /// @param ecc_group, Algebraic operations are on elliptic curves
  /// @param sk_B, the secret key of Bob
  /// @param pk_A, the public key of Alice
  /// @param pk_B, the public key of Bob
  /// @param iv, random value
  /// @param C_prime_set, ciphertext fragments set
  /// @return plaintext
  std::string DecryptFrags(
      const std::unique_ptr<EcGroup>& ecc_group, const Keys::PrivateKey& sk_B,
      const Keys::PublicKey& pk_A, const Keys::PublicKey& pk_B,
      ByteContainerView iv,
      const std::pair<std::vector<Capsule::CFrag>, std::vector<uint8_t>>&
          C_prime_set) const;
};
}  // namespace yacl::crypto

#endif  // YACL_CRYPTO_PRIMITIVES_TPRE_TPRE_H_
