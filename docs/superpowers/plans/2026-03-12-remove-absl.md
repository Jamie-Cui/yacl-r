# Remove Abseil Dependency — Implementation Plan

> **For agentic workers:** REQUIRED: Use superpowers:subagent-driven-development (if subagents available) or superpowers:executing-plans to implement this plan. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Remove all `absl` (Abseil) includes and symbols from the yacl-r codebase, with no new external dependencies.

**Architecture:** Upgrade to C++20 to get `std::span` and `<bit>` for free; write a minimal `yacl/base/strings.h` + `.cc` for string utilities; replace stack traces with POSIX `<execinfo.h>`; self-contain `scope_guard.h`; rewrite `int128.cc` ostream without absl; replace `absl::base_internal::DirectMmap` with POSIX `mmap()`.

**Tech Stack:** C++20, `<span>`, `<bit>`, `<execinfo.h>`, POSIX `mmap()`

---

## Chunk 1: Foundation — C++20 + strings.h + scope_guard

### Task 1: Upgrade C++ standard to C++20

**Files:**
- Modify: `CMakeLists.txt:87`
- Modify: `cmake/deps/abseil.cmake` (leave alone until Chunk 6 — abseil still needed during migration)

- [ ] **Step 1: Change C++ standard**

In `CMakeLists.txt`, change line 87:
```cmake
# Before:
set(CMAKE_CXX_STANDARD 17) # Enable C++17

# After:
set(CMAKE_CXX_STANDARD 20) # Enable C++20
```

- [ ] **Step 2: Verify build still works**

```bash
cmake -S . -B build -G Ninja
cmake --build build -j$(nproc) 2>&1 | tail -20
```
Expected: no new errors (absl still linked, only standard version changes)

- [ ] **Step 3: Commit**

```bash
git add CMakeLists.txt
git commit -m "chore: upgrade C++ standard from 17 to 20"
```

---

### Task 2: Create yacl/base/strings.h

**Files:**
- Create: `yacl/base/strings.h`
- Modify: `yacl/base/CMakeLists.txt` (add strings.cc to YACL_SOURCE_FILES)

- [ ] **Step 1: Write the header**

Create `yacl/base/strings.h`:
```cpp
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

#include <charconv>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace yacl {

// ---------------------------------------------------------------------------
// Hex encoding / decoding
// ---------------------------------------------------------------------------

// Encode raw bytes to lowercase hex string.
// Equivalent to absl::BytesToHexString(string_view).
std::string BytesToHexString(std::string_view bytes);

// Encode a span of bytes to lowercase hex string.
std::string BytesToHexString(std::span<const uint8_t> bytes);

// Decode a hex string into binary bytes stored in *out.
// Returns true on success, false if input is not valid hex.
// Equivalent to absl::HexStringToBytes(hex, out).
bool HexStringToBytes(std::string_view hex, std::string* out);

// ---------------------------------------------------------------------------
// String splitting / joining
// ---------------------------------------------------------------------------

// Split sv on every occurrence of delim.
// Returns a vector of non-owning views into sv (sv must outlive the result).
// Equivalent to absl::StrSplit(sv, delim) for char delimiter.
std::vector<std::string_view> StrSplit(std::string_view sv, char delim);

// Split sv on every occurrence of the string delimiter sep.
std::vector<std::string_view> StrSplit(std::string_view sv,
                                       std::string_view sep);

// Join elements of [begin,end) with separator sep.
// Elements are converted to string via fmt (or operator<<).
// For simple string ranges:
std::string StrJoin(const std::vector<std::string>& parts,
                    std::string_view sep);
std::string StrJoin(const std::vector<std::string_view>& parts,
                    std::string_view sep);

// Join a span of int64_t values with separator sep.
std::string StrJoin(std::span<const int64_t> parts, std::string_view sep);

// ---------------------------------------------------------------------------
// Number parsing
// ---------------------------------------------------------------------------

// Parse a decimal integer from sv into *out.
// Returns true on success.
// Equivalent to absl::SimpleAtoi(sv, out).
bool SimpleAtoi(std::string_view sv, int64_t* out);
bool SimpleAtoi(std::string_view sv, int32_t* out);
bool SimpleAtoi(std::string_view sv, uint64_t* out);
bool SimpleAtoi(std::string_view sv, uint32_t* out);

// Parse a hex integer from sv (without 0x prefix) into *out.
// Equivalent to absl::SimpleHexAtoi(sv, out).
bool SimpleHexAtoi(std::string_view sv, uint64_t* out);

// ---------------------------------------------------------------------------
// String matching
// ---------------------------------------------------------------------------

// Equivalent to absl::StrContains.
inline bool StrContains(std::string_view haystack, std::string_view needle) {
  return haystack.find(needle) != std::string_view::npos;
}

// Equivalent to absl::StartsWith.
inline bool StartsWith(std::string_view s, std::string_view prefix) {
  return s.size() >= prefix.size() &&
         s.substr(0, prefix.size()) == prefix;
}

// Equivalent to absl::EndsWith.
inline bool EndsWith(std::string_view s, std::string_view suffix) {
  return s.size() >= suffix.size() &&
         s.substr(s.size() - suffix.size()) == suffix;
}

// ---------------------------------------------------------------------------
// ASCII character utilities
// ---------------------------------------------------------------------------

// These are thin wrappers over <cctype> with proper unsigned-char casting.
// Equivalent to absl::ascii_is*().
inline bool AsciiIsDigit(char c) {
  return std::isdigit(static_cast<unsigned char>(c)) != 0;
}
inline bool AsciiIsAlpha(char c) {
  return std::isalpha(static_cast<unsigned char>(c)) != 0;
}
inline bool AsciiIsWhitespace(char c) {
  return std::isspace(static_cast<unsigned char>(c)) != 0;
}
inline bool AsciiIsAlNum(char c) {
  return std::isalnum(static_cast<unsigned char>(c)) != 0;
}
inline char AsciiToLower(char c) {
  return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
}
inline char AsciiToUpper(char c) {
  return static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
}

// Convert a string to lowercase in place.
// Equivalent to absl::AsciiStrToLower(str).
void AsciiStrToLower(std::string* s);

// Return a lowercase copy of sv.
std::string AsciiStrToLower(std::string_view sv);

}  // namespace yacl
```

