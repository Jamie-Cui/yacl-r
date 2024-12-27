# YACL-R (Yet Another Cryptographic Library for Research)

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

## Repo Layout

- [base](yacl/base/): some basic types and utils in yacl.
- [crypto](yacl/crypto/): **crypto algorithms** without [link](yacl/link/).
- [kernel](yacl/kernel/): **crypto kernel** that includes [link](yacl/link/) with (WIP) multi-thread support, i.e. OT, DPF.
- [io](yacl/io/): a simple streaming-based io library.
- [link](yacl/link/): a simple rpc-based MPI framework, providing the [SPMD](https://en.wikipedia.org/wiki/SPMD) parallel programming capability.

## Prerequisites

- **bazel**: [.bazelversion](.bazelversion) file describes the recommended version of bazel. We recommend to use the official [bazelisk](https://github.com/bazelbuild/bazelisk?tab=readme-ov-file#installation) to manage bazel version.
- **gcc >= 10.3**
- **[ninja/ninja-build](https://ninja-build.org/)**
- **Perl 5 with core modules** (Required by [OpenSSL](https://github.com/openssl/openssl/blob/master/INSTALL.md#prerequisites))

## Getting Started

Yacl uses the [bazel](https://bazel.build/) build system, you may use the following codes to build and test yacl modules. For more guidelines about **how to develop on yacl**, please check the [Getting Started Guide](GETTING_STARTED.md).

## License

See [LICENSE](LICENSE) and [NOTICE.md](NOTICE.md)
