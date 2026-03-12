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

#pragma once

#include <cstdint>

namespace yacl::engine {

// Additive share in Z_{2^64}.
// Invariant: share_P0.v + share_P1.v == secret (mod 2^64).
struct AShare {
  uint64_t v{0};
};

// Boolean (XOR) share in GF(2)^64.
// Invariant: share_P0.v XOR share_P1.v == secret.
struct BShare {
  uint64_t v{0};
};

// Beaver multiplication triple for arithmetic Z_{2^64}.
// Each party holds one triple struct.
// Invariant: (a_P0 + a_P1) * (b_P0 + b_P1) == (c_P0 + c_P1) (mod 2^64).
struct ATriple {
  uint64_t a{0};
  uint64_t b{0};
  uint64_t c{0};
};

// Beaver AND triple for boolean GF(2)^64.
// Each party holds one triple struct.
// Invariant: (a_P0 ^ a_P1) & (b_P0 ^ b_P1) == (c_P0 ^ c_P1).
struct BTriple {
  uint64_t a{0};
  uint64_t b{0};
  uint64_t c{0};
};

}  // namespace yacl::engine
