# Yet Another Cryptographic Library for Research


[![CircleCI](https://dl.circleci.com/status-badge/img/gh/Jamie-Cui/yacl-r/tree/main.svg?style=svg)](https://dl.circleci.com/status-badge/redirect/gh/Jamie-Cui/yacl-r/tree/main)


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

- **gcc >= 10.3**
- **[ninja/ninja-build](https://ninja-build.org/)**
- **Perl 5 with core modules** (Required by [OpenSSL](https://github.com/openssl/openssl/blob/master/INSTALL.md#prerequisites))
- **gflags** (Required by brpc)
- **protobuf**

## Getting Started

Yacl-r tries to support both [cmake](https://cmake.org/) and [bazel](https://bazel.build/) build system. For more guidelines about **how to develop on yacl**, please check the [Getting Started Guide](GETTING_STARTED.md).

**Ubuntu with CMake**

``` sh
# install dependencies
sudo apt install -y git cmake gcc-11 g++-11

# optional, make gcc-11 g++-11 system default
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 10
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 10

# download yacl-r
git clone https://github.com/Jamie-Cui/yacl-r.git

# enter the project
cd yacl-r

# configure 
cmake -S . -B build

# build (this may take a while)
cmake --build build
```

## License

See [LICENSE](LICENSE) and [NOTICE.md](NOTICE.md)
