// Copyright 2022 Ant Group Co., Ltd.
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

#include "yacl/base/int128.h"

#include <algorithm>
#include <utility>

namespace {

// Print a uint128 value in decimal to the given ostream.
std::ostream& PrintUInt128(std::ostream& os, uint128_t x) {
  if (x == 0) {
    return os << '0';
  }
  char buf[40];
  int len = 0;
  while (x > 0) {
    buf[len++] = '0' + static_cast<int>(x % 10);
    x /= 10;
  }
  std::reverse(buf, buf + len);
  return os.write(buf, len);
}

}  // namespace

namespace std {

std::ostream& operator<<(std::ostream& os, int128_t x) {
  if (x < 0) {
    os << '-';
    // Avoid INT128_MIN overflow: -(x+1)+1 stays in uint128_t range.
    return PrintUInt128(
        os, static_cast<uint128_t>(-(x + 1)) + static_cast<uint128_t>(1));
  }
  return PrintUInt128(os, static_cast<uint128_t>(x));
}

std::ostream& operator<<(std::ostream& os, uint128_t x) {
  return PrintUInt128(os, x);
}

}  // namespace std

namespace yacl {

std::pair<int64_t, uint64_t> DecomposeInt128(int128_t v) {
  return {static_cast<int64_t>(static_cast<uint128_t>(v) >> 64),
          static_cast<uint64_t>(static_cast<uint128_t>(v))};
}

std::pair<uint64_t, uint64_t> DecomposeUInt128(uint128_t v) {
  return {static_cast<uint64_t>(v >> 64), static_cast<uint64_t>(v)};
}

}  // namespace yacl
