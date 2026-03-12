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

#include <utility>

namespace yacl {

// RAII scope-exit guard. Calls the stored callable on destruction.
// Equivalent to absl::Cleanup.
//
// Usage:
//   ON_SCOPE_EXIT([&]() { cleanup(); });
template <typename F>
class ScopeExit {
 public:
  explicit ScopeExit(F&& f) : f_(std::forward<F>(f)), active_(true) {}
  ~ScopeExit() {
    if (active_) {
      f_();
    }
  }
  ScopeExit(ScopeExit&& other) noexcept
      : f_(std::move(other.f_)), active_(other.active_) {
    other.active_ = false;
  }
  ScopeExit(const ScopeExit&) = delete;
  ScopeExit& operator=(const ScopeExit&) = delete;
  ScopeExit& operator=(ScopeExit&&) = delete;

  void Cancel() { active_ = false; }

 private:
  F f_;
  bool active_;
};

template <typename F>
ScopeExit<std::decay_t<F>> MakeScopeExit(F&& f) {
  return ScopeExit<std::decay_t<F>>(std::forward<F>(f));
}

}  // namespace yacl

#define SCOPEGUARD_LINENAME_CAT(name, line) name##line
#define SCOPEGUARD_LINENAME(name, line) SCOPEGUARD_LINENAME_CAT(name, line)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ON_SCOPE_EXIT(...) \
  auto SCOPEGUARD_LINENAME(EXIT, __LINE__) = ::yacl::MakeScopeExit(__VA_ARGS__)
