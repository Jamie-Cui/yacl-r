// Copyright 2024 Ant Group Co., Ltd.
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

#include <span>

#include "yacl/utils/byte_container_view.h"

/* security parameter declaration */
// this header dispatches between multiple AEAD schemes at runtime, concrete
// scheme headers declare their own security parameters

namespace yacl {

// ===================================================
// AEAD: Authenticated Encryption with Associated Data
// ===================================================
//
// AEAD provides confidentiality by encrypting the data with a symmetric
// encryption algorithm, and provides authenticity by using a MAC tag over the
// encrypted data.
//
// Related standards:
// + https://datatracker.ietf.org/doc/html/rfc5116
//
// NOTE Strictly, mac-then-encrypt algorithm is not aead, we add those
// algorithms only for backword compatiability.

enum class AeadType : uint8_t {
  UNKNOWN = 0,
  AES128_GCM = 1,  // Galois-Counter Mode
  AES256_GCM = 2,  // Galois-Counter Mode
#ifdef YACL_WITH_TONGSUO
  SM4_GCM = 3,  // NOTE only Yacl built with gm mode supports this feature
#endif
  SM4_MTE_HMAC_SM3 = 4,  // Mac-Then-Encrypt with SM4 counter mode
};

// Pre-defined default Aead algorithm for AeadCtx only
constexpr AeadType kDefaultAeadAlgorithm = AeadType::AES128_GCM;

// AEAD Context Class
class AeadCtx {
 public:
  // Constructors
  AeadCtx();
  explicit AeadCtx(AeadType algorithm) { SetAlgorithm(algorithm); }

  // Get a default AeadCtx with the AeadAlgorithm set. This function could be
  // seen as a helper function if you do not know which algorithm to choose.
  // Yacl recommend the use of GetDefault().
  static AeadCtx& GetDefault() {
    static AeadCtx ctx(kDefaultAeadAlgorithm);
    return ctx;
  }

  // Get the key size of the AEAD algorithm that is stored insize AeadCtx
  size_t GetKeySize() {
    YACL_ENFORCE(algorithm_ != AeadType::UNKNOWN);
    return GetKeySize(algorithm_);
  }

  // Staticlly get the key size of an AEAD algorithm
  static size_t GetKeySize(AeadType algorithm);

  // Get the mac size of the AEAD algorithm that is stored insize AeadCtx
  //
  // NOTE in case of mac-then-encrypt algorithm, this function fetches the
  // encrypted mac size
  size_t GetMacSize() {
    YACL_ENFORCE(algorithm_ != AeadType::UNKNOWN);
    return GetMacSize(algorithm_);
  }

  // Staticlly get the mac size of an AEAD algorithm
  //
  // NOTE in case of mac-then-encrypt algorithm, this function fetches the
  // encrypted mac size
  static size_t GetMacSize(AeadType algorithm);

  AeadType GetAlgorithm() { return algorithm_; }
  void SetAlgorithm(AeadType algorithm) { algorithm_ = algorithm; }

  // Encrypts plaintext into ciphertext and mac. The input arguments
  // are the AEAD algorithm, the plaintext,  and the optional
  // additional-authenticated-data (aad).
  //
  // NOTE Since Mac-Then-Encrypt results in one ciphertext, the argument "mac"
  // is ignored for Mte algorithms
  void Encrypt(ByteContainerView plaintext, ByteContainerView key,
               ByteContainerView iv, std::span<uint8_t> ciphertext,
               std::span<uint8_t> mac, ByteContainerView aad = "") const;

  // Decrypts ciphertext and mac into ciphertext. The input arguments are the
  // AEAD algorithm, the ciphertext, the mac,and the optional
  // additional-authenticated-data (aad).
  //
  // NOTE Since Mac-Then-Encrypt results in one ciphertext, the argument "mac"
  // is ignored for Mte algorithms
  void Decrypt(ByteContainerView ciphertext, ByteContainerView mac,
               ByteContainerView key, ByteContainerView iv,
               std::span<uint8_t> plaintext, ByteContainerView aad = "") const;

  // Staticlly encrypts plaintext into ciphertext and mac. The input arguments
  // are the AEAD algorithm, the plaintext, the symmetric encryption key, the
  // initialization vector (iv), and the optional additional-authenticated-data
  // (aad).
  //
  // NOTE Since Mac-Then-Encrypt results in one ciphertext, the argument "mac"
  // is ignored for Mte algorithms
  static void Encrypt(AeadType algorithm, ByteContainerView plaintext,
                      ByteContainerView key, ByteContainerView iv,
                      std::span<uint8_t> ciphertext, std::span<uint8_t> mac,
                      ByteContainerView aad = "");

  // Staticlly decrypts ciphertext and mac into ciphertext. The input
  // arguments are the AEAD algorithm, the ciphertext, the mac, the symmetric
  // encryption key, the initialization vector (iv), and the optional
  // additional-authenticated-data (aad).
  //
  // NOTE Since Mac-Then-Encrypt results in one ciphertext, the argument "mac"
  // is ignored for Mte algorithms
  static void Decrypt(AeadType algorithm, ByteContainerView ciphertext,
                      ByteContainerView mac, ByteContainerView key,
                      ByteContainerView iv, std::span<uint8_t> plaintext,
                      ByteContainerView aad = "");

 private:
  AeadType algorithm_ = AeadType::UNKNOWN;  // GCM crypto schema
};

}  // namespace yacl
