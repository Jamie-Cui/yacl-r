// Copyright 2024 Jamie Cui
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

#include "yacl/crypto/hash/blake3.h"
#include "yacl/crypto/hash/ssl_hash.h"

namespace yacl::crypto {

// Select the appropriate SHA-2 variant based on field bit size.
inline HashAlgorithm SelectSha2ByBits(size_t bits) {
  if (bits <= 224) {
    return HashAlgorithm::SHA224;
  } else if (bits <= 256) {
    return HashAlgorithm::SHA256;
  } else if (bits <= 384) {
    return HashAlgorithm::SHA384;
  } else {
    return HashAlgorithm::SHA512;
  }
}

// Compute a hash of the input data using the given algorithm.
// For BLAKE3, field_bits controls the output length.
inline std::vector<uint8_t> HashForCurve(HashAlgorithm algo, size_t field_bits,
                                         ByteContainerView data) {
  if (algo != HashAlgorithm::BLAKE3) {
    return SslHash(algo).Update(data).CumulativeHash();
  }
  return Blake3Hash((field_bits + 7) / 8).Update(data).CumulativeHash();
}

}  // namespace yacl::crypto
