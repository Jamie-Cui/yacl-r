# C++ Coding Style

Yacl-r follows the [Envoy C++ style guidelines](https://github.com/envoyproxy/envoy/blob/main/STYLE.md)
with C++20 enabled. Exceptions are allowed and commonly used through
`YACL_ENFORCE`, `YACL_THROW`, and `yacl::Exception`.

Use the local module style when it is more specific than the general guideline.
Format focused changes with `clang-format -i path/to/file.cc`; no pre-commit
hook is configured.
