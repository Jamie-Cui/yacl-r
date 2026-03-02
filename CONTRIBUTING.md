# Contributing to YACL-r

Thank you for your interest in contributing to YACL-r! This document provides guidelines and instructions for contributing.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [How to Contribute](#how-to-contribute)
- [Pull Request Process](#pull-request-process)
- [Coding Standards](#coding-standards)
- [Testing Guidelines](#testing-guidelines)
- [Reporting Issues](#reporting-issues)

## Code of Conduct

Be respectful and inclusive. We welcome contributions from everyone. Please be professional in all interactions.

## Getting Started

1. Fork the repository on GitHub
2. Clone your fork locally
3. Set up the development environment (see [GETTING_STARTED.md](GETTING_STARTED.md))
4. Create a feature branch for your changes

## Development Setup

See [GETTING_STARTED.md](GETTING_STARTED.md) for detailed setup instructions.

Quick setup:

```bash
# Install pre-commit hooks
pip install pre-commit
pre-commit install

# Configure and build
cmake -S . -B build -G Ninja
cmake --build build

# Run tests
cd build && ctest
```

## How to Contribute

### Reporting Bugs

1. Check existing issues to avoid duplicates
2. Use the issue template if available
3. Include:
   - Operating system and version
   - Compiler version
   - Steps to reproduce
   - Expected vs actual behavior
   - Relevant logs or error messages

### Suggesting Features

1. Open an issue with the "enhancement" label
2. Describe the feature and its use case
3. Explain why it would benefit the project

### Submitting Code

1. Create a feature branch: `git checkout -b feature/my-feature`
2. Make your changes following our coding standards
3. Add tests for new functionality
4. Ensure all tests pass: `cd build && ctest`
5. Run linting: `pre-commit run --all-files`
6. Commit with a clear message
7. Push and create a pull request

## Pull Request Process

1. **One feature per PR**: Keep changes focused
2. **Update documentation**: Update relevant docs (README, CHANGELOG, etc.)
3. **Add tests**: New code should have corresponding tests
4. **Pass CI**: All CI checks must pass
5. **Code review**: A maintainer will review your PR

### Commit Messages

Write clear, descriptive commit messages:

```
feat(crypto): add support for X25519 key exchange

- Implement X25519 key generation
- Add key exchange function
- Add unit tests

Closes #123
```

Use conventional commit prefixes:
- `feat:` New feature
- `fix:` Bug fix
- `docs:` Documentation changes
- `test:` Test additions/modifications
- `refactor:` Code refactoring
- `chore:` Maintenance tasks
- `perf:` Performance improvements

## Coding Standards

### C++ Style

- Follow [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- Use clang-format with the provided `.clang-format`
- Use clang-tidy with the provided `.clang-tidy`

### Naming Conventions

| Type | Convention | Example |
|------|------------|---------|
| Files | `snake_case` | `buffer_test.cc` |
| Classes | `PascalCase` | `Buffer` |
| Functions | `PascalCase` | `GetData()` |
| Variables | `snake_case_` | `buffer_size_` |
| Constants | `kPascalCase` | `kMaxSize` |
| Macros | `UPPER_SNAKE_CASE` | `YACL_VERSION` |

### Code Organization

```
yacl/crypto/my_feature/
├── CMakeLists.txt      # Build configuration
├── my_feature.h        # Public header
├── my_feature.cc       # Implementation
└── my_feature_test.cc  # Unit tests
```

### Headers

```cpp
// Copyright 2025 Jamie Cui
//
// Licensed under the Apache License, Version 2.0 (the "License");
// ...

#ifndef YACL_CRYPTO_MY_FEATURE_H_
#define YACL_CRYPTO_MY_FEATURE_H_

// Standard library
#include <string>
#include <vector>

// Third-party
#include "absl/strings/string_view.h"

// Project headers
#include "yacl/base/buffer.h"

namespace yacl::crypto {

// ...

}  // namespace yacl::crypto

#endif  // YACL_CRYPTO_MY_FEATURE_H_
```

## Testing Guidelines

### Test File Naming

- Use `*_test.cc` suffix
- Place tests alongside the code being tested

### Test Structure

```cpp
#include "gtest/gtest.h"

#include "yacl/crypto/my_feature.h"

namespace yacl::crypto {
namespace {

TEST(MyFeatureTest, BasicFunctionalityWorks) {
  // Arrange
  auto input = CreateTestInput();

  // Act
  auto result = MyFunction(input);

  // Assert
  EXPECT_TRUE(result.IsOk());
}

TEST(MyFeatureTest, HandlesInvalidInput) {
  EXPECT_THROW(MyFunction(InvalidInput()), std::invalid_argument);
}

}  // namespace
}  // namespace yacl::crypto
```

### Test Coverage

- Aim for meaningful coverage, not just numbers
- Test edge cases and error conditions
- Use parameterized tests for multiple inputs

## Reporting Issues

### Security Issues

**Do not open public issues for security vulnerabilities.**

Instead, please report security issues privately. See [SECURITY.md](SECURITY.md) for details.

### Regular Issues

1. Search existing issues first
2. Use a clear, descriptive title
3. Provide all requested information in the issue template

---

Thank you for contributing to YACL-r!