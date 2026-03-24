// Copyright 2024 Jamie Cui
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

#include "gtest/gtest.h"

#include "yacl/base/secparam.h"
#include "yacl/crypto/aead/aead.h"
#include "yacl/crypto/dpf/pprf.h"
#include "yacl/crypto/envelope/digital_envelope.h"
#include "yacl/crypto/hash/blake3.h"
#include "yacl/crypto/hash/hash_utils.h"
#include "yacl/crypto/hash/ssl_hash.h"
#include "yacl/crypto/hmac/hmac.h"
#include "yacl/crypto/hmac/hmac_sha256.h"
#include "yacl/crypto/hmac/hmac_sm3.h"
#include "yacl/crypto/pke/pke_interface.h"
#include "yacl/crypto/sign/signing.h"
#include "yacl/crypto/tools/crhash.h"
#include "yacl/crypto/tools/ro.h"
#include "yacl/crypto/tools/rp.h"
#include "yacl/kernel/algorithms/base_ot_interface.h"

namespace yacl::crypto {
namespace {

TEST(SecParamCoverageTest, PublicHeadersDeclareExpectedModules) {
  EXPECT_EQ(YACL_MODULE_SECPARAM_C("pprf"), SecParam::C::k128);
  EXPECT_EQ(YACL_MODULE_SECPARAM_S("pprf"), SecParam::S::INF);

  EXPECT_EQ(YACL_MODULE_SECPARAM_C("sm_env"), SecParam::C::k128);
  EXPECT_EQ(YACL_MODULE_SECPARAM_S("sm_env"), SecParam::S::INF);
  EXPECT_EQ(YACL_MODULE_SECPARAM_C("rsa_env"), SecParam::C::k128);
  EXPECT_EQ(YACL_MODULE_SECPARAM_S("rsa_env"), SecParam::S::INF);

  EXPECT_EQ(YACL_MODULE_SECPARAM_C("sha256_hash"), SecParam::C::k128);
  EXPECT_EQ(YACL_MODULE_SECPARAM_S("sha256_hash"), SecParam::S::INF);
  EXPECT_EQ(YACL_MODULE_SECPARAM_C("sm3_hash"), SecParam::C::k128);
  EXPECT_EQ(YACL_MODULE_SECPARAM_S("sm3_hash"), SecParam::S::INF);

#ifndef YACL_WITH_TONGSUO
  EXPECT_EQ(YACL_MODULE_SECPARAM_C("blake2_hash"), SecParam::C::k256);
  EXPECT_EQ(YACL_MODULE_SECPARAM_S("blake2_hash"), SecParam::S::INF);
#endif

  EXPECT_EQ(YACL_MODULE_SECPARAM_C("hmac_sha256"), SecParam::C::k128);
  EXPECT_EQ(YACL_MODULE_SECPARAM_S("hmac_sha256"), SecParam::S::INF);
  EXPECT_EQ(YACL_MODULE_SECPARAM_C("hmac_sm3"), SecParam::C::k128);
  EXPECT_EQ(YACL_MODULE_SECPARAM_S("hmac_sm3"), SecParam::S::INF);

  EXPECT_EQ(YACL_MODULE_SECPARAM_C("crhash_128"), SecParam::C::k128);
  EXPECT_EQ(YACL_MODULE_SECPARAM_S("crhash_128"), SecParam::S::INF);
  EXPECT_EQ(YACL_MODULE_SECPARAM_C("ccrhash_128"), SecParam::C::k128);
  EXPECT_EQ(YACL_MODULE_SECPARAM_S("ccrhash_128"), SecParam::S::INF);
}

}  // namespace
}  // namespace yacl::crypto
