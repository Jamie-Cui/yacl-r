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

#pragma once

#include "yacl/utils/secparam.h"

// Get the RSA public key size (in bits) from computational security parameter
// see: NIST Special Publication 800-57
size_t GetRsaPkSize(SecParam::C c) {
  switch (c) {
    case SecParam::C::k112:
      return 2048;
    case SecParam::C::k128:
      return 3072;
    case SecParam::C::k192:
      return 7680;
    case SecParam::C::k256:
      return 15360;
    default:
      YACL_THROW("Unsupported security parameter for RSA: {}",
                 SecParam::MakeInt(c));
  }
}

size_t GetMlDsaMatrixSize(SecParam::PqcCat c) {
  switch (c) {
    case SecParam::PqcCat::k2:
      return 44;
    case SecParam::PqcCat::k3:
      return 65;
    case SecParam::PqcCat::k5:
      return 87;
    default:
      YACL_THROW("Unsupported security parameter for ML-DSA: {}",
                 SecParam::MakeInt(c));
  }
}
