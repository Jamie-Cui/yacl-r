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

#include <algorithm>

#include "gtest/gtest.h"

#include "yacl/kem/rsa.h"
#include "yacl/utils/exception.h"
#include "yacl/utils/ossl/defines.h"

namespace yacl {

TEST(RsaKem, EncapsulateDecapsulate_shouldOk) {
  // GIVEN
  auto [pk, sk] = GenRsaKeyPairToPemBuf();

  // WHEN
  auto enc_ctx = RsaKemEncapsulator(pk);
  auto dec_ctx = RsaKemDecapsulator(sk);

  auto encapsulation = enc_ctx.Encapsulate();
  auto shared_secret =
      dec_ctx.Decapsulate(ByteContainerView(encapsulation.ciphertext));

  // THEN
  EXPECT_EQ(enc_ctx.Type(), KemTy::RSA_KEM);
  EXPECT_EQ(dec_ctx.Type(), KemTy::RSA_KEM);
  EXPECT_FALSE(encapsulation.ciphertext.empty());
  EXPECT_FALSE(encapsulation.shared_secret.empty());
  EXPECT_EQ(shared_secret, encapsulation.shared_secret);
}

TEST(RsaKem, WrongKeyShouldNotMatch) {
  // GIVEN
  auto [pk1, sk1] = GenRsaKeyPairToPemBuf();
  auto [pk2, sk2] = GenRsaKeyPairToPemBuf();

  // WHEN
  auto enc_ctx = RsaKemEncapsulator(pk1);
  auto dec_ctx = RsaKemDecapsulator(sk2);  // decapsulate with wrong key

  auto encapsulation = enc_ctx.Encapsulate();

  auto shared_secret =
      dec_ctx.Decapsulate(ByteContainerView(encapsulation.ciphertext));

  // THEN
  EXPECT_NE(shared_secret, encapsulation.shared_secret);
}

}  // namespace yacl
