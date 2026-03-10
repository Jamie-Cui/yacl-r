# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

YACL-r (Yet Another Cryptographic Library for Research) is a C++ cryptographic library focused on secure computation protocols. It is a fork of [secretflow/yacl](https://github.com/secretflow/yacl) with additional research-oriented implementations. Target platforms: macOS Apple Silicon, Linux x86_64, Linux aarch64.

## Common Development Commands

```bash
# Configure (Ninja recommended)
cmake -S . -B build -G Ninja

# Build (Release by default)
cmake --build build

# Enable brpc transport (requires protobuf); all other modules always build
cmake -S . -B build -G Ninja -DENABLE_BRPC=On

# Debug build
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug

# Run all tests
cd build && ctest

# Run a specific test (name matches the *_test.cc filename without extension)
cd build && ctest -R buffer_test

# Run tests in parallel with verbose failure output
cd build && ctest -j$(nproc) --output-on-failure

# Install to default output/ directory
cmake --install build

# Format (pre-commit must be installed: pip install pre-commit && pre-commit install)
pre-commit run clang-format --all-files

# Run all pre-commit checks (clang-format, trailing-whitespace, codespell)
pre-commit run --all-files

# Build with fuzzing enabled (requires Clang)
cmake -S . -B build-fuzz -G Ninja \
  -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
  -DBUILD_FUZZ=On
cmake --build build-fuzz -j$(nproc)

# Run a fuzz target (e.g., hash_fuzz for 60 seconds)
./build-fuzz/bin/hash_fuzz -max_total_time=60

# Run with a corpus directory
mkdir -p corpus/hash && ./build-fuzz/bin/hash_fuzz corpus/hash -max_total_time=300
```

## Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `ENABLE_BRPC` | Off | Enable brpc transport (adds protobuf + brpc deps). Without this, ASIO-only transport is used. |
| `BUILD_TEST` | On | Build test binaries |
| `BUILD_FUZZ` | Off | Build fuzz targets (requires Clang with libFuzzer) |
| `BUILD_YACL_SHARED` | Off | Build as shared library instead of static |

All modules (base, crypto, math, utils, io, engine, kernel, link) always build. Only brpc/protobuf support is optional.

## Code Architecture

### Module Dependency Graph

```
engine → kernel → link → (ASIO or brpc network transport)
    │        │
    └────────┴────→ crypto (no network dependencies)
```

- **base**: Fundamental types (buffer, int128, exception, dynamic_bitset)
- **crypto**: Core cryptographic algorithms, no network dependency. Includes dpf, tpre, vss, sync_drbg modules
- **engine**: Interactive secure computation engines (requires kernel + link)
- **io**: Streaming-based I/O (circuit, kv, msgpack, rw, stream)
- **kernel**: Crypto kernel with network support — OT/VOLE implementations (requires link)
- **link**: RPC-based MPI framework for SPMD parallel programming
- **math**: Galois fields, big integers (mpint)
- **utils**: Parallelism, Hamming distance, matrix transpose, platform utils

### SPI (Service Provider Interface) Pattern

A key architectural pattern: pluggable implementations registered at static initialization time.

- `SpiFactoryBase<SPI_T>` manages library registration and selection
- Implementations register via `REGISTER_SPI_LIBRARY_HELPER(factory, name, perf_score, checker, creator)`
- SPI modules are built as OBJECT libraries and linked into the final `yacl` target to ensure registration code runs
- Examples: ECC (`EcGroupFactory` — OpenSSL, MCL, FourQ, Toy backends), Pairing (`PairingGroupFactory`), Galois Field (`GaloisFieldFactory`), DRBG (`DRBGFactory`)
- Key variable: `YACL_SPI_LIBS` collects all SPI object libraries in CMake

### Build System Internals

Source files are collected via three CMake variables propagated with `PARENT_SCOPE`:
- `YACL_SOURCE_FILES` — all `.cc` files (test files excluded at top level)
- `YACL_PROTO_SOURCE_FILES` — `.proto` files (only when `ENABLE_BRPC=On`)
- `YACL_SPI_LIBS` — SPI object library targets

These compile into `yacl_obj` (object lib) → `yacl` (static/shared lib, alias `Yacl::yacl`).

### Adding a Test

Tests use Google Test. In a subdirectory's `CMakeLists.txt`:

```cmake
add_yacl_test_if(my_feature_test)
```

This creates an executable from `my_feature_test.cc`, links `Yacl::yacl`, `gtest`, and `gmock`, and registers it with CTest. The test file lives alongside the code it tests.

### Adding a Fuzz Target

Fuzz targets use libFuzzer (Clang required). Create a `*_fuzz.cc` file alongside the source with an `LLVMFuzzerTestOneInput` entry point:

```cpp
#include <cstddef>
#include <cstdint>

#include "yacl/crypto/my_feature.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  // exercise the code under test with arbitrary input
  return 0;
}
```

Then add to the subdirectory's `CMakeLists.txt`:

```cmake
add_yacl_fuzz_if(my_feature_fuzz)
```

This creates an executable from `my_feature_fuzz.cc`, links `Yacl::yacl`, and compiles with `-fsanitize=fuzzer,address,undefined`. Fuzz targets are only built when `BUILD_FUZZ=On`.

### Adding Source Files

In subdirectory `CMakeLists.txt`, append to `YACL_SOURCE_FILES` and propagate:

```cmake
set(YACL_SOURCE_FILES ${YACL_SOURCE_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/my_feature.cc PARENT_SCOPE)
```

## Code Style

- Google C++ Style, enforced by clang-format v16 (`.clang-format` config: Google base, include regrouping)
- C++17 standard, `-Wall -Wextra -Werror`
- Pre-commit hooks: clang-format, trailing-whitespace, codespell
- Conventional commits: `feat:`, `fix:`, `docs:`, `test:`, `refactor:`, `chore:`, `perf:`

### Naming Conventions

| Type | Convention | Example |
|------|------------|---------|
| Files | `snake_case` | `buffer_test.cc` |
| Classes | `PascalCase` | `Buffer` |
| Functions | `PascalCase` | `GetData()` |
| Member variables | `snake_case_` (trailing underscore) | `buffer_size_` |
| Constants | `kPascalCase` | `kMaxSize` |
| Macros | `UPPER_SNAKE_CASE` | `YACL_VERSION` |

### Include Order (enforced by clang-format)

1. C system headers (`<*.h>`)
2. C++ standard headers (`<*>`)
3. Third-party headers (`"third_party/..."`)
4. Project headers (`"yacl/..."`)
5. Protobuf generated headers (`*.pb.h`)

### Header Guards

Use `#pragma once`. Namespaces follow directory structure: `yacl::crypto`, `yacl::math`, etc.
