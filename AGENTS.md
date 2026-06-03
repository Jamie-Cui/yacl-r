# AGENTS.md - Guidelines for AI Coding Agents

This document gives AI agents the project-specific commands and conventions for
working on YACL-r.

## Build Commands

```bash
# Configure a Release build.
cmake -S . -B build -G Ninja

# Configure a Debug build.
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug

# Build.
cmake --build build -j$(nproc)

# Build with fuzzing. Requires Clang/libFuzzer.
cmake -S . -B build-fuzz -G Ninja \
  -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
  -DBUILD_FUZZ=On
cmake --build build-fuzz -j$(nproc)
```

## Testing

Run tests from the build directory so CTest properties and runtime paths are
applied correctly.

```bash
cd build

# Run all tests.
ctest --output-on-failure

# Run a single test by executable/test name pattern.
ctest -R buffer_test --output-on-failure

# Run a specific test executable directly when no CTest working-directory
# property is needed.
./bin/buffer_test
```

## Formatting And Linting

No pre-commit hook configuration is maintained in this repo. Use focused manual
checks when needed:

```bash
# Format a specific file.
clang-format -i path/to/file.cc

# Run clang-tidy.
clang-tidy path/to/file.cc -- -p build
```

## Code Style Guidelines

### General Style

- Follow Google C++ Style Guide with Envoy extensions.
- Use C++20 features where appropriate.
- Use exceptions for error handling.
- Keep edits scoped to the relevant module.

### File Organization

- Headers: `.h` extension.
- Source: `.cc` extension.
- Tests: `*_test.cc` suffix, placed alongside tested code.
- Fuzz targets: `*_fuzz.cc` suffix, placed alongside tested code.

### Naming Conventions

| Type | Convention | Example |
|------|------------|---------|
| Files | `snake_case` | `buffer_test.cc`, `prg.h` |
| Classes/Structs | `PascalCase` | `Buffer`, `Prg` |
| Functions/Methods | `PascalCase` or existing local style | `GetData()`, `resize()` |
| Member Variables | `snake_case_` | `buffer_size_`, `ptr_` |
| Local Variables | `snake_case` | `input_size`, `result` |
| Constants | `kPascalCase` | `kMaxSize`, `kKey1` |
| Macros | `UPPER_SNAKE_CASE` | `YACL_ENFORCE` |
| Namespaces | `lowercase` | `yacl`, `mpc` |

### Include Order

1. Standard library headers, e.g. `<string>`, `<vector>`.
2. System headers, e.g. `<cstdint>`.
3. Third-party headers, e.g. `"gtest/gtest.h"`.
4. Project headers, e.g. `"yacl/utils/buffer.h"`.

Use `#pragma once` in headers.

## Error Handling

Use `YACL_ENFORCE` for runtime assertions with formatted messages:

```cpp
YACL_ENFORCE(size >= 0, "Size must be non-negative, got {}", size);
YACL_ENFORCE(ptr != nullptr, "Pointer cannot be null");
```

The codebase also uses the custom `Exception` class:

```cpp
throw yacl::Exception("Something went wrong");
```

## Testing Guidelines

### Test Structure

```cpp
#include "gtest/gtest.h"
#include "yacl/my_feature.h"

namespace yacl {
namespace {

TEST(MyFeatureTest, BasicFunctionalityWorks) {
  auto result = MyFunction(input);
  EXPECT_TRUE(result.IsOk());
}

}  // namespace
}  // namespace yacl
```

### Test Naming

- Test suite: `<ClassOrFeature>Test`, e.g. `BufferTest`.
- Test case: descriptive PascalCase.

### Adding New Tests

Add the test to the relevant `CMakeLists.txt`:

```cmake
add_yacl_test_if(my_feature_test)
```

If the test depends on source-tree data files, set a CTest working directory for
that test instead of relying on the process current directory.

### Adding New Fuzz Targets

Fuzz targets use libFuzzer and require Clang. Create a `*_fuzz.cc` file with an
`LLVMFuzzerTestOneInput` entry point, then add it to the relevant
`CMakeLists.txt`:

```cmake
add_yacl_fuzz_if(my_feature_fuzz)
```

## Project Structure

```text
yacl-r/
├── yacl/
│   ├── aead/             # AEAD implementations
│   ├── aes/              # AES intrinsics and wrappers
│   ├── bc/               # Block-cipher helpers
│   ├── dpf/              # DPF/DCF/PPRF protocols
│   ├── envelope/         # Envelope encryption
│   ├── experimental/     # Experimental PRG/RO/code utilities
│   ├── hash/             # Hashing utilities
│   ├── hmac/             # HMAC
│   ├── io/               # Streaming I/O and circuit parsing
│   ├── link/             # RPC-based SPMD framework
│   ├── math/             # Big integers, fields, ECC, pairing
│   ├── mpc/              # MPC kernels and executors
│   ├── oprf/             # OPRF
│   ├── ot/               # Base OT, OT extension, OT stores
│   ├── pke/              # Public-key encryption
│   ├── rand/             # Randomness, DRBG, entropy source code
│   ├── sign/             # Signatures
│   ├── snark/            # SNARK-related code
│   ├── tpre/             # Threshold proxy re-encryption
│   ├── utils/            # Common utilities
│   └── vole/             # VOLE and MPFSS code
└── cmake/                # CMake modules and dependency scripts
```

## License Notes

There is no automated license-header check configured. Preserve existing
license headers when editing files, but do not add or enforce a repository-wide
license check unless it is explicitly requested.

## Important Notes

- Use `constexpr` for compile-time constants.
- Prefer `std::byte` for raw memory buffers when it fits the local code.
- Prefer `int64_t` for sizes and indices when adding new APIs, unless local code
  already uses another type.
- Run tests from the build directory to ensure CTest properties are applied.
