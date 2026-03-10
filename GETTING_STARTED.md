# Getting Started Guide

This guide provides detailed instructions for developing with YACL-r.

## Table of Contents

- [Prerequisites](#prerequisites)
- [Building from Source](#building-from-source)
- [Running Tests](#running-tests)
- [Project Structure](#project-structure)
- [Development Workflow](#development-workflow)
- [Coding Style](#coding-style)
- [Debugging Tips](#debugging-tips)

## Prerequisites

### Linux (Ubuntu/Debian)

```bash
# Install essential build tools
sudo apt update
sudo apt install -y git cmake ninja-build gcc g++

# Install dependencies
sudo apt install -y libgflags-dev libprotobuf-dev protobuf-compiler

# For GMP (big integer library)
sudo apt install -y libgmp-dev

# Perl is required by OpenSSL build
sudo apt install -y perl
```

### macOS (Apple Silicon)

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Homebrew packages
brew install cmake ninja gcc git gmp protobuf gflags perl
```

### Compiler Requirements

- GCC >= 10.3
- Clang >= 12 (alternative)

## Building from Source

### Quick Start

```bash
# Clone the repository
git clone https://github.com/Jamie-Cui/yacl-r.git
cd yacl-r

# Configure (Release build by default)
cmake -S . -B build -G Ninja

# Build
cmake --build build
```

### Build Options

```bash
# Enable brpc transport (requires protobuf); without this, ASIO-only transport is used
cmake -S . -B build -G Ninja -DENABLE_BRPC=On

# Disable tests
cmake -S . -B build -G Ninja -DBUILD_TEST=Off

# Debug build
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
```

### Build Configurations

| Option | Default | Description |
|--------|---------|-------------|
| `ENABLE_BRPC` | Off | Enable brpc transport (adds protobuf + brpc deps) |
| `BUILD_TEST` | On | Build test binaries |

## Running Tests

```bash
# Run all tests
cd build && ctest

# Run specific test (matches *_test.cc filename)
cd build && ctest -R buffer_test

# Run tests with verbose output
cd build && ctest --output-on-failure

# Run tests in parallel
cd build && ctest -j$(nproc)
```

## Project Structure

```
yacl-r/
├── yacl/                 # Main source directory
│   ├── base/             # Fundamental types (buffer, int128, exception)
│   ├── crypto/           # Cryptographic algorithms (no network dependencies)
│   │   ├── aead/         # Authenticated encryption
│   │   ├── aes/          # AES implementations
│   │   ├── ecc/          # Elliptic curve cryptography
│   │   ├── hash/         # Hash functions
│   │   ├── experimental/ # Research-grade code (dpf, tpre, vss)
│   │   └── ...
│   ├── engine/           # Interactive engines
│   ├── io/               # Streaming I/O library
│   ├── kernel/           # Crypto kernel with network support
│   ├── link/             # RPC-based MPI framework
│   ├── math/             # Math library (galois_field, big integers)
│   └── utils/            # Utilities
├── cmake/                # CMake modules and dependency scripts
├── include/              # Public headers
└── build/                # Build output directory
```

## Development Workflow

### Setting Up Pre-commit Hooks

```bash
# Install pre-commit (requires Python)
pip install pre-commit

# Install the hooks
pre-commit install

# Run manually on all files
pre-commit run --all-files
```

### Making Changes

1. Create a feature branch: `git checkout -b feature/my-feature`
2. Make your changes
3. Run tests: `cd build && ctest`
4. Run pre-commit: `pre-commit run --all-files`
5. Commit your changes
6. Push and create a pull request

### Code Organization

- **Headers**: Use `.h` extension
- **Source**: Use `.cc` extension
- **Tests**: Use `*_test.cc` suffix, placed alongside the code being tested
- **Protobuf**: Use `.proto` extension for protocol definitions

## Coding Style

YACL-r follows Google C++ Style Guide with clang-format enforcement.

### Format Configuration

The `.clang-format` file configures formatting rules. Key points:

- Based on Google style
- Include sorting is enabled
- 4-space indentation

### Running clang-format

```bash
# Format specific file
clang-format -i path/to/file.cc

# Format all files (via pre-commit)
pre-commit run clang-format --all-files
```

### Naming Conventions

- **Files**: `snake_case.cc`, `snake_case.h`
- **Classes**: `PascalCase`
- **Functions**: `PascalCase` (Google style)
- **Variables**: `snake_case_` (with trailing underscore for members)
- **Constants**: `kPascalCase`
- **Macros**: `UPPER_SNAKE_CASE`

## Debugging Tips

### Build with Debug Symbols

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

### Enable Sanitizers

```bash
# Address Sanitizer (ASan)
cmake -S . -B build -G Ninja -DCMAKE_CXX_FLAGS="-fsanitize=address -g"

# Undefined Behavior Sanitizer (UBSan)
cmake -S . -B build -G Ninja -DCMAKE_CXX_FLAGS="-fsanitize=undefined -g"
```

### IDE Integration

The build generates `compile_commands.json` for IDE/LSP integration:

```bash
# The file is generated at:
build/compile_commands.json

# Link it for IDEs that expect it at project root
ln -s build/compile_commands.json .
```

### Common Issues

**Build fails with missing dependencies**

Ensure all prerequisites are installed. Check cmake output for specific missing dependencies.

**Tests fail with "library not found"**

Set `LD_LIBRARY_PATH` or run tests from the build directory:

```bash
cd build && ctest
```

**Compilation errors on ARM/macOS**

Some compiler flags are x86-specific. Check platform-specific build options.

## Further Reading

- [ALGORITHMS.md](ALGORITHMS.md) - List of implemented algorithms
- [STYLE.md](STYLE.md) - Additional style guidelines
- [CHANGELOG.md](CHANGELOG.md) - Version history