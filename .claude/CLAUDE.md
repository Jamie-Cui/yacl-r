# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

YACL-r (Yet Another Cryptographic Library for Research) is a C++ cryptographic library focused on secure computation protocols. It is a fork and extension of the secretflow/yacl library with additional research-oriented cryptographic implementations.

Key features:
- Implements state-of-the-art secure computation protocols
- Supports primitives like OT (Oblivious Transfer), VOLE (Vector OLE), TPRE (Threshold Proxy Re-encryption)
- Provides tools like PRG (Pseudo-Random Generator), RO (Random Oracle)
- Target platforms: MacOS Apple Silicon, Linux x86_64, and Linux aarch64

## Code Architecture

The library is organized into modules with dependency relationships:

```
engine → kernel → link → (network dependencies)
    │        │
    └────────┴────→ crypto (no network dependencies)
```

- **base**: Fundamental types (buffer, int128, exception, dynamic_bitset)
- **crypto**: Core cryptographic algorithms (no network). Submodules include: aead, aes, block_cipher, ecc, hash, hmac, oprf, pairing, pke, rand, sign, tools. The `experimental/` subdirectory contains research-grade code (dpf, tpre, vss).
- **engine**: Interactive engines for specific purposes (requires kernel + link)
- **io**: Streaming-based I/O library (circuit, kv, msgpack, rw, stream)
- **kernel**: Crypto kernel with network support - OT/VOLE implementations (requires link)
- **link**: RPC-based MPI framework for SPMD parallel programming
- **math**: Math library (galois_field, mpint for big integers)
- **utils**: Utilities (parallel, hamming, matrix_transpose, platform_utils)

## Build System

CMake options (all default to Off except BUILD_TEST):
- `ENABLE_ENGINE`: Enable engine module (auto-enables kernel and link)
- `ENABLE_KERNEL`: Enable kernel module (auto-enables link)
- `ENABLE_LINK`: Enable link/network module
- `BUILD_TEST`: Build test binaries (default: On)

Dependencies: GCC >= 10.3, ninja-build, Perl 5, gflags, protobuf, GMP

## Common Development Commands

```bash
# Configure
cmake -S . -B build

# Build (release by default)
cmake --build build

# Build with networking/crypto kernel support
cmake -S . -B build -DENABLE_KERNEL=On -DENABLE_LINK=On

# Run all tests
cd build && ctest

# Run specific test (name matches *_test.cc filename without extension)
cd build && ctest -R buffer_test

# Install to default output/ directory
cmake --install build
```

## Code Style

- Google C++ style (enforced by clang-format via pre-commit)
- Install hooks: `pre-commit install`
- Format all files: `pre-commit run clang-format --all-files`

## Testing

- Tests use Google Test framework
- Named `*_test.cc`, added via `add_yacl_test_if()` macro in CMakeLists.txt
- Located alongside the code they test