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

#include <vector>

#include "yacl/utils/byte_container_view.h"

/* security parameter declaration */
// this header dispatches between concrete signature schemes at runtime,
// concrete scheme headers declare their own security parameters

namespace yacl {

enum class SignTy : uint8_t {
  UNKNOWN,
  SM2_SM3,
  RSA_SHA256,
  ML_DSA,
};

class Signer {
 public:
  virtual ~Signer() = default;
  virtual SignTy Type() const = 0;
  virtual std::vector<uint8_t> Sign(ByteContainerView message) const = 0;
};

class Verifier {
 public:
  virtual ~Verifier() = default;
  virtual SignTy Type() const = 0;
  virtual bool Verify(ByteContainerView message,
                      ByteContainerView signature) const = 0;
};

}  // namespace yacl
