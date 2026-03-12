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

#pragma once

#include <cstring>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>

#include "yacl/base/buffer.h"

namespace yacl {

// ByteContainerView is an extension to std::span<const uint8_t> that can take
// various containers that can store bytes (e.g., std::string,
// std::vector<uint8_t>, etc.) and be used as a span.
class ByteContainerView : public std::span<const uint8_t> {
 public:
  using std::span<const uint8_t>::span;

  template <typename T, std::enable_if_t<sizeof(T) == 1, bool> = true>
  /* implicit */ ByteContainerView(const T *v, size_t size)
      : std::span<const uint8_t>(reinterpret_cast<const uint8_t *>(v), size) {}

  /* implicit */ ByteContainerView(const void *v, size_t size)
      : std::span<const uint8_t>(static_cast<const uint8_t *>(v), size) {}

  template <typename ByteContainer,
            std::enable_if_t<sizeof(typename ByteContainer::value_type) == 1,
                             bool> = true>
  /* implicit */ ByteContainerView(const ByteContainer &u)
      : std::span<const uint8_t>(
            reinterpret_cast<const uint8_t *>(u.data()), u.size()) {}

  template <size_t N>
  explicit ByteContainerView(const char bytes[N])
      : std::span<const uint8_t>(reinterpret_cast<const uint8_t *>(bytes), N) {
  }

  // Do not mark explicit, so pointer decay can happen
  /* implicit */ ByteContainerView(const char *s)
      : std::span<const uint8_t>(reinterpret_cast<const uint8_t *>(s),
                                 s != nullptr ? std::strlen(s) : 0) {}

  explicit ByteContainerView(const Buffer &b)
      : std::span<const uint8_t>(b.data<uint8_t>(), b.size()) {}

  // std::span<const T> has begin()/end() but not cbegin()/cend().
  // Provide them for compatibility with code that expects cbegin/cend.
  constexpr auto cbegin() const noexcept { return begin(); }
  constexpr auto cend() const noexcept { return end(); }

  bool operator==(const ByteContainerView &other) const {
    if (data() == other.data() && size() == other.size()) {
      return true;
    }
    if (size() == other.size()) {
      return std::memcmp(data(), other.data(), size()) == 0;
    }
    return false;
  }

  operator std::string_view() const {
    return empty() ? std::string_view()
                   : std::string_view(reinterpret_cast<const char *>(data()),
                                      size());
  }
};

}  // namespace yacl
