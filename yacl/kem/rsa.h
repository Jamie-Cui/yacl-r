// Copyright 2026 Ant Group Co., Ltd.
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

#include <utility>

#include "yacl/kem/kem.h"
#include "yacl/utils/ossl/key_utils.h"
#include "yacl/utils/secparam.h"

/* security parameter declaration */
YACL_MODULE_DECLARE("rsa_kem", SecParam::C::k128, SecParam::S::INF);

namespace yacl {

// RSA-KEM with RSASVE.
class RsaKemEncapsulator : public KemEncapsulator {
 public:
  explicit RsaKemEncapsulator(ossl::UniquePkey&& pk) : pk_(std::move(pk)) {}
  explicit RsaKemEncapsulator(/* pem key */ ByteContainerView pk_buf)
      : pk_(LoadKeyFromBuf(pk_buf)) {}

  KemTy Type() const override { return scheme_; }
  KemEncapsulation Encapsulate() override;

 private:
  const ossl::UniquePkey pk_;
  const KemTy scheme_ = KemTy::RSA_KEM;
};

class RsaKemDecapsulator : public KemDecapsulator {
 public:
  explicit RsaKemDecapsulator(ossl::UniquePkey&& sk) : sk_(std::move(sk)) {}
  explicit RsaKemDecapsulator(/* pem key */ ByteContainerView sk_buf)
      : sk_(LoadKeyFromBuf(sk_buf)) {}

  KemTy Type() const override { return scheme_; }
  std::vector<uint8_t> Decapsulate(ByteContainerView ciphertext) override;

 private:
  const ossl::UniquePkey sk_;
  const KemTy scheme_ = KemTy::RSA_KEM;
};

}  // namespace yacl