- [ ] **Step 2: Write the implementation**

Create `yacl/base/strings.cc`:
```cpp
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

#include "yacl/base/strings.h"

#include <algorithm>
#include <charconv>
#include <cctype>
#include <sstream>
#include <stdexcept>

namespace yacl {

namespace {

constexpr char kHexChars[] = "0123456789abcdef";

}  // namespace

std::string BytesToHexString(std::string_view bytes) {
  std::string result;
  result.reserve(bytes.size() * 2);
  for (unsigned char c : bytes) {
    result.push_back(kHexChars[c >> 4]);
    result.push_back(kHexChars[c & 0xf]);
  }
  return result;
}

std::string BytesToHexString(std::span<const uint8_t> bytes) {
  return BytesToHexString(
      std::string_view(reinterpret_cast<const char*>(bytes.data()),
                       bytes.size()));
}

bool HexStringToBytes(std::string_view hex, std::string* out) {
  if (hex.size() % 2 != 0) return false;
  out->clear();
  out->reserve(hex.size() / 2);
  for (size_t i = 0; i < hex.size(); i += 2) {
    uint8_t hi, lo;
    auto r1 = std::from_chars(hex.data() + i, hex.data() + i + 1, hi, 16);
    auto r2 = std::from_chars(hex.data() + i + 1, hex.data() + i + 2, lo, 16);
    if (r1.ec != std::errc{} || r2.ec != std::errc{}) return false;
    out->push_back(static_cast<char>((hi << 4) | lo));
  }
  return true;
}

std::vector<std::string_view> StrSplit(std::string_view sv, char delim) {
  std::vector<std::string_view> result;
  size_t start = 0;
  for (size_t i = 0; i <= sv.size(); ++i) {
    if (i == sv.size() || sv[i] == delim) {
      result.push_back(sv.substr(start, i - start));
      start = i + 1;
    }
  }
  return result;
}

std::vector<std::string_view> StrSplit(std::string_view sv,
                                       std::string_view sep) {
  std::vector<std::string_view> result;
  if (sep.empty()) {
    result.push_back(sv);
    return result;
  }
  size_t start = 0;
  size_t pos;
  while ((pos = sv.find(sep, start)) != std::string_view::npos) {
    result.push_back(sv.substr(start, pos - start));
    start = pos + sep.size();
  }
  result.push_back(sv.substr(start));
  return result;
}

std::string StrJoin(const std::vector<std::string>& parts,
                    std::string_view sep) {
  std::string result;
  for (size_t i = 0; i < parts.size(); ++i) {
    if (i > 0) result.append(sep);
    result.append(parts[i]);
  }
  return result;
}

std::string StrJoin(const std::vector<std::string_view>& parts,
                    std::string_view sep) {
  std::string result;
  for (size_t i = 0; i < parts.size(); ++i) {
    if (i > 0) result.append(sep);
    result.append(parts[i]);
  }
  return result;
}

std::string StrJoin(std::span<const int64_t> parts, std::string_view sep) {
  std::string result;
  for (size_t i = 0; i < parts.size(); ++i) {
    if (i > 0) result.append(sep);
    result.append(std::to_string(parts[i]));
  }
  return result;
}

bool SimpleAtoi(std::string_view sv, int64_t* out) {
  auto r = std::from_chars(sv.data(), sv.data() + sv.size(), *out);
  return r.ec == std::errc{} && r.ptr == sv.data() + sv.size();
}

bool SimpleAtoi(std::string_view sv, int32_t* out) {
  auto r = std::from_chars(sv.data(), sv.data() + sv.size(), *out);
  return r.ec == std::errc{} && r.ptr == sv.data() + sv.size();
}

bool SimpleAtoi(std::string_view sv, uint64_t* out) {
  auto r = std::from_chars(sv.data(), sv.data() + sv.size(), *out);
  return r.ec == std::errc{} && r.ptr == sv.data() + sv.size();
}

bool SimpleAtoi(std::string_view sv, uint32_t* out) {
  auto r = std::from_chars(sv.data(), sv.data() + sv.size(), *out);
  return r.ec == std::errc{} && r.ptr == sv.data() + sv.size();
}

bool SimpleHexAtoi(std::string_view sv, uint64_t* out) {
  auto r = std::from_chars(sv.data(), sv.data() + sv.size(), *out, 16);
  return r.ec == std::errc{} && r.ptr == sv.data() + sv.size();
}

void AsciiStrToLower(std::string* s) {
  std::transform(s->begin(), s->end(), s->begin(),
                 [](unsigned char c) { return std::tolower(c); });
}

std::string AsciiStrToLower(std::string_view sv) {
  std::string result(sv);
  AsciiStrToLower(&result);
  return result;
}

}  // namespace yacl
```

- [ ] **Step 3: Add strings.cc to the build**

In `yacl/base/CMakeLists.txt`, find the `set(YACL_SOURCE_FILES ...)` block and add:
```cmake
set(YACL_SOURCE_FILES
    ${YACL_SOURCE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/strings.cc
    PARENT_SCOPE)
```

- [ ] **Step 4: Build to verify compilation**

```bash
cmake --build build -j$(nproc) 2>&1 | tail -20
```
Expected: builds successfully

- [ ] **Step 5: Commit**

```bash
git add yacl/base/strings.h yacl/base/strings.cc yacl/base/CMakeLists.txt
git commit -m "feat: add yacl/base/strings.h as absl/strings replacement"
```

---

### Task 3: Self-contain scope_guard.h

**Files:**
- Modify: `yacl/utils/scope_guard.h`

The current `scope_guard.h` only wraps `absl::Cleanup`. Replace it with a native C++
implementation.

- [ ] **Step 1: Rewrite scope_guard.h**

Replace the entire content of `yacl/utils/scope_guard.h` with:
```cpp
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
    if (active_) f_();
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
#define ON_SCOPE_EXIT(...)                                              \
  auto SCOPEGUARD_LINENAME(EXIT, __LINE__) = ::yacl::MakeScopeExit(__VA_ARGS__)
```

- [ ] **Step 2: Update mmapped_file.cc to use ON_SCOPE_EXIT**

In `yacl/io/rw/mmapped_file.cc`, replace:
```cpp
#include "absl/base/internal/direct_mmap.h"
#include "absl/cleanup/cleanup.h"
```
with:
```cpp
#include "yacl/utils/scope_guard.h"
```

Replace:
```cpp
  absl::Cleanup close_fd = [&fd]() {
    // By posix standard, close the file will
    // not unmap the region, so let's close
    // the file.
    close(fd);
  };
```
with:
```cpp
  ON_SCOPE_EXIT([&fd]() {
    // By posix standard, close the file will
    // not unmap the region, so let's close
    // the file.
    close(fd);
  });
```

Replace:
```cpp
  data_ = absl::base_internal::DirectMmap(nullptr, size_, PROT_READ,
                                          MAP_PRIVATE, fd, 0);
```
with:
```cpp
  data_ = mmap(nullptr, size_, PROT_READ, MAP_PRIVATE, fd, 0);
```

Replace:
```cpp
    absl::base_internal::DirectMunmap(data_, size_);
```
with:
```cpp
    munmap(data_, size_);
```

- [ ] **Step 3: Build to verify**

```bash
cmake --build build -j$(nproc) 2>&1 | grep -E "error:|warning:.*mmapped|warning:.*scope_guard" | head -20
```
Expected: no errors

- [ ] **Step 4: Commit**

```bash
git add yacl/utils/scope_guard.h yacl/io/rw/mmapped_file.cc
git commit -m "refactor: replace absl::Cleanup with native ScopeExit, use POSIX mmap directly"
```

---

## Chunk 2: yacl/base module

### Task 4: Fix yacl/base/int128.h and int128.cc

**Files:**
- Modify: `yacl/base/int128.h`
- Modify: `yacl/base/int128.cc`

- [ ] **Step 1: Fix int128.h — replace absl::countl_zero with std::countl_zero**

In `yacl/base/int128.h`:

Replace:
```cpp
#include "absl/numeric/bits.h"
```
with:
```cpp
#include <bit>
```

Replace (line ~82):
```cpp
  return hi == 0 ? absl::countl_zero(lo) + 64 : absl::countl_zero(hi);
```
with:
```cpp
  return hi == 0 ? std::countl_zero(lo) + 64 : std::countl_zero(hi);
```

- [ ] **Step 2: Fix int128.cc — remove absl::int128 dependency**

The `int128.cc` uses `absl::int128` only for `operator<<` and for `DecomposeInt128`/`DecomposeUInt128`.

Replace the entire content of `yacl/base/int128.cc` with:
```cpp
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

// Print uint128 in decimal.
std::ostream& PrintUInt128(std::ostream& os, uint128_t x) {
  if (x == 0) return os << '0';
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
    // Handle INT128_MIN without overflow: -(x+1)+1
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
```

- [ ] **Step 3: Build to verify**

```bash
cmake --build build -j$(nproc) 2>&1 | grep "int128" | head -20
```
Expected: no errors

- [ ] **Step 4: Run int128 tests**

```bash
cd build && ctest -R int128 --output-on-failure
```
Expected: PASS

- [ ] **Step 5: Commit**

```bash
git add yacl/base/int128.h yacl/base/int128.cc
git commit -m "refactor: remove absl dependency from int128 (use std::countl_zero, native ostream)"
```

---

### Task 5: Fix yacl/base/exception.h

**Files:**
- Modify: `yacl/base/exception.h`

The file uses:
- `absl/debugging/stacktrace.h` → `<execinfo.h>` (POSIX)
- `absl/debugging/symbolize.h` → `<execinfo.h>` (backtrace_symbols)
- `absl/strings/str_join.h` → `yacl/base/strings.h`
- `absl/types/span.h` → `<span>`

- [ ] **Step 1: Rewrite exception.h includes and absl symbols**

