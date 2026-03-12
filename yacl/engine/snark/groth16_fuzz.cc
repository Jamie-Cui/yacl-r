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

#include <cstddef>
#include <cstdint>

#include "yacl/crypto/pairing/pairing.h"
#include "yacl/engine/snark/serialization.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (size < 24) {
    return 0;  // too small to be a valid proof
  }

  try {
    auto pairing =
        yacl::crypto::PairingGroupFactory::Instance().Create("bn_snark1");

    yacl::ByteContainerView buf(data, size);
    auto proof =
        yacl::engine::snark::DeserializeProof(buf, std::move(pairing));
  } catch (...) {
    // Expected: malformed input should throw, not crash.
  }

  return 0;
}
