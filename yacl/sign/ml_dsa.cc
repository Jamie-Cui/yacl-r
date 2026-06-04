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

#include "yacl/sign/ml_dsa.h"

#include <vector>

namespace yacl {

namespace {

ossl::UniqueSignature FetchSignatureAlg(const ossl::UniquePkey& pkey) {
  const char* alg_name = EVP_PKEY_get0_type_name(pkey.get());
  YACL_ENFORCE(alg_name != nullptr, "Cannot get ML-DSA key type name.");

  auto sig_alg =
      ossl::UniqueSignature(EVP_SIGNATURE_fetch(nullptr, alg_name, nullptr));
  YACL_ENFORCE(sig_alg != nullptr,
               "Cannot fetch ML-DSA signature algorithm {}. {}", alg_name,
               ossl::GetOSSLErr());
  return sig_alg;
}

}  // namespace

std::vector<uint8_t> MlDsaSigner::Sign(ByteContainerView message) const {
  auto ctx =
      ossl::UniquePkeyCtx(EVP_PKEY_CTX_new_from_pkey(nullptr, sk_.get(),
                                                     /* propquery */ nullptr));
  YACL_ENFORCE(ctx != nullptr);

  // ML-DSA signs messages, not pre-computed digests. OpenSSL exposes it via
  // the message-signing EVP path.
  auto sig_alg = FetchSignatureAlg(sk_);
  OSSL_RET_1(EVP_PKEY_sign_message_init(ctx.get(), sig_alg.get(),
                                        /* params */ nullptr));

  size_t outlen = 0;
  OSSL_RET_1(EVP_PKEY_sign(ctx.get(), /* sig */ nullptr, &outlen,
                           message.data(), message.size()));

  std::vector<uint8_t> out(outlen);
  OSSL_RET_1(EVP_PKEY_sign(ctx.get(), out.data(), &outlen, message.data(),
                           message.size()));
  out.resize(outlen);

  return out;
}

bool MlDsaVerifier::Verify(ByteContainerView message,
                           ByteContainerView signature) const {
  auto ctx =
      ossl::UniquePkeyCtx(EVP_PKEY_CTX_new_from_pkey(nullptr, pk_.get(),
                                                     /* propquery */ nullptr));
  YACL_ENFORCE(ctx != nullptr);

  auto sig_alg = FetchSignatureAlg(pk_);
  OSSL_RET_1(EVP_PKEY_verify_message_init(ctx.get(), sig_alg.get(),
                                          /* params */ nullptr));

  int ret = EVP_PKEY_verify(ctx.get(), signature.data(), signature.size(),
                            message.data(), message.size());
  YACL_ENFORCE(ret >= 0, ossl::GetOSSLErr());
  return ret == 1;
}

}  // namespace yacl
