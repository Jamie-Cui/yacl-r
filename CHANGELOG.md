# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- Add CONTRIBUTING.md with contribution guidelines
- Add SECURITY.md with security policy
- Add GETTING_STARTED.md with detailed development guide
- Add CI workflow for building and testing on multiple platforms
- Add coverage reporting configuration

### Changed

- CI linter now fails on errors instead of continuing
- Make platform-specific compiler flags conditional (x86_64 vs ARM64)
- Remove duplicate header files from include/ directory

## [0.4.5] - 2024-05-15

### Added

- Re-organize repo layout, add kernel folder for crypto protocols with links
- Add ECC lib25519 implementation
- Add ECC FourQ implementation
- Unify f2k implementation

### Fixed

- Fix RandBits implementation

## [0.4.3] - 2024-02-01

### Added

- Add Silent Vole (malicious version)

### Fixed

- Multiple bugfixes

## [0.4.2] - 2024-01-09

### Changed

- Bump OpenSSL to 3.0.12 (experimental)

### Added

- Add Softspoken OTe (malicious version)

### Changed

- Refactor entropy source, drbg, and rand
- Refine traditional crypto APIs

### Fixed

- Multiple bugfixes

## [0.4.1.1] - 2023-11-16

### Added

- Init Global Security Parameters for Yacl
- Add Softspoken OTe (semi-honest version)
- Add Silent Vole (WIP: optimize MpVole and DualEncode)

## [0.4.1] - 2023-10-20

### Added

- Add Sigma-type ZKP Protocols (An unified implementation)
- Add ECC Pairing SPI and support to libmcl(ecc, pairing)
- Add Multiplication for GF(2^64) and GF(2^128)
- Add AVX2 Matrix Transpose

### Fixed

- Fix KOS OTe security flaws

## [0.3.3] - 2023-05-25

### Added

- Add Ferret OTe
- Add Gywz OTe (Correlated GGM Tree)
- Add KOS OTe (warning: KOS still has potential security flaws)

## [0.3.1] - 2023-02-02

### Added

- Add `dynamic_bitset` for manipulating bit vectors

### Changed

- RO now can accept multiple inputs
- Improve iknp performance
- Add iknp cot api

### Fixed

- Fix Several m1 related bugs

## [0.3.0] - 2022-12-08

### Added

- Add random permutation and correlation-robust hash function
- Add OT/OTe benchmark

### Changed

- Fix randomness implementation
- Re-organize repo layout

### Fixed

- Fix Random Oracle Usage

## [0.2.0] - 2022-12-01

### Changed

- Rename YASL to YACL
- Re-organize repo layout

[Unreleased]: https://github.com/Jamie-Cui/yacl-r/compare/v0.4.5...HEAD
[0.4.5]: https://github.com/Jamie-Cui/yacl-r/compare/v0.4.3...v0.4.5
[0.4.3]: https://github.com/Jamie-Cui/yacl-r/compare/v0.4.2...v0.4.3
[0.4.2]: https://github.com/Jamie-Cui/yacl-r/compare/v0.4.1.1...v0.4.2
[0.4.1.1]: https://github.com/Jamie-Cui/yacl-r/compare/v0.4.1...v0.4.1.1
[0.4.1]: https://github.com/Jamie-Cui/yacl-r/compare/v0.3.3...v0.4.1
[0.3.3]: https://github.com/Jamie-Cui/yacl-r/compare/v0.3.1...v0.3.3
[0.3.1]: https://github.com/Jamie-Cui/yacl-r/compare/v0.3.0...v0.3.1
[0.3.0]: https://github.com/Jamie-Cui/yacl-r/compare/v0.2.0...v0.3.0
[0.2.0]: https://github.com/Jamie-Cui/yacl-r/releases/tag/v0.2.0