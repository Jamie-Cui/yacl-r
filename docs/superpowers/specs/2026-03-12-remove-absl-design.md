# Remove Abseil Dependency — Design Spec

**Date:** 2026-03-12
**Status:** Approved (Ralph-loop autonomous iteration)

## Problem

YACL-r depends on `abseil-cpp` (absl) across 63 files in all 8 modules. Absl is a large
dependency that backports C++17/20 features and provides string utilities. With the
codebase already on modern tooling targeting platforms that fully support C++20, absl can
be removed entirely.

## Approach: Upgrade to C++20 + Internal String Utilities

Upgrade the C++ standard from 17 → 20 to get `std::span` and `<bit>` for free. Write a
thin `yacl/base/strings.h` + `yacl/base/strings.cc` for string utilities. Replace all
absl symbols with their equivalents. Remove absl from CMake.

## Replacement Map

| absl symbol | replacement | header |
|---|---|---|
| `absl::Span<T>` | `std::span<T>` | `<span>` (C++20) |
| `absl::MakeSpan(...)` | `std::span(...)` | `<span>` |
| `absl::countl_zero` | `std::countl_zero` | `<bit>` (C++20) |
| `absl::countr_zero` | `std::countr_zero` | `<bit>` |
| `absl::popcount` | `std::popcount` | `<bit>` |
| `absl::string_view` | `std::string_view` | `<string_view>` (C++17) |
| `absl::BytesToHexString` | `yacl::BytesToHexString` | `yacl/base/strings.h` |
| `absl::HexStringToBytes` | `yacl::HexStringToBytes` | `yacl/base/strings.h` |
| `absl::StrSplit` | `yacl::StrSplit` | `yacl/base/strings.h` |
| `absl::StrJoin` | `yacl::StrJoin` | `yacl/base/strings.h` |
| `absl::SimpleAtoi` | `yacl::SimpleAtoi` | `yacl/base/strings.h` |
| `absl::SimpleHexAtoi` | `yacl::SimpleHexAtoi` | `yacl/base/strings.h` |
| `absl::StrContains` | `yacl::StrContains` | `yacl/base/strings.h` |
| `absl::StartsWith` | `yacl::StartsWith` | `yacl/base/strings.h` |
| `absl::EndsWith` | `yacl::EndsWith` | `yacl/base/strings.h` |
| `absl::AsciiIsDigit` | `std::isdigit` (cast to `unsigned char`) | `<cctype>` |
| `absl::AsciiIsAlpha` | `std::isalpha` (cast to `unsigned char`) | `<cctype>` |
| `absl::AsciiIsWhitespace` | `std::isspace` (cast to `unsigned char`) | `<cctype>` |
| `absl::AsciiToLower` | `std::tolower` (cast to `unsigned char`) | `<cctype>` |
| `absl::Cleanup` | `yacl::ScopeExit` | `yacl/utils/scope_guard.h` |
| `absl::GetStackTrace` | `::backtrace` | `<execinfo.h>` |
| `absl::Symbolize` | `::backtrace_symbols` | `<execinfo.h>` |
| `absl::InitializeSymbolizer` | removed | — |
| `absl::base_internal::DirectMmap` | `::mmap` | `<sys/mman.h>` |
| `absl::base_internal::DirectMunmap` | `::munmap` | `<sys/mman.h>` |
| `absl::int128` (in int128.cc) | implement via yacl's own `__int128` | — |

## Files to Create

- `yacl/base/strings.h` — string utility declarations
- `yacl/base/strings.cc` — string utility implementations

## Files to Modify

### Build System
- `CMakeLists.txt` — upgrade C++17→20, remove absl FetchContent + link

### yacl/base/
- `exception.h` — replace absl debugging with `<execinfo.h>`
- `int128.h` — replace `absl::countl_zero` with `std::countl_zero`
- `int128.cc` — remove absl::int128 dependency for operator<<
- `byte_container_view.h` — `absl::Span` → `std::span`
- `block.h` — `absl::Span` → `std::span`
- `exception_test.cc` — replace absl::StrContains with yacl::StrContains

### yacl/utils/
- `scope_guard.h` — ensure ScopeExit exists (add if missing)
- `hamming.h` — `absl::countl_zero` → `std::countl_zero`
- `spi/spi_factory.h` — absl string → yacl string
- `spi/item.h` — `absl::Span` → `std::span`
- `spi/argument/arg_k.h`, `arg_kv.h`, `util.cc` — absl string/span replacements

### yacl/crypto/ (22 files)
- All crypto headers/sources: `absl::Span` → `std::span`, hex/string utils

### yacl/kernel/ (12 files)
- All OT/VOLE algorithm headers: `absl::Span` → `std::span`

### yacl/io/ (8 files)
- CSV, circuit, kv: string split/numbers, `absl::Cleanup` → `yacl::ScopeExit`
- `rw/mmapped_file.cc` — use POSIX mmap directly

### yacl/link/ (3 files)
- Trace, broadcast, channel: hex strings, bit ops

### yacl/math/ (2 files)
- GF vector: `absl::Span` → `std::span`
- Gadget: `absl::Span` → `std::span`

### yacl/engine/ (3 files)
- Executor: `absl::Span` → `std::span`, hex strings

## Execution Order

1. Upgrade C++ standard to C++20 in CMakeLists.txt
2. Create `yacl/base/strings.h` + `.cc`
3. Audit and fix `yacl/utils/scope_guard.h` for `ScopeExit`
4. Fix `yacl/base/exception.h` (hardest — stack trace)
5. Fix `yacl/base/int128.{h,cc}`
6. Fix `yacl/base/byte_container_view.h` and `block.h`
7. Replace `absl::Span` across all modules (mechanical)
8. Replace string utilities across all modules (mechanical)
9. Fix `yacl/io/rw/mmapped_file.cc` (POSIX mmap)
10. Remove absl from CMakeLists.txt
11. Build, fix any remaining errors
12. Run full test suite

## Non-Goals

- Replacing the entire string utility API surface with something different
- Refactoring callers beyond what is required for compilation
- Adding new features

## Testing

- Build must succeed with `-Wall -Wextra -Werror`
- All existing tests must pass: `cd build && ctest --output-on-failure`
