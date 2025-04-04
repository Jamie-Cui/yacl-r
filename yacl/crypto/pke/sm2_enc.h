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

#include <memory>
#include <utility>
#include <vector>

#include "yacl/crypto/key_utils.h"
#include "yacl/crypto/pke/pke_interface.h"
#include "yacl/base/secparam.h"

/* security parameter declaration */
YACL_MODULE_DECLARE("sm2_enc", SecParam::C::k128, SecParam::S::INF);

namespace yacl::crypto {

// SM2
class Sm2Encryptor : public PkeEncryptor {
 public:
  explicit Sm2Encryptor(ossl::UniquePkey&& pk) : pk_(std::move(pk)) {}
  explicit Sm2Encryptor(ByteContainerView pk_buf)
      : pk_(LoadKeyFromBuf(pk_buf)) {}

  PkeScheme GetScheme() const override { return scheme_; }
  std::vector<uint8_t> Encrypt(ByteContainerView plaintext) override;

 private:
  const ossl::UniquePkey pk_;
  const PkeScheme scheme_ = PkeScheme::SM2;
};

class Sm2Decryptor : public PkeDecryptor {
 public:
  explicit Sm2Decryptor(
      ossl::UniquePkey&& sk /* should contain (sk, pk) pair */)
      : sk_(std::move(sk)) {}
  explicit Sm2Decryptor(ByteContainerView sk_buf)
      : sk_(LoadKeyFromBuf(sk_buf)) {}

  PkeScheme GetScheme() const override { return scheme_; }
  std::vector<uint8_t> Decrypt(ByteContainerView ciphertext) override;

 private:
  const ossl::UniquePkey sk_;
  const PkeScheme scheme_ = PkeScheme::SM2;
};

}  // namespace yacl::crypto
