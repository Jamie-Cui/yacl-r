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
#include <string>

#include "yacl/utils/serializer.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  auto view = yacl::ByteContainerView(data, size);

  // Try deserializing as various types; malformed input should not crash
  try {
    yacl::DeserializeVars<int64_t>(view);
  } catch (...) {
  }

  try {
    yacl::DeserializeVars<std::string>(view);
  } catch (...) {
  }

  try {
    yacl::DeserializeVars<bool>(view);
  } catch (...) {
  }

  try {
    yacl::DeserializeVars<int64_t, bool, std::string>(view);
  } catch (...) {
  }

  return 0;
}
