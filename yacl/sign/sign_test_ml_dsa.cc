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

#include <string>

#include "gtest/gtest.h"

#include "yacl/sign/ml_dsa.h"
#include "yacl/utils/ossl/defines.h"

namespace yacl {

TEST(MlDsaSigning, SignVerify_shouldOk) {
  // GIVEN
  auto [pk, sk] = GenMlDsaKeyPairToPemBuf();
  std::string plaintext = "I am a plaintext.";

  // WHEN & THEN
  auto S = MlDsaSigner(sk);
  auto signature = S.Sign(plaintext);
  auto V = MlDsaVerifier(pk);
  EXPECT_TRUE(V.Verify(plaintext, signature));
}

TEST(MlDsaSigning, SignVerify_shouldFail_wrong_message) {
  // GIVEN
  auto [pk, sk] = GenMlDsaKeyPairToPemBuf();
  std::string plaintext = "I am a plaintext.";
  std::string faketext = "I am a faketext.";

  // WHEN & THEN
  auto S = MlDsaSigner(sk);
  auto signature = S.Sign(plaintext);
  auto V = MlDsaVerifier(pk);
  EXPECT_FALSE(V.Verify(faketext, signature));
}

TEST(MlDsaSigning, SignVerify_shouldFail_wrong_signature) {
  // GIVEN
  auto [pk, sk] = GenMlDsaKeyPairToPemBuf();
  std::string plaintext = "I am a plaintext.";
  std::string faketext = "I am a faketext.";

  // WHEN & THEN
  auto S = MlDsaSigner(sk);
  auto fake_signature = S.Sign(faketext);
  auto V = MlDsaVerifier(pk);
  EXPECT_FALSE(V.Verify(plaintext, fake_signature));
}

TEST(MlDsaSigning, SignVerify_shouldFail_wrong_key) {
  // GIVEN
  auto [pk1, sk1] = GenMlDsaKeyPairToPemBuf();
  auto [pk2, sk2] = GenMlDsaKeyPairToPemBuf();

  std::string plaintext = "I am a plaintext.";

  // WHEN & THEN
  auto S = MlDsaSigner(sk1);
  auto signature = S.Sign(plaintext);
  auto V = MlDsaVerifier(pk2);
  EXPECT_FALSE(V.Verify(plaintext, signature));
}

TEST(MlDsaSigning, SignVerifyInitWithCert_shouldOk) {
  // GIVEN
  auto [pk_buf, sk_buf] = GenMlDsaKeyPairToPemBuf(); /* pkey */
  auto pk = LoadKeyFromBuf(pk_buf);
  auto sk = LoadKeyFromBuf(sk_buf);
  auto cert = MakeX509Cert(pk, sk,
                           {
                               {"C", "CN"},
                               {"ST", "ZJ"},
                               {"L", "HZ"},
                               {"O", "TEE"},
                               {"OU", "EGG"},
                               {"CN", "demo.trustedegg.com"},
                           },
                           3, HashTy::SHA256);
  auto cert_buf = ExportX509CertToBuf(cert);

  std::string plaintext = "I am a plaintext.";

  // WHEN & THEN
  /* signer */
  auto S = MlDsaSigner(sk_buf);
  auto signature = S.Sign(plaintext);

  /* verifier */
  auto cert_pk = LoadX509CertPublicKeyFromBuf(cert_buf);
  auto V = MlDsaVerifier(std::move(cert_pk));
  EXPECT_TRUE(V.Verify(plaintext, signature));
}

}  // namespace yacl
