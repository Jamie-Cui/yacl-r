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

The library is organized into several modules:

1. **base**: Fundamental types and utilities
2. **crypto**: Core cryptographic algorithms without network dependencies
3. **engine**: Interactive engines designed for specific purposes
4. **io**: Streaming-based I/O library
5. **kernel**: Crypto kernel with network support (link module) and multi-threading (WIP)
6. **link**: Simple RPC-based MPI framework for SPMD parallel programming
7. **math**: Simplified math library supporting big integers
8. **utils**: Additional utility functions

## Build System

The project uses CMake as its primary build system with the following key options:
- `ENABLE_ENGINE`: Enable the engine module (default: Off)
- `ENABLE_KERNEL`: Enable the kernel module (default: Off)
- `ENABLE_LINK`: Enable the link (network) module (default: Off)
- `BUILD_TEST`: Build testing binaries (default: On)

Important build dependencies:
- GCC >= 10.3
- Ninja/ninja-build
- Perl 5 with core modules
- gflags
- protobuf

## Common Development Commands

### Building the Project
```bash
# Configure the project
cmake -S . -B build

# Build the project
cmake --build build

# Build with specific options
cmake -S . -B build -DENABLE_KERNEL=On -DENABLE_LINK=On
```

### Running Tests
```bash
# Run all tests
cd build && ctest

# Run tests with verbose output
cd build && ctest --output-on-failure

# Run a specific test
cd build && ctest -R test_name
```

### Code Formatting
The project uses clang-format with Google style guidelines:
```bash
# Format code (requires pre-commit hooks)
pre-commit run clang-format --all-files
```

## Testing Structure

Tests are written using Google Test framework and follow the naming convention:
- Test files are named `*_test.cc`
- Tests are added to CMake using the `add_yacl_test` macro
- Tests are typically located alongside the code they test

## Code Style

- Follows Envoy C++ style guidelines with appropriate exceptions
- Uses Google C++ style for formatting (enforced by clang-format)
- Header files are organized with specific include categories and priorities