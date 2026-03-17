# Yet Another Cryptographic Library for Research

```
██╗   ██╗ █████╗  ██████╗██╗           ██████╗ 
╚██╗ ██╔╝██╔══██╗██╔════╝██║           ██╔══██╗
 ╚████╔╝ ███████║██║     ██║     █████╗██████╔╝
  ╚██╔╝  ██╔══██║██║     ██║     ╚════╝██╔══██╗
   ██║   ██║  ██║╚██████╗███████╗      ██║  ██║
   ╚═╝   ╚═╝  ╚═╝ ╚═════╝╚══════╝      ╚═╝  ╚═╝
```

Yacl-r is a fork and extension of the C++ crypto library [secretflow/yacl](https://github.com/secretflow/yacl). The crypto modules in Yacl implement many state-of-art secure computation protocols, including primitives like OT, VOLE, TPRE, and tools like PRG, RO. Check the full list of Yacl's supported algorithms in [ALGORITHMS.md](ALGORITHMS.md).

> [!WARNING]
> Yacl-r is under heavy development, please use at your own risk

Target Platforms (hopefully): MacOS Apple Silicon, Linux x86_64 and Linux aarch64.

## Repo Layout

- [base](yacl/base/): some basic types and utils in yacl.
- [crypto](yacl/crypto/): **crypto algorithms** without [link](yacl/link/).
- [engine](yacl/engine/): **interactive engines** that is desgined for a purpose.
- [io](yacl/io/): a simple streaming-based io library.
- [kernel](yacl/kernel/): **crypto kernel** that includes [link](yacl/link/) with (WIP) multi-thread support, i.e. OT, DPF.
- [link](yacl/link/): a simple rpc-based MPI framework, providing the [SPMD](https://en.wikipedia.org/wiki/SPMD) parallel programming capability.
- [math](yacl/math/): a simplified math lib (or interface), supporting big integer.
- [utils](yacl/utils/): other good-to-have utilities

## Prerequisites

- **gcc >= 11** or **clang >= 12** (C++20 support required)
- **cmake >= 3.20**
- **[ninja/ninja-build](https://ninja-build.org/)**
- **Perl 5 with core modules** (Required by [OpenSSL](https://github.com/openssl/openssl/blob/master/INSTALL.md#prerequisites))
- **patch** (required for dependency patching)

## Getting Started

Yacl-r uses [cmake](https://cmake.org/) as its build system.

**Ubuntu with CMake**

``` sh
# install dependencies
sudo apt install -y git cmake ninja-build gcc-11 g++-11 patch

# optional, make gcc-11 g++-11 system default
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 10
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 10

# download yacl-r
git clone https://github.com/Jamie-Cui/yacl-r.git

# enter the project
cd yacl-r

# configure (Ninja recommended)
cmake -S . -B build -G Ninja

# build (this may take a while)
cmake --build build -j$(nproc)
```

## Installing

Yacl-r now supports `make install` / `cmake --install`.

```sh
# configure
cmake -S . -B build

# build and install into the default prefix: ./output
cmake --build build -j$(nproc)
cmake --install build

# or install into a custom prefix
cmake --install build --prefix /path/to/prefix
```

Vendored third-party dependencies are installed into a private subtree inside
the install prefix instead of being copied into system-wide include/library
locations:

- public headers: `include/yacl-r/yacl/...`
- private vendored headers: `include/yacl-r/deps/...`
- private vendored libraries: `lib*/yacl-r/deps/...`

This keeps Yacl-r's bundled dependencies isolated from the system toolchain and
from unrelated projects. The current exception is GMP, which is still resolved
as a system dependency at package-consume time.

Consumers can use the installed package via CMake:

```cmake
find_package(Yacl CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE Yacl::yacl)
```

The exported target adds `include/yacl-r` to the include path, so installed
consumers still include public headers as `#include "yacl/..."`.

## License

See [LICENSE](LICENSE) 