In `yacl/base/exception.h`, replace the block:
```cpp
#include "absl/debugging/stacktrace.h"
#include "absl/debugging/symbolize.h"
#include "absl/strings/str_join.h"
#include "absl/types/span.h"
```
with:
```cpp
#include <execinfo.h>

#include <span>

#include "yacl/base/strings.h"
```

Replace the fmt::formatter specialization for `absl::Span<const int64_t>`:
```cpp
template <>
struct fmt::formatter<absl::Span<const int64_t>> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(absl::Span<const int64_t> number, FormatContext& ctx) {
    return fmt::format_to(ctx.out(), "{}", absl::StrJoin(number, "x"));
  }
};

template <>
struct fmt::formatter<std::vector<int64_t>> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const std::vector<int64_t>& number, FormatContext& ctx) {
    return fmt::format_to(ctx.out(), "{}", absl::StrJoin(number, "x"));
  }
};
```
with:
```cpp
template <>
struct fmt::formatter<std::span<const int64_t>> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(std::span<const int64_t> number, FormatContext& ctx) {
    return fmt::format_to(ctx.out(), "{}", yacl::StrJoin(number, "x"));
  }
};

template <>
struct fmt::formatter<std::vector<int64_t>> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const std::vector<int64_t>& number, FormatContext& ctx) {
    return fmt::format_to(ctx.out(), "{}",
                          yacl::StrJoin(std::span<const int64_t>(number), "x"));
  }
};
```

Replace the constructor that uses `absl::Symbolize`:
```cpp
  explicit Exception(std::string msg, void** stacks, int dep,
                     bool append_stack_to_msg = false) {
    for (int i = 0; i < dep; ++i) {
      std::array<char, 2048> tmp;
      const char* symbol = "(unknown)";
      if (absl::Symbolize(stacks[i], tmp.data(), tmp.size())) {
        symbol = tmp.data();
      }
      stack_trace_.append(fmt::format("#{} {}+{}\n", i, symbol, stacks[i]));
    }
    ...
```
with:
```cpp
  explicit Exception(std::string msg, void** stacks, int dep,
                     bool append_stack_to_msg = false) {
    // Use backtrace_symbols for symbol resolution (POSIX).
    char** symbols = ::backtrace_symbols(stacks, dep);
    for (int i = 0; i < dep; ++i) {
      const char* symbol = (symbols != nullptr) ? symbols[i] : "(unknown)";
      stack_trace_.append(fmt::format("#{} {}\n", i, symbol));
    }
    if (symbols != nullptr) free(symbols);  // NOLINT(cppcoreguidelines-no-malloc)

    if (append_stack_to_msg) {
      msg_ = fmt::format("{}\nStacktrace:\n{}", msg, stack_trace_);
    } else {
      msg_ = std::move(msg);
    }
  }
```

Replace the `YACL_THROW_HELPER` macro that uses `absl::GetStackTrace`:
```cpp
#define YACL_THROW_HELPER(ExceptionName, AppendStack, ...)                     \
  do {                                                                         \
    ::yacl::stacktrace_t __stacks__;                                           \
    int __dep__ = absl::GetStackTrace(__stacks__.data(),                       \
                                      ::yacl::internal::kMaxStackTraceDep, 0); \
    throw ExceptionName(YACL_ERROR_MSG(__VA_ARGS__), __stacks__.data(),        \
                        __dep__, AppendStack);                                 \
  } while (false)
```
with:
```cpp
#define YACL_THROW_HELPER(ExceptionName, AppendStack, ...)                     \
  do {                                                                         \
    ::yacl::stacktrace_t __stacks__;                                           \
    int __dep__ = ::backtrace(__stacks__.data(),                               \
                              ::yacl::internal::kMaxStackTraceDep);            \
    throw ExceptionName(YACL_ERROR_MSG(__VA_ARGS__), __stacks__.data(),        \
                        __dep__, AppendStack);                                 \
  } while (false)
```

Replace the `YACL_ENFORCE` macro:
```cpp
#define YACL_ENFORCE(condition, ...)                                     \
  do {                                                                   \
    if (!(condition)) {                                                  \
      ::yacl::stacktrace_t __stacks__;                                   \
      const int __dep__ = absl::GetStackTrace(                           \
          __stacks__.data(), ::yacl::internal::kMaxStackTraceDep, 0);    \
```
with:
```cpp
#define YACL_ENFORCE(condition, ...)                                     \
  do {                                                                   \
    if (!(condition)) {                                                  \
      ::yacl::stacktrace_t __stacks__;                                   \
      const int __dep__ = ::backtrace(                                   \
          __stacks__.data(), ::yacl::internal::kMaxStackTraceDep);       \
```

Remove the comment block (lines 168-175) that refers to `absl::InitializeSymbolizer`:
```cpp
// add absl::InitializeSymbolizer to main function to get
// human-readable names stack trace
//
// Example:
// int main(int argc, char *argv[]) {
//   absl::InitializeSymbolizer(argv[0]);
//   ...
// }
```
(just delete it — `backtrace_symbols` works without initialization)

