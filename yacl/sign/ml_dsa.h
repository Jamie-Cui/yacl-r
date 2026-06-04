// Copyright 2026 Jamie Cui
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
#include <vector>

#include "yacl/sign/sign.h"
#include "yacl/utils/ossl/key_utils.h"
#include "yacl/utils/secparam.h"

/* submodules */
#include "yacl/hash/hash_utils.h"

/* TODO security parameter declaration */

namespace yacl {

// RSA sign with sha256 (wrapper for OpenSSL)
class MlDsaSigner final : public Signer {
 public:
  // constructors and destrucors
  explicit MlDsaSigner(ossl::UniquePkey&& sk) : sk_(std::move(sk)) {}
  explicit MlDsaSigner(/* pem key */ ByteContainerView sk_buf)
      : sk_(LoadKeyFromBuf(sk_buf)) {}

  // return the scheme name
  SignTy Type() const override { return scheme_; }

  // sign a message with stored private key
  std::vector<uint8_t> Sign(ByteContainerView message) const override;

 private:
  const ossl::UniquePkey sk_;
  const SignTy scheme_ = SignTy::ML_DSA;
};

// RSA verify with sha256 (wrapper for OpenSSL)
class MlDsaVerifier final : public Verifier {
 public:
  // constructors and destrucors
  explicit MlDsaVerifier(ossl::UniquePkey&& pk) : pk_(std::move(pk)) {}
  explicit MlDsaVerifier(/* pem key */ ByteContainerView pk_buf)
      : pk_(LoadKeyFromBuf(pk_buf)) {}

  // return the scheme name
  SignTy Type() const override { return scheme_; }

  // verify a message and its signature with stored public key
  bool Verify(ByteContainerView message,
              ByteContainerView signature) const override;

 private:
  const ossl::UniquePkey pk_;
  const SignTy scheme_ = SignTy::ML_DSA;
};

}  // namespace yacl
