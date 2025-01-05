# Copyright 2025 Jamie Cui
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Code from
# https://github.com/llvm/llvm-project/blob/main/utils/bazel/third_party_build/gmp.BUILD

# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

load("@rules_foreign_cc//foreign_cc:defs.bzl", "configure_make_variant")

filegroup(
    name = "sources",
    srcs = glob(["**"]),
)

configure_make_variant(
    name = "gmp",
    configure_options = ["--with-pic"],
    copts = ["-w"],
    lib_name = "libgmp",
    lib_source = ":sources",
    out_static_libs = [
        "libgmp.a",
    ],
    toolchain = "@rules_foreign_cc//toolchains:preinstalled_autoconf_toolchain",
    visibility = ["//visibility:public"],
)