Also remove the stacktrace_t type alias line that references absl types, keeping only:
```cpp
using stacktrace_t = std::array<void*, ::yacl::internal::kMaxStackTraceDep>;
```
(this is already correct, just verify it's still there)

- [ ] **Step 2: Fix exception_test.cc**

In `yacl/base/exception_test.cc`, replace:
```cpp
#include "absl/strings/match.h"
```
with:
```cpp
#include "yacl/base/strings.h"
```

Replace:
```cpp
  EXPECT_TRUE(absl::StrContains(e.what(), expected));
```
with:
```cpp
  EXPECT_TRUE(yacl::StrContains(e.what(), expected));
```

- [ ] **Step 3: Build to verify**

```bash
cmake --build build -j$(nproc) 2>&1 | grep -E "error:.*exception|error:.*absl" | head -20
```
Expected: no errors

- [ ] **Step 4: Run exception tests**

```bash
cd build && ctest -R exception_test --output-on-failure
```
Expected: PASS

- [ ] **Step 5: Commit**

```bash
git add yacl/base/exception.h yacl/base/exception_test.cc
git commit -m "refactor: replace absl debugging with POSIX backtrace in exception.h"
```

---

### Task 6: Fix yacl/base/byte_container_view.h and block.h

**Files:**
- Modify: `yacl/base/byte_container_view.h`
- Modify: `yacl/base/block.h`

- [ ] **Step 1: Fix byte_container_view.h**

Read the file first. Replace `#include "absl/types/span.h"` with `#include <span>`.
Replace all `absl::Span<T>` with `std::span<T>` and `absl::MakeSpan(...)` with
`std::span(...)` and `absl::MakeConstSpan(...)` with `std::span<const ...>(...)`.

Run:
```bash
grep -n "absl::" yacl/base/byte_container_view.h
```
Then apply replacements manually or with sed for verification.

- [ ] **Step 2: Fix block.h**

```bash
grep -n "absl::" yacl/base/block.h
```
Apply the same `absl::Span` → `std::span` and `absl/types/span.h` → `<span>` replacements.

- [ ] **Step 3: Build to verify**

```bash
cmake --build build -j$(nproc) 2>&1 | grep -E "error:.*block\.h|error:.*byte_container" | head -20
```

- [ ] **Step 4: Commit**

```bash
git add yacl/base/byte_container_view.h yacl/base/block.h
git commit -m "refactor: replace absl::Span with std::span in base/byte_container_view.h and block.h"
```

---

## Chunk 3: yacl/utils module

### Task 7: Fix yacl/utils/hamming.h and yacl/utils/spi/

**Files:**
- Modify: `yacl/utils/hamming.h`
- Modify: `yacl/utils/spi/item.h`
- Modify: `yacl/utils/spi/item.cc`
- Modify: `yacl/utils/spi/argument/arg_k.h`
- Modify: `yacl/utils/spi/argument/arg_kv.h`
- Modify: `yacl/utils/spi/argument/arg_kv.cc`
- Modify: `yacl/utils/spi/argument/util.cc`
- Modify: `yacl/utils/spi/spi_factory.h`

- [ ] **Step 1: Fix hamming.h**

Replace `#include "absl/numeric/bits.h"` with `#include <bit>`.
Replace `absl::countl_zero(...)` with `std::countl_zero(...)`.
Replace `absl::popcount(...)` with `std::popcount(...)`.

- [ ] **Step 2: Fix spi/item.h**

Replace `#include "absl/types/span.h"` with `#include <span>`.
Replace `absl::Span<T>` → `std::span<T>` throughout.
Replace `absl::MakeSpan(...)` → `std::span(...)`.
Replace `absl::MakeConstSpan(...)` → `std::span<const T>(...)` (must specify element type).

Note: `absl::MakeSpan(&As<T>(), 1)` becomes `std::span<T>(&As<T>(), 1)`.
`absl::MakeSpan(As<std::vector<RawT>>())` becomes `std::span<RawT>(As<std::vector<RawT>>())`.

- [ ] **Step 3: Fix spi/item.cc**

Replace `absl::Span<type>` → `std::span<type>` in TRY_TYPE macro and IsAll method.
Replace `absl::Span<const bool>` → `std::span<const bool>`.
Replace `absl::Span<bool>` → `std::span<bool>`.

- [ ] **Step 4: Fix spi/argument/ files**

In `arg_kv.h` and `arg_kv.cc`:
- Replace `#include "absl/strings/ascii.h"` with nothing (use `yacl/base/strings.h`)
- Add `#include "yacl/base/strings.h"`
- Replace `absl::AsciiStrToLower(std::string(...))` with `yacl::AsciiStrToLower(std::string_view(...))`
- Replace `absl::AsciiStrToLower(value)` with `yacl::AsciiStrToLower(std::string_view(value))`

In `util.cc`:
- Replace `absl::AsciiStrToLower(...)` with `yacl::AsciiStrToLower(...)`
- Replace `absl::StrJoin(words, "_")` with `yacl::StrJoin(words, "_")`
  Note: `words` is a vector of strings — the `StrJoin` overload in strings.h handles this.

In `spi_factory.h`:
- Replace absl string/span includes and usages.

- [ ] **Step 5: Fix arg_k.h**

Replace `absl::Span` with `std::span`.

- [ ] **Step 6: Build utils module**

```bash
cmake --build build -j$(nproc) 2>&1 | grep -E "error:.*utils|error:.*spi" | head -20
```

- [ ] **Step 7: Commit**

```bash
git add yacl/utils/
git commit -m "refactor: remove absl from yacl/utils (span, strings, bits)"
```

---

## Chunk 4: yacl/crypto module

### Task 8: Fix all crypto files

**Files (22 total — grouped by type of change):**

The crypto module has two kinds of changes:
1. `absl::Span<T>` → `std::span<T>` (most files)
2. `absl::BytesToHexString` / `absl::HexStringToBytes` → yacl equivalents (11 files)
3. `absl::StrSplit` → `yacl::StrSplit` (oprf_ctx.h, ossl_provider/helper.h)
4. `absl::string_view` → `std::string_view`

**Strategy:** Do one grep-then-edit pass per file. Build after each logical group.

- [ ] **Step 1: Fix crypto/aead/ headers**

Files: `aead.h`, `all_gcm.h`
- Replace `absl/types/span.h` → `<span>`
- Replace `absl::Span<T>` → `std::span<T>`

- [ ] **Step 2: Fix crypto/aes/aes_intrinsics.h**

Same span replacement.

- [ ] **Step 3: Fix crypto/block_cipher/ files**

Files: `symmetric_crypto.h`, `symmetric_crypto_test.cc`
- Span replacements + hex string replacements in test.

- [ ] **Step 4: Fix crypto/ecc/ files**

Files: `curve_meta.cc`, `ecc_spi.cc`, `FourQlib/FourQ_group.cc`, `toy/montgomery.cc`
- In `montgomery.cc`: replace `absl::BytesToHexString(absl::string_view(buf.data(), buf.size()))` with `yacl::BytesToHexString(std::string_view((char*)buf.data(), buf.size()))`
- Other files: span replacements.

- [ ] **Step 5: Fix crypto/envelope/digital_envelope.cc**

Span + hex replacements.

- [ ] **Step 6: Fix crypto/hash/ test files**

Files: `blake3_test.cc`, `ssl_hash_all_test.cc`
- Replace `#include "absl/strings/escaping.h"` with `#include "yacl/base/strings.h"`
- Replace `absl::BytesToHexString(...)` with `yacl::BytesToHexString(...)`
- Replace `absl::HexStringToBytes(hex, &bytes)` with `yacl::HexStringToBytes(hex, &bytes)`

Note: `absl::BytesToHexString` takes `std::string_view` while test passes `std::string`.
`std::string` implicitly converts to `std::string_view` — no change needed at call site.

- [ ] **Step 7: Fix crypto/hmac/hmac_all_test.cc**

Same hex string replacements as Step 6.

- [ ] **Step 8: Fix crypto/oprf/oprf_ctx.h and oprf_test.cc**

In `oprf_ctx.h`:
```cpp
// Before:
std::vector<std::string> split = absl::StrSplit(str, '-');
// After:
std::vector<std::string_view> split = yacl::StrSplit(str, '-');
// Note: split elements are views into str. If std::string is required, copy:
// auto parts = yacl::StrSplit(str, '-');
// std::vector<std::string> split(parts.begin(), parts.end());
```

- [ ] **Step 9: Fix crypto/ossl_provider/helper.h**

Replace `absl::StrSplit(selfdir_str, "sandbox")` with `yacl::StrSplit(selfdir_str, "sandbox")`.
Note: `StrSplit` now returns `vector<string_view>` not `vector<string>`. Update variable
declarations from `std::vector<std::string>` to `std::vector<std::string_view>`, or add
explicit copies.

- [ ] **Step 10: Fix crypto/pairing/factory/mcl_factory.cc**

Span replacements.

- [ ] **Step 11: Fix crypto/tools/crhash.h and cuckoo_index.h**

Span replacements.

- [ ] **Step 12: Fix crypto/tpre/ test files**

Replace `absl::BytesToHexString(absl::string_view(...))` with `yacl::BytesToHexString(std::string_view(...))`.
Replace `absl::string_view` with `std::string_view`.

- [ ] **Step 13: Fix crypto/sync_drbg/sync_drbg_test.cc**

The usages are in comments (`// SPDLOG_INFO...`) — just replace `absl/strings/escaping.h`
include with `yacl/base/strings.h` and update the commented-out lines too for consistency.

- [ ] **Step 14: Build crypto module**

```bash
cmake --build build -j$(nproc) 2>&1 | grep -E "error:.*crypto" | head -40
```
Fix any remaining errors.

- [ ] **Step 15: Run crypto tests**

```bash
cd build && ctest -R "hash|hmac|block_cipher|aead|oprf|tpre|drbg|ecc" --output-on-failure
```
Expected: all PASS

- [ ] **Step 16: Commit**

```bash
git add yacl/crypto/
git commit -m "refactor: remove absl from yacl/crypto (span, strings, hex)"
```

---

## Chunk 5: Remaining modules — kernel, io, link, math, engine

### Task 9: Fix yacl/kernel module

**Files (12 files — all headers):**
`algorithms/base_ot.h`, `algorithms/base_ot_interface.h`, `algorithms/gywz_ote.h`,
`algorithms/iknp_ote.h`, `algorithms/kkrt_ote.h`, `algorithms/kos_ote.h`,
`algorithms/sgrr_ote.h`, `algorithms/softspoken_ote.h`,
`benchmark/ot_bench.h`,
`code/ea_code.h`, `code/linear_code.h`, `code/silver_code.h`

- [ ] **Step 1: Batch-replace span in all kernel headers**

For each file, run:
```bash
grep -n "absl::" yacl/kernel/algorithms/<file>.h
```
Then apply:
- `#include "absl/types/span.h"` → `#include <span>`
- `#include "absl/numeric/bits.h"` → `#include <bit>`
- `absl::Span<T>` → `std::span<T>`
- `absl::countl_zero` → `std::countl_zero`
- `absl::countr_zero` → `std::countr_zero`
- `absl::popcount` → `std::popcount`
- `absl::MakeSpan(...)` → `std::span(...)`

- [ ] **Step 2: Build kernel module**

```bash
cmake --build build -j$(nproc) 2>&1 | grep -E "error:.*kernel" | head -20
```

- [ ] **Step 3: Commit**

```bash
git add yacl/kernel/
git commit -m "refactor: remove absl from yacl/kernel (span, bits)"
```

---

### Task 10: Fix yacl/io module

**Files:** `circuit/bristol_fashion.cc`, `circuit/bristol_fashion.h`,
`kv/kvstore.h`, `rw/csv_reader.cc`, `rw/csv_reader.h`,
`rw/float.h`, `rw/mmapped_file.cc` (already done in Task 3), `rw/rw_test.cc`

- [ ] **Step 1: Fix bristol_fashion.cc**

Replace `#include "absl/strings/str_split.h"` and `#include "absl/strings/numbers.h"` with
`#include "yacl/base/strings.h"`.

Replace:
```cpp
std::vector<std::string> splits = absl::StrSplit(ret, ' ');
YACL_ENFORCE(absl::SimpleAtoi(splits[0], &circ_->ng));
```
with:
```cpp
auto split_views = yacl::StrSplit(ret, ' ');
std::vector<std::string> splits(split_views.begin(), split_views.end());
YACL_ENFORCE(yacl::SimpleAtoi(splits[0], &circ_->ng));
```
(Or change `circ_->ng` target type to match — check what type it is. If it's `int64_t`,
`SimpleAtoi(string_view, int64_t*)` works directly on the string_view.)

Better approach — use `split_views` directly without copying:
```cpp
auto splits = yacl::StrSplit(ret, ' ');
YACL_ENFORCE(yacl::SimpleAtoi(splits[0], &circ_->ng));
```
This works since `splits[0]` is a `std::string_view` and `SimpleAtoi` takes `std::string_view`.

Apply same pattern for all `absl::StrSplit` + `absl::SimpleAtoi` usages in the file.

- [ ] **Step 2: Fix bristol_fashion.h**

Span replacement if needed. Check:
```bash
grep -n "absl::" yacl/io/circuit/bristol_fashion.h
```

- [ ] **Step 3: Fix kv/kvstore.h**

Span replacement.

- [ ] **Step 4: Fix rw/csv_reader.cc and csv_reader.h**

Replace `absl/strings/ascii.h` → use `yacl/base/strings.h` (for `AsciiIsDigit` etc.)
Replace `absl::ascii_isdigit(c)` → `yacl::AsciiIsDigit(c)`.
Replace span usages.

- [ ] **Step 5: Fix rw/float.h**

Span replacement.

- [ ] **Step 6: Fix rw/rw_test.cc**

String/span replacements.

- [ ] **Step 7: Build io module**

```bash
cmake --build build -j$(nproc) 2>&1 | grep -E "error:.*io/" | head -20
```

- [ ] **Step 8: Run io tests**

```bash
cd build && ctest -R "rw_test|circuit|csv" --output-on-failure
```

- [ ] **Step 9: Commit**

```bash
git add yacl/io/
git commit -m "refactor: remove absl from yacl/io (strings, span)"
```

---

### Task 11: Fix yacl/link module

**Files:** `algorithm/broadcast.cc`, `trace.cc`, `transport/channel.cc`

- [ ] **Step 1: Fix broadcast.cc**

Replace `#include "absl/numeric/bits.h"` with `#include <bit>`.
Replace `absl::bit_floor(ctx->WorldSize())` with `std::bit_floor(ctx->WorldSize())`.

- [ ] **Step 2: Fix trace.cc**

Replace `#include "absl/strings/escaping.h"` with `#include "yacl/base/strings.h"`.
Replace `absl::BytesToHexString(content)` with `yacl::BytesToHexString(content)`.

- [ ] **Step 3: Fix transport/channel.cc**

Replace `absl/strings/numbers.h` include with `yacl/base/strings.h`.
Replace `absl::SimpleAtoi(absl::string_view(ptr, size), &out)` with
`yacl::SimpleAtoi(std::string_view(ptr, size), &out)`.
Replace `absl::string_view` with `std::string_view`.

- [ ] **Step 4: Build and test**

```bash
cmake --build build -j$(nproc) 2>&1 | grep -E "error:.*link/" | head -20
cd build && ctest -R "link|transport|broadcast" --output-on-failure 2>/dev/null || echo "(no link tests)"
```

- [ ] **Step 5: Commit**

```bash
git add yacl/link/
git commit -m "refactor: remove absl from yacl/link (bits, strings)"
```

---

### Task 12: Fix yacl/math and yacl/engine modules

**Files:**
- `math/gadget.h`
- `math/galois_field/factory/gf_vector.h`
- `engine/plaintext/executor_test.cc`
- `engine/secret_share/executor.h`
- `engine/secret_share/executor.cc`

- [ ] **Step 1: Fix math/gadget.h**

Replace `#include "absl/numeric/bits.h"` with `#include <bit>`.
Replace `absl::countl_zero(x)` with `std::countl_zero(x)`.

- [ ] **Step 2: Fix math/galois_field/factory/gf_vector.h**

Replace `absl::Span<T>` with `std::span<T>`.

- [ ] **Step 3: Fix engine files**

Replace `absl::Span<T>` → `std::span<T>` and hex string usages.

- [ ] **Step 4: Build and test**

```bash
cmake --build build -j$(nproc) 2>&1 | grep -E "error:.*math|error:.*engine" | head -20
cd build && ctest -R "executor|galois|plaintext" --output-on-failure
```

- [ ] **Step 5: Commit**

```bash
git add yacl/math/ yacl/engine/
git commit -m "refactor: remove absl from yacl/math and yacl/engine"
```

---

## Chunk 6: CMake Cleanup — Remove Abseil from Build

### Task 13: Remove absl from CMake

**Files:**
- Modify: `CMakeLists.txt`
- Delete: `cmake/deps/abseil.cmake`

Only do this AFTER all source changes are complete and tests pass.

- [ ] **Step 1: Full build + test verification**

```bash
cmake --build build -j$(nproc) 2>&1 | tail -5
cd build && ctest --output-on-failure -j$(nproc)
```
Expected: All tests PASS, zero compilation errors.

- [ ] **Step 2: Check for remaining absl includes in source tree**

```bash
grep -r '#include "absl/' yacl/ --include="*.h" --include="*.cc"
```
Expected: no output. If there are remaining files, fix them before proceeding.

- [ ] **Step 3: Remove absl from root CMakeLists.txt**

In `CMakeLists.txt`:
- Remove line `include(deps/abseil)` (around line 181)
- Remove `PUBLIC Deps::absl` from `target_link_libraries` (around line 264)

- [ ] **Step 4: Delete abseil.cmake**

```bash
rm cmake/deps/abseil.cmake
```

- [ ] **Step 5: Clean rebuild without absl**

```bash
rm -rf build
cmake -S . -B build -G Ninja
cmake --build build -j$(nproc) 2>&1 | tail -20
```
Expected: builds successfully without downloading or linking absl.

- [ ] **Step 6: Run full test suite**

```bash
cd build && ctest --output-on-failure -j$(nproc)
```
Expected: All tests PASS.

- [ ] **Step 7: Commit**

```bash
git add CMakeLists.txt
git rm cmake/deps/abseil.cmake
git commit -m "chore: remove abseil-cpp dependency from build system"
```

---

## Appendix: Quick Reference for Mechanical Replacements

When editing each file, apply these substitutions in order:

```
#include "absl/types/span.h"            → #include <span>
#include "absl/numeric/bits.h"          → #include <bit>
#include "absl/strings/string_view.h"   → #include <string_view>
#include "absl/strings/escaping.h"      → #include "yacl/base/strings.h"
#include "absl/strings/ascii.h"         → #include "yacl/base/strings.h"
#include "absl/strings/str_split.h"     → #include "yacl/base/strings.h"
#include "absl/strings/str_join.h"      → #include "yacl/base/strings.h"
#include "absl/strings/numbers.h"       → #include "yacl/base/strings.h"
#include "absl/strings/match.h"         → #include "yacl/base/strings.h"

absl::Span<T>                           → std::span<T>
absl::Span<const T>                     → std::span<const T>
absl::MakeSpan(ptr, len)                → std::span(ptr, len)
absl::MakeSpan(vec)                     → std::span(vec)
absl::MakeConstSpan(vec)                → std::span<const T>(vec)
absl::string_view                       → std::string_view

absl::countl_zero(x)                    → std::countl_zero(x)
absl::countr_zero(x)                    → std::countr_zero(x)
absl::popcount(x)                       → std::popcount(x)
absl::bit_floor(x)                      → std::bit_floor(x)

absl::BytesToHexString(sv)              → yacl::BytesToHexString(sv)
absl::HexStringToBytes(hex, out)        → yacl::HexStringToBytes(hex, out)
absl::StrSplit(sv, delim)               → yacl::StrSplit(sv, delim)
absl::StrJoin(range, sep)               → yacl::StrJoin(range, sep)
absl::SimpleAtoi(sv, out)               → yacl::SimpleAtoi(sv, out)
absl::SimpleHexAtoi(sv, out)            → yacl::SimpleHexAtoi(sv, out)
absl::StrContains(h, n)                 → yacl::StrContains(h, n)
absl::StartsWith(s, p)                  → yacl::StartsWith(s, p)
absl::EndsWith(s, s)                    → yacl::EndsWith(s, s)
absl::AsciiStrToLower(str)              → yacl::AsciiStrToLower(str)
absl::ascii_isdigit(c)                  → yacl::AsciiIsDigit(c)
absl::ascii_isalpha(c)                  → yacl::AsciiIsAlpha(c)
absl::ascii_isspace(c)                  → yacl::AsciiIsWhitespace(c)
absl::Cleanup var = [...]()             → ON_SCOPE_EXIT([...]())
```

> **Note on StrSplit return type:** `yacl::StrSplit` returns `vector<string_view>` while
> `absl::StrSplit` with `std::string` destination returns `vector<string>`. If callers
> store results in `vector<string>`, add explicit copy:
> ```cpp
> auto sv_parts = yacl::StrSplit(str, delim);
> std::vector<std::string> parts(sv_parts.begin(), sv_parts.end());
> ```

> **Note on HexStringToBytes:** `absl::HexStringToBytes` returns `bool` and takes
> `(std::string_view hex, std::string* out)`. The yacl replacement has the same signature.
