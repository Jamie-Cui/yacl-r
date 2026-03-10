# AGENTS.md - Guidelines for AI Coding Agents

This document provides essential information for AI agents working on the YACL-r codebase.

## Build Commands

```bash
# Configure (Release build by default)
cmake -S . -B build -G Ninja

# Debug build
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug

# With brpc transport (requires protobuf)
cmake -S . -B build -G Ninja -DENABLE_BRPC=On

# Build
cmake --build build -j$(nproc)

# Build with fuzzing (requires Clang)
cmake -S . -B build-fuzz -G Ninja \
  -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
  -DBUILD_FUZZ=On
cmake --build build-fuzz -j$(nproc)
```

## Testing

```bash
cd build

# Run all tests
ctest

# Run single test (matches test binary name pattern)
ctest -R buffer_test

# Run with verbose output
ctest --output-on-failure

# Run specific test executable directly
./bin/buffer_test
```

## Linting and Formatting

```bash
# Run pre-commit checks on all files
pre-commit run --all-files

# Format specific file
clang-format -i path/to/file.cc

# Run clang-tidy
clang-tidy path/to/file.cc -- -p build
```

## Code Style Guidelines

### General Style
- Follow Google C++ Style Guide with Envoy extensions
- Use C++17 features where appropriate
- Use exceptions for error handling (encouraged in this codebase)

### File Organization
- Headers: `.h` extension
- Source: `.cc` extension
- Tests: `*_test.cc` suffix, placed alongside tested code
- Fuzz targets: `*_fuzz.cc` suffix, placed alongside tested code

### Naming Conventions
| Type | Convention | Example |
|------|------------|---------|
| Files | `snake_case` | `buffer_test.cc`, `prg.h` |
| Classes/Structs | `PascalCase` | `Buffer`, `Prg` |
| Functions/Methods | `PascalCase` (camelBack) | `GetData()`, `resize()` |
| Member Variables | `snake_case_` (trailing underscore) | `buffer_size_`, `ptr_` |
| Local Variables | `snake_case` | `input_size`, `result` |
| Constants | `kPascalCase` | `kMaxSize`, `kKey1` |
| Enum Constants | `UPPER_CASE` | `kMaxStackTraceDep` |
| Macros | `UPPER_SNAKE_CASE` | `YACL_ENFORCE` |
| Namespaces | `lowercase` | `yacl`, `yacl::crypto` |

### Include Order
1. Standard library headers (e.g., `<string>`, `<vector>`)
2. System headers (e.g., `<cstdint>`)
3. Third-party headers (e.g., `"gtest/gtest.h"`)
4. Project headers (e.g., `"yacl/base/buffer.h"`)
5. Protobuf headers (e.g., `"file.pb.h"`)

### Header Guards
Use `#pragma once` instead of traditional include guards.

## Error Handling

### YACL_ENFORCE Macro
Use `YACL_ENFORCE` for runtime assertions with formatted messages:
```cpp
YACL_ENFORCE(size >= 0, "Size must be non-negative, got {}", size);
YACL_ENFORCE(ptr != nullptr, "Pointer cannot be null");
```

### Exceptions
The codebase uses a custom `Exception` class:
```cpp
throw yacl::Exception("Something went wrong");
```

## Testing Guidelines

### Test Structure
```cpp
#include "gtest/gtest.h"
#include "yacl/crypto/my_feature.h"

namespace yacl::crypto {
namespace {

TEST(MyFeatureTest, BasicFunctionalityWorks) {
  auto result = MyFunction(input);
  EXPECT_TRUE(result.IsOk());
}

}  // namespace
}  // namespace yacl::crypto
```

### Test Naming
- Test suite: `<ClassOrFeature>Test` (e.g., `BufferTest`, not `buffer_test`)
- Test case: Descriptive name in PascalCase

### Adding New Tests
Add to the relevant `CMakeLists.txt`:
```cmake
add_yacl_test_if(my_feature_test)
```

### Adding New Fuzz Targets
Fuzz targets use libFuzzer and require Clang. Create a `*_fuzz.cc` file with an
`LLVMFuzzerTestOneInput` entry point, then add to the relevant `CMakeLists.txt`:
```cmake
add_yacl_fuzz_if(my_feature_fuzz)
```

## Project Structure

```
yacl-r/
├── yacl/                 # Main source directory
│   ├── base/             # Fundamental types (Buffer, int128, Exception)
│   ├── crypto/           # Cryptographic algorithms (no network dependencies)
│   ├── engine/           # Interactive engines
│   ├── io/               # Streaming I/O library
│   ├── kernel/           # Crypto kernel with network support
│   ├── link/             # RPC-based MPI framework
│   ├── math/             # Math library (galois_field, big integers)
│   └── utils/            # Utilities
├── cmake/                # CMake modules and dependency scripts
└── include/              # Public headers
```

## Pre-commit Hooks

```bash
pip install pre-commit
pre-commit install
```

Hooks include: trailing-whitespace, clang-format, codespell

## License Headers

All source files must include the Apache 2.0 license header:
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
```

## Important Notes

- Use `constexpr` for compile-time constants
- Prefer `std::byte` for raw memory buffers
- Use `int64_t` for sizes and indices (not `size_t`)
- Run tests from the build directory to ensure library paths are correct