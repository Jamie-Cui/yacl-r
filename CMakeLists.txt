# Copyright 2024 Jamie Cui
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

cmake_minimum_required(VERSION 3.20)

project(yacl_r
  VERSION 0.4.5
  DESCRIPTION "YACL-r (Yet Another Crypto Library for Research)"
  HOMEPAGE_URL "https://github.com/Jamie-Cui/yacl-r"
  LANGUAGES CXX C)

# ------------------------------------------------------------------------------
# Project Generic Configuration
# ------------------------------------------------------------------------------

# Configure build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      "Release"
      CACHE STRING "Build type" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Release" "Debug"
                                               "MinSizeRel" "RelWithDebInfo")
endif()
message(STATUS "Build type (CMAKE_BUILD_TYPE): ${CMAKE_BUILD_TYPE}")

# Set module path
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

# Enable compile_commands.json for lsp
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# This policy was introduced in CMake 3.13; OLD by default until CMake 3.21
cmake_policy(SET CMP0077 NEW)
cmake_policy(SET CMP0135 NEW)

# Generic this project's CMake modules
include(AddCompilerFlags)
include(AddYaclTest)

# Generic CMake modules
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)
include(CheckCXXSourceRuns)
include(CheckLanguage)
include(ExternalProject)
include(GNUInstallDirs)

# ------------------------------------------------------------------------------
# Toolchain Configuration
# ------------------------------------------------------------------------------

# CMake arguments
set(CMAKE_POSITION_INDEPENDENT_CODE On) # Enable position-independent-code
set(CMAKE_CXX_STANDARD 17)              # Enable C++17
set(CMAKE_C_STANDARD_REQUIRED Yes)      # Force C standard to be present

# HACK CMAKE_ARGS should only be used in external project to reuse main
# project's cmake args
set(CMAKE_ARGS
  -DCMAKE_POSITION_INDEPENDENT_CODE=On
  -DCMAKE_CXX_STANDARD=17
  -DCMAKE_C_STANDARD_REQUIRED=Yes)

# Additional compiler flags
add_compiler_flags("-Wall")
add_compiler_flags("-Wextra")
add_compiler_flags("-Werror")

add_compiler_flags("-Wno-sign-compare")

# FIXME remove this flag
add_compiler_flags("-Wno-unused-function")
add_compiler_flags("-Wno-vla") # no variable length error warning

# For cryptos
add_compiler_flags("-maes")
add_compiler_flags("-mavx")
add_compiler_flags("-mpclmul")

add_compiler_flags("-Wno-shift-count-overflow")


# make it colorful under ninja-build
if(CMAKE_GENERATOR STREQUAL Ninja)
  add_compiler_flags(-fdiagnostics-color=always)
endif()

# ------------------------------------------------------------------------------
# External Dependencies Configuration
# ------------------------------------------------------------------------------

# prefer -pthread instead of -lpthread or -lpthreads
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
find_package(GMP REQUIRED)

set(CMAKE_THIRDPARTY_PREFIX
  ${CMAKE_BINARY_DIR}/thirdparty)
set(CMAKE_THIRDPARTY_LIBDIR
  ${CMAKE_THIRDPARTY_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_THIRDPARTY_BINDIR
  ${CMAKE_THIRDPARTY_PREFIX}/${CMAKE_INSTALL_BINDIR})
set(CMAKE_THIRDPARTY_INCLUDEDIR
  ${CMAKE_THIRDPARTY_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})

include(deps/abseil)
include(deps/blake3)
include(deps/cpu_features)
include(deps/fmt)
include(deps/fourq)
include(deps/gtest)
include(deps/gflags)
include(deps/libsodium)
include(deps/libtommath)
include(deps/mcl)
include(deps/msgpack)
include(deps/openssl)
include(deps/spdlog)
include(deps/sse2neon)

# ------------------------------------------------------------------------------
# Project Target Configuration
# ------------------------------------------------------------------------------

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

enable_testing()

add_library(yacl STATIC)

target_include_directories(yacl PUBLIC
    # these dirs are only used when building inside the build tree
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
    # these dirs are only used when linking against a prebuilt package
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_include_directories(yacl PUBLIC ${CMAKE_THIRDPARTY_INCLUDEDIR})

set(YACL_SOURCE_FILES "")

add_subdirectory(yacl/base)
add_subdirectory(yacl/crypto)
# add_subdirectory(yacl/engine)
add_subdirectory(yacl/io)
# add_subdirectory(yacl/kernel)
add_subdirectory(yacl/math)
add_subdirectory(yacl/utils)

target_sources(yacl PRIVATE ${YACL_SOURCE_FILES})

target_link_libraries(yacl PUBLIC
  Thirdparty::absl
  Thirdparty::blake3
  Thirdparty::cpu_features
  Thirdparty::fmt
  Thirdparty::fourq
  Thirdparty::gflags
  Thirdparty::libsodium
  Thirdparty::libtommath
  Thirdparty::mcl
  Thirdparty::msgpack
  Thirdparty::openssl
  Thirdparty::spdlog
  Thirdparty::sse2neon)
