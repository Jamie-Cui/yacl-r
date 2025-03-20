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

project(
  yacl_r
  VERSION 0.4.5
  DESCRIPTION "YACL-r (Yet Another Crypto Library for Research)"
  HOMEPAGE_URL "https://github.com/Jamie-Cui/yacl-r"
  LANGUAGES CXX C)

# option(BUILD_TESTING "Build the testing binaries (On/Off)." On)

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
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Enable compile_commands.json for lsp
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# This policy was introduced in CMake 3.13; OLD by default until CMake 3.21
cmake_policy(SET CMP0077 NEW)
cmake_policy(SET CMP0135 NEW)
cmake_policy(SET CMP0168 NEW)
cmake_policy(SET CMP0169 OLD)

# Generic this project's CMake modules
include(AddCompilerFlags)
include(AddYaclTest)

# Generic CMake modules
include(CheckCXXCompilerFlag)
include(CheckCXXSourceRuns)
include(CheckLanguage)
include(CMakeDependentOption)
include(CTest)
include(ExternalProject)
include(GNUInstallDirs)
include(FetchContent)

# ------------------------------------------------------------------------------
# Toolchain Configuration
# ------------------------------------------------------------------------------

# CMake arguments
set(CMAKE_POSITION_INDEPENDENT_CODE On) # Enable position-independent-code
set(CMAKE_CXX_STANDARD 17) # Enable C++17
set(CMAKE_C_STANDARD_REQUIRED Yes) # Force C standard to be present

# Additional compiler flags
add_compiler_flags("-Wall")
add_compiler_flags("-Wextra")
add_compiler_flags("-Werror")

# FIXME try removing the following flags
add_compiler_flags("-Wno-sign-compare")
add_compiler_flags("-Wno-unused-function")
add_compiler_flags("-Wno-vla") # no variable length error warning
add_compiler_flags("-Wno-shift-count-overflow")
add_compiler_flags("-Wno-deprecated-declarations") # for butil
add_compiler_flags("-Wno-unused-parameter") # for butil
add_compiler_flags("-Wno-inconsistent-missing-override") # context_test

# HACK we need to add the following global compiler flag in order to use fourq
add_compiler_flags("-D __LINUX__")
add_compiler_flags("-D _ARM64_")

# For cryptos
add_compiler_flags("-maes")
add_compiler_flags("-mavx")
add_compiler_flags("-mpclmul")

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

set(CMAKE_DEPS_PREFIX ${CMAKE_BINARY_DIR}/deps)
set(CMAKE_DEPS_LIBDIR ${CMAKE_DEPS_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_DEPS_BINDIR ${CMAKE_DEPS_PREFIX}/${CMAKE_INSTALL_BINDIR})
set(CMAKE_DEPS_INCLUDEDIR ${CMAKE_DEPS_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})
set(CMAKE_DEPS_SRCDIR ${CMAKE_DEPS_PREFIX}/src)

# HACK FetchContent should download to build/deps/src
set(FETCHCONTENT_BASE_DIR ${CMAKE_DEPS_PREFIX}/src)

# Protobuf must be placed before all dependencies
include(deps/protobuf)

# Interconnection should be built after protobuf
include(deps/interconnection)

include(deps/abseil)
include(deps/blake3)
include(deps/brpc)
include(deps/cpu_features)
include(deps/fmt)
include(deps/fourq)
include(deps/gtest)
include(deps/gflags)
include(deps/leveldb)
include(deps/libsodium)
include(deps/libtommath)
include(deps/mcl)
include(deps/msgpack)
include(deps/openssl)
include(deps/spdlog)
include(deps/sse2neon)
include(deps/simplest_ot)

# ------------------------------------------------------------------------------
# Project Target Configuration
# ------------------------------------------------------------------------------

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

enable_testing()

add_library(yacl_proto OBJECT) # object lib for protobuf
add_library(yacl OBJECT) # main static lib
add_library(yacl_all STATIC) # main static lib

set(YACL_SOURCE_FILES "")
set(YACL_PROTO_SOURCE_FILES "")

add_subdirectory(yacl/base)
add_subdirectory(yacl/crypto)
add_subdirectory(yacl/engine)
add_subdirectory(yacl/io)
add_subdirectory(yacl/kernel)
add_subdirectory(yacl/link)
add_subdirectory(yacl/math)
add_subdirectory(yacl/utils)

target_sources(yacl PRIVATE ${YACL_SOURCE_FILES})
target_sources(yacl_proto PRIVATE ${YACL_PROTO_SOURCE_FILES})

protobuf_generate(
  LANGUAGE
  cpp
  TARGET
  yacl_proto
  IMPORT_DIRS
  ${CMAKE_CURRENT_LIST_DIR}
  PROTOC_OUT_DIR
  ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(yacl_proto PRIVATE protobuf::libprotobuf)
target_include_directories(yacl_proto PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_include_directories(
  yacl
  PUBLIC # these dirs are only used when building inside the build tree
         "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
         "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
         # these dirs are only used when linking against a prebuilt package
         "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
         ${CMAKE_DEPS_INCLUDEDIR})

target_link_libraries(
  yacl
  PUBLIC Deps::absl
         Deps::blake3
         Deps::cpu_features
         Deps::fmt
         Deps::fourq
         Deps::leveldb
         Deps::libsodium
         Deps::libtommath
         Deps::mcl
         Deps::msgpack
         Deps::openssl
         Deps::spdlog
         Deps::sse2neon
         Deps::brpc
         Deps::simplest_ot
         Deps::gmp)

install(
  TARGETS yacl_all
  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

target_include_directories(yacl_all PUBLIC ${PROJECT_SOURCE_DIR}
                                           ${CMAKE_DEPS_INCLUDEDIR})
target_link_libraries(yacl_all PUBLIC yacl yacl_proto Deps::interconnection)

add_library(Yacl::yacl ALIAS yacl_all)
