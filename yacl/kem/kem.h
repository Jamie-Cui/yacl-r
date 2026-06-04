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

#include <cstdint>
#include <vector>

#include "yacl/utils/byte_container_view.h"

namespace yacl {

enum class KemTy : uint8_t { UNKNOWN, RSA_KEM, EC_DHKEM };

struct KemEncapsulation {
  std::vector<uint8_t> ciphertext;
  std::vector<uint8_t> shared_secret;
};

class KemEncapsulator {
 public:
  virtual ~KemEncapsulator() = default;
  virtual KemTy Type() const = 0;
  virtual KemEncapsulation Encapsulate() = 0;
};

class KemDecapsulator {
 public:
  virtual ~KemDecapsulator() = default;
  virtual KemTy Type() const = 0;
  virtual std::vector<uint8_t> Decapsulate(ByteContainerView ciphertext) = 0;
};

}  // namespace yacl
