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

- **gcc >= 10.3**
- **[ninja/ninja-build](https://ninja-build.org/)**
- **Perl 5 with core modules** (Required by [OpenSSL](https://github.com/openssl/openssl/blob/master/INSTALL.md#prerequisites))
- TODO **others**

## Getting Started

Yacl-r tries to support both [cmake](https://cmake.org/) and [bazel](https://bazel.build/) build system. For more guidelines about **how to develop on yacl**, please check the [Getting Started Guide](GETTING_STARTED.md).

**TL; DR**

``` sh
# for cmake
mkdir -p build
cd build
cmake ..
make -j8

# for bazel
bazel build //...
```

## License

See [LICENSE](LICENSE) and [NOTICE.md](NOTICE.md)
