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

#include "yacl/kem/rsa.h"

#include <vector>

#include "openssl/core_names.h"

#include "yacl/utils/exception.h"

namespace yacl {

KemEncapsulation RsaKemEncapsulator::Encapsulate() {
  // see: https://www.openssl.org/docs/man3.5/man3/EVP_PKEY_encapsulate.html
  auto ctx = ossl::UniquePkeyCtx(
      EVP_PKEY_CTX_new(pk_.get(), /* engine = default */ nullptr));
  YACL_ENFORCE(ctx != nullptr);

  OSSL_RET_1(EVP_PKEY_encapsulate_init(ctx.get(), nullptr));
  OSSL_RET_1(
      EVP_PKEY_CTX_set_kem_op(ctx.get(), OSSL_KEM_PARAM_OPERATION_RSASVE));

  size_t ciphertext_len = 0;
  size_t shared_secret_len = 0;
  OSSL_RET_1(EVP_PKEY_encapsulate(ctx.get(), nullptr, &ciphertext_len, nullptr,
                                  &shared_secret_len));

  KemEncapsulation out;
  out.ciphertext.resize(ciphertext_len);
  out.shared_secret.resize(shared_secret_len);
  OSSL_RET_1(EVP_PKEY_encapsulate(ctx.get(), out.ciphertext.data(),
                                  &ciphertext_len, out.shared_secret.data(),
                                  &shared_secret_len));

  out.ciphertext.resize(ciphertext_len);
  out.shared_secret.resize(shared_secret_len);
  return out;
}

std::vector<uint8_t> RsaKemDecapsulator::Decapsulate(
    ByteContainerView ciphertext) {
  // see: https://www.openssl.org/docs/man3.5/man3/EVP_PKEY_decapsulate.html
  auto ctx = ossl::UniquePkeyCtx(
      EVP_PKEY_CTX_new(sk_.get(), /* engine = default */ nullptr));
  YACL_ENFORCE(ctx != nullptr);

  OSSL_RET_1(EVP_PKEY_decapsulate_init(ctx.get(), nullptr));
  OSSL_RET_1(
      EVP_PKEY_CTX_set_kem_op(ctx.get(), OSSL_KEM_PARAM_OPERATION_RSASVE));

  size_t shared_secret_len = 0;
  OSSL_RET_1(EVP_PKEY_decapsulate(ctx.get(), nullptr, &shared_secret_len,
                                  ciphertext.data(), ciphertext.size()));

  std::vector<uint8_t> shared_secret(shared_secret_len);
  OSSL_RET_1(EVP_PKEY_decapsulate(ctx.get(), shared_secret.data(),
                                  &shared_secret_len, ciphertext.data(),
                                  ciphertext.size()));

  shared_secret.resize(shared_secret_len);
  return shared_secret;
}

}  // namespace yacl
