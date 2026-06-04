// Copyright 2019 Ant Group Co., Ltd.
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

#pragma once

#include <cstdint>
#include <cstring>
#include <memory>
#include <numeric>
#include <span>
#include <string>
#include <vector>

#include "yacl/utils/byte_container_view.h"
#include "yacl/utils/int128.h"
#include "yacl/utils/ossl/defines.h"
#include "yacl/utils/secparam.h"

/* security parameter declaration */
YACL_MODULE_DECLARE("aes_all_modes", SecParam::C::k128, SecParam::S::INF);

namespace yacl {

// supported AES modes
enum class BlockCipherTy : int {
  AES128_ECB,  // ECB = Electronic Code Book
  AES128_CBC,  // CBC = Cipher Block Chaining
  AES128_CTR,  // CTR = Counter
  SM4_ECB,     // ECB = Electronic Code Book
  SM4_CBC,     // CBC = Cipher Block Chaining
  SM4_CTR,     // CTR = Counter
};

// This class implements Symmetric- crypto.
class BlockCipher {
 public:
  // constructor
  BlockCipher(BlockCipherTy type, uint128_t key, uint128_t iv = 0);
  BlockCipher(BlockCipherTy type, ByteContainerView key, ByteContainerView iv);

  // CBC Block Size.
  static constexpr int BlockSize() { return 128 / 8; }

  // Reset the internal contexts of SymmetricCrypto (enc_ctx_, dec_ctx)
  // NOTE: key_, type_, and initial_vector_ stay unchanged
  void Reset();

  // Encrypts `plaintext` into `ciphertext`.
  // Note the ciphertext/plaintext size must be `N * kBlockSize`.
  void Encrypt(std::span<const uint8_t> plaintext,
               std::span<uint8_t> ciphertext) const;

  // Decrypts `ciphertext` into `plaintext`.
  // Note the ciphertext/plaintext size must be `N * kBlockSize`.
  void Decrypt(std::span<const uint8_t> ciphertext,
               std::span<uint8_t> plaintext) const;

  // Wrapper for uint128.
  uint128_t Encrypt(uint128_t input) const;
  uint128_t Decrypt(uint128_t input) const;

  // Wrapper for span<uint128>.
  void Encrypt(std::span<const uint128_t> plaintext,
               std::span<uint128_t> ciphertext) const;
  void Decrypt(std::span<const uint128_t> ciphertext,
               std::span<uint128_t> plaintext) const;

  // Getter
  BlockCipherTy GetType() const { return type_; }

  //
  static void EcbMakeContentBlocks(uint128_t count, std::span<uint128_t> buf) {
    std::iota(buf.begin(), buf.end(), count);
  }

 private:
  const BlockCipherTy type_;  //  Crypto type
  const uint128_t key_;       // Symmetric key, 128 bits
  const uint128_t iv_;        // Initialize vector

  // openssl cipher contexts
  ossl::UniqueCipherCtx enc_ctx_;
  ossl::UniqueCipherCtx dec_ctx_;
};

class AesCbcCrypto : public BlockCipher {
 public:
  AesCbcCrypto(uint128_t key, uint128_t iv)
      : BlockCipher(BlockCipherTy::AES128_CBC, key, iv) {}
};

class Sm4CbcCrypto : public BlockCipher {
 public:
  Sm4CbcCrypto(uint128_t key, uint128_t iv)
      : BlockCipher(BlockCipherTy::SM4_CBC, key, iv) {}
};

// in some asymmetric scene
// may exist parties only need update count by buffer size.
inline uint64_t DummyUpdateRandomCount(uint64_t count, size_t buffer_size) {
  constexpr size_t block_size = BlockCipher::BlockSize();
  const size_t nblock = (buffer_size + block_size - 1) / block_size;
  return count + nblock;
}

/* to a string which openssl recognizes */
inline const char* ToString(BlockCipherTy type) {
  switch (type) {
      // see: https://www.openssl.org/docs/manmaster/man7/EVP_CIPHER-AES.html
      // see: https://www.openssl.org/docs/man3.0/man7/EVP_CIPHER-SM4.html
    case BlockCipherTy::AES128_ECB:
      return "aes-128-ecb";
    case BlockCipherTy::AES128_CBC:
      return "aes-128-cbc";
    case BlockCipherTy::AES128_CTR:
      return "aes-128-ctr";
    case BlockCipherTy::SM4_ECB:
      return "sm4-ecb";
    case BlockCipherTy::SM4_CBC:
      return "sm4-cbc";
    case BlockCipherTy::SM4_CTR:
      return "sm4-ctr";
    default:
      YACL_THROW("Unsupported symmetric encryption algo: {}",
                 static_cast<int>(type));
  }
}

}  // namespace yacl
