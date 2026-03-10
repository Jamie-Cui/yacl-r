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

#include "yacl/base/buffer.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  // Limit input size to avoid OOM from large allocations during resize/reserve
  constexpr size_t kMaxInputSize = 1 << 20;  // 1 MiB
  if (size > kMaxInputSize) {
    return 0;
  }

  // Construct a buffer from raw bytes
  yacl::Buffer buf(data, size);

  // Exercise copy construction
  yacl::Buffer buf_copy(buf);

  // Exercise move construction
  yacl::Buffer buf_moved(std::move(buf_copy));

  // Exercise resize (grow and shrink)
  if (buf_moved.size() > 0) {
    buf_moved.resize(buf_moved.size() * 2);
    buf_moved.resize(buf_moved.size() / 4);
  }

  // Exercise reserve
  buf_moved.reserve(buf_moved.size() + 64);

  // Exercise equality
  yacl::Buffer buf2(data, size);
  (void)(buf_moved == buf2);

  return 0;
}
