# Yet Another Cryptographic Library for Research

```
██╗   ██╗ █████╗  ██████╗██╗           ██████╗
╚██╗ ██╔╝██╔══██╗██╔════╝██║           ██╔══██╗
 ╚████╔╝ ███████║██║     ██║     █████╗██████╔╝
  ╚██╔╝  ██╔══██║██║     ██║     ╚════╝██╔══██╗
   ██║   ██║  ██║╚██████╗███████╗      ██║  ██║
   ╚═╝   ╚═╝  ╚═╝ ╚═════╝╚══════╝      ╚═╝  ╚═╝
```

Yacl-r is a fork and extension of the C++ crypto library
[secretflow/yacl](https://github.com/secretflow/yacl). It provides research
implementations of cryptographic and secure-computation building blocks,
including OT, VOLE, TPRE, DPF, PRG, RO, AES, hashing, public-key encryption,
signatures, and supporting math/link utilities.

> [!WARNING]
> Yacl-r is under heavy development. APIs, module layout, and build options may
> change.

Target platforms: Linux x86_64, Linux aarch64, and macOS Apple Silicon.

## Repo Layout

- [aead](yacl/aead/), [aes](yacl/aes/), [bc](yacl/bc/), [hash](yacl/hash/),
  [hmac](yacl/hmac/): symmetric crypto, hashing, and authentication primitives.
- [pke](yacl/pke/), [sign](yacl/sign/), [envelope](yacl/envelope/),
  [tpre](yacl/tpre/): public-key, signature, envelope, and threshold proxy
  re-encryption code.
- [ot](yacl/ot/): base OT, OT extension, OT stores, and SimplestOT backends.
- [vole](yacl/vole/): base VOLE, MP-VOLE, MPFSS, and silent VOLE code.
- [mpc](yacl/mpc/): higher-level MPC kernels and plaintext/secret-sharing
  executors.
- [dpf](yacl/dpf/), [oprf](yacl/oprf/), [snark](yacl/snark/): additional
  protocol implementations.
- [io](yacl/io/): streaming I/O and Bristol Fashion circuit parsing.
- [link](yacl/link/): RPC-style SPMD communication utilities.
- [math](yacl/math/): big integers, finite fields, ECC, and pairing support.
- [rand](yacl/rand/): randomness, DRBG, entropy source, and OpenSSL provider
  code.
- [utils](yacl/utils/): common types, buffers, exceptions, serialization, and
  helper utilities.
- [experimental](yacl/experimental/): experimental PRG/RO/code utilities.
- [cmake](cmake/): CMake modules, dependency scripts, and dependency patches.

## Prerequisites

- GCC >= 11 or Clang >= 12 with C++20 support
- CMake >= 3.20
- Ninja or Make
- Perl 5 with core modules, required by OpenSSL
- `patch`, required for dependency patching
- GMP development files available on the system

Ubuntu example:

```sh
sudo apt install -y git cmake ninja-build gcc g++ patch libgmp-dev
```

## Build

Yacl-r uses CMake.

```sh
# Configure a Release build.
cmake -S . -B build -G Ninja

# Configure a Debug build.
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug

# Build.
cmake --build build -j$(nproc)
```

Build fuzz targets with Clang/libFuzzer:

```sh
cmake -S . -B build-fuzz -G Ninja \
  -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
  -DBUILD_FUZZ=On
cmake --build build-fuzz -j$(nproc)
```

## Test

Run tests from the build directory:

```sh
cd build
ctest --output-on-failure
ctest -R plaintext_executor_test --output-on-failure
./bin/buffer_test
```

## Formatting And Checks

No pre-commit or automated license-header check is configured in this repo.
Use the regular build, test, and focused formatting/linting commands instead:

```sh
clang-format -i path/to/file.cc
clang-tidy path/to/file.cc -- -p build
cmake --build build -j$(nproc)
ctest --output-on-failure
```

## Install

Yacl-r supports `cmake --install`.

```sh
cmake -S . -B build
cmake --build build -j$(nproc)
cmake --install build

# Or install into a custom prefix.
cmake --install build --prefix /path/to/prefix
```

Vendored third-party dependencies are installed into a private subtree inside
the install prefix instead of being copied into system-wide include/library
locations:

- public headers: `include/yacl-r/yacl/...`
- private vendored headers: `include/yacl-r/deps/...`
- private vendored libraries: `lib*/yacl-r/deps/...`

This keeps Yacl-r's bundled dependencies isolated from the system toolchain and
from unrelated projects. GMP is still resolved as a system dependency at
package-consume time.

Consumers can use the installed package via CMake:

```cmake
find_package(Yacl CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE Yacl::yacl)
```

The exported target adds `include/yacl-r` to the include path, so installed
consumers include public headers as `#include "yacl/..."`.

## License

See [LICENSE](LICENSE).
