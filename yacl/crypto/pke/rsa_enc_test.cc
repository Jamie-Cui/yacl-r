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

#include "yacl/crypto/pke/rsa_enc.h"

#include <string>

#include "gtest/gtest.h"

#include "yacl/base/exception.h"
#include "yacl/crypto/ossl_wrappers.h"

namespace yacl::crypto {

TEST(RsaEnc, EncryptDecrypt_shouldOk) {
  // GIVEN
  auto [pk, sk] = GenRsaKeyPairToPemBuf();
  std::string m = "I am a plaintext.";

  // WHEN
  auto enc_ctx = RsaEncryptor(pk);
  auto dec_ctx = RsaDecryptor(sk);

  auto c = enc_ctx.Encrypt(m);
  auto m_check = dec_ctx.Decrypt(c);

  // THEN
  EXPECT_EQ(std::memcmp(m.data(), m_check.data(), m.size()), 0);
}

TEST(RsaEnc, ShouldNotOk) {
  // GIVEN
  auto [pk1, sk1] = GenRsaKeyPairToPemBuf();
  auto [pk2, sk2] = GenRsaKeyPairToPemBuf();
  std::string m = "I am a plaintext.";

  // WHEN
  auto enc_ctx = RsaEncryptor(pk1);
  auto dec_ctx = RsaDecryptor(sk2);  // decrypt with wrong key

  auto c = enc_ctx.Encrypt(m);

  // THEN
  EXPECT_THROW(dec_ctx.Decrypt(c), yacl::Exception);
}

}  // namespace yacl::crypto
