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

# --------------------------------------------------------------------
# User options
# --------------------------------------------------------------------

# Toggle different modules
option(ENABLE_ENGINE "Enabel the engine module (On/Off)." Off)
option(ENABLE_KERNEL "Enable the kernel module (On/Off)." Off)
option(ENABLE_LINK "Enable the link (network) module (On/Off)." Off)
option(BUILD_TEST "Build the testing binaries (On/Off)." On)

# Check for options
if(ENABLE_ENGINE AND NOT ENABLE_KERNEL)
  message(
    WARNING
      "Engine enabled, but kernels disabled. Turn on kernel automatically.")
  set(ENABLE_KERNEL On)
endif()

if(ENABLE_ENGINE AND NOT ENABLE_LINK)
  message(
    WARNING "Engine enabled, but link disabled. Turn on link automatically.")
  set(ENABLE_LINK On)
endif()

if(ENABLE_KERNEL AND NOT ENABLE_LINK)
  message(
    WARNING "Kernel enabled, but link disabled. Turn on link automatically.")
  set(ENABLE_LINK On)
endif()

# --------------------------------------------------------------------
# Project Generic Configuration
# --------------------------------------------------------------------

# Configure build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      "Release"
      CACHE STRING "Build type" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Release" "Debug"
                                               "MinSizeRel" "RelWithDebInfo")
endif()

# Set module path
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Enable compile_commands.json for lsp
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# This policy was introduced in CMake 3.13; OLD by default until CMake 3.21
# cmake_policy(SET CMP0077 NEW)
# cmake_policy(SET CMP0135 NEW)
# cmake_policy(SET CMP0168 NEW)
# cmake_policy(SET CMP0169 OLD)

# Generic this project's CMake modules
include(AddCompilerFlags)
include(AddYaclTest)
include(ImportLib)

# Generic CMake modules
include(CheckCXXCompilerFlag)
include(CheckCXXSourceRuns)
include(CheckLanguage)
include(CMakeDependentOption)
include(CTest)
include(ExternalProject)
include(GNUInstallDirs)
include(FetchContent)

set(CMAKE_CTEST_ARGUMENTS "--output-on-failure")

# --------------------------------------------------------------------------
# Toolchain Configuration
# --------------------------------------------------------------------------

# CMake arguments
set(CMAKE_POSITION_INDEPENDENT_CODE On) # Enable position-independent-code
set(CMAKE_CXX_STANDARD 17) # Enable C++17
set(CMAKE_C_STANDARD_REQUIRED Yes) # Force C standard to be present

if(NOT BUILD_TEST)
  set(CMAKE_SKIP_RPATH TRUE) # Do not use RPATH for testing
endif()

# Additional compiler flags
add_compiler_flags(-Wall)
add_compiler_flags(-Wextra)
add_compiler_flags(-Werror)

# For platform_utils
add_compiler_flags(-mbmi2)

# For cryptos
add_compiler_flags(-maes)
add_compiler_flags(-mavx)
add_compiler_flags(-mpclmul)

# Security flags
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # fpic
add_compiler_flags(-fstack-protector-strong) # stack protection
add_compiler_flags(-D_FORTIFY_SOURCE=2 -O2)
add_linker_flags(-s) # strip

# HACK supress warnings
add_compiler_flags(-Wno-sign-compare)
add_compiler_flags(-Wno-unused-function)
add_compiler_flags(-Wno-vla) # no variable length error warning
add_compiler_flags(-Wno-shift-count-overflow)
add_compiler_flags(-Wno-deprecated-declarations) # for butil
add_compiler_flags(-Wno-unused-parameter) # for butil
# add_compiler_flags(-Wno-inconsistent-missing-override) # context_test
# add_compiler_flags(-Wno-stringop-overflow) # mcl
add_compiler_flags(-Wno-mismatched-new-delete) # msgpack
add_compiler_flags(-Wno-array-bounds) # mcl

# HACK we need to add the following global compiler flag in order to compile fourq
add_compile_definitions(__LINUX__)
add_compile_definitions(_ARM64_)

# make it colorful under ninja-build
if(CMAKE_GENERATOR STREQUAL Ninja)
  add_compiler_flags(-fdiagnostics-color=always)
endif()

# --------------------------------------------------------------------------
# External Dependencies Configuration
# --------------------------------------------------------------------------

# prefer -pthread instead of -lpthread or -lpthreads
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
find_package(GMP REQUIRED)

set(CMAKE_DEPS_PREFIX ${CMAKE_BINARY_DIR}/deps)
set(CMAKE_DEPS_LIBDIR ${CMAKE_DEPS_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_DEPS_BINDIR ${CMAKE_DEPS_PREFIX}/${CMAKE_INSTALL_BINDIR})
set(CMAKE_DEPS_INCLUDEDIR ${CMAKE_DEPS_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})
set(CMAKE_DEPS_SRCDIR ${CMAKE_DEPS_PREFIX}/src)

# Explicitly create build/deps directory
file(MAKE_DIRECTORY ${CMAKE_DEPS_PREFIX})
file(MAKE_DIRECTORY ${CMAKE_DEPS_LIBDIR})
file(MAKE_DIRECTORY ${CMAKE_DEPS_BINDIR})
file(MAKE_DIRECTORY ${CMAKE_DEPS_INCLUDEDIR})
file(MAKE_DIRECTORY ${CMAKE_DEPS_SRCDIR})

# HACK FetchContent should download to build/deps/src
set(FETCHCONTENT_BASE_DIR ${CMAKE_DEPS_PREFIX}/src)

if(ENABLE_LINK)
  # Protobuf must be placed before all dependencies
  include(deps/protobuf)
  # Interconnection should be built after protobuf
  include(deps/interconnection)
  include(deps/simplest_ot)
  include(deps/brpc)
endif()

include(deps/abseil)
include(deps/blake3)
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

# --------------------------------------------------------------------------
# Project Target Configuration
# --------------------------------------------------------------------------

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

message(STATUS "==============================================================")
message(STATUS "User Options and Configurations")
message(STATUS "==============================================================")
message(STATUS "CMake Version             : ${CMAKE_VERSION}")
message(STATUS "Build Type                : ${CMAKE_BUILD_TYPE}")
message(STATUS "CPU Type                  : ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Compiler                  : ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Compiler Version          : ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ Standard              : ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler Flags            : \n${CMAKE_CXX_FLAGS}")
message(STATUS "Linker Flags              : \n${LINK_OPTIONS}")
message(STATUS "(opt) Engine Module       : ${ENABLE_ENGINE}")
message(STATUS "(opt) Kernel Module       : ${ENABLE_KERNEL}")
message(STATUS "(opt) Link Module         : ${ENABLE_LINK}")
message(STATUS "(opt) Build Test          : ${BUILD_TEST}")
message(STATUS "==============================================================")

enable_testing()

#
# Iteracte all files
#

set(YACL_PROTO_SOURCE_FILES "")
set(YACL_SOURCE_FILES "")
set(YACL_SPI_LIBS "")

add_subdirectory(yacl/base)
add_subdirectory(yacl/crypto)
add_subdirectory(yacl/math)
add_subdirectory(yacl/utils)
add_subdirectory(yacl/io)

if(ENABLE_ENGINE)
  add_subdirectory(yacl/engine)
endif()

if(ENABLE_KERNEL)
  add_subdirectory(yacl/kernel)
endif()

if(ENABLE_LINK)
  add_subdirectory(yacl/link)
endif()

# TODO Remove all test files from ${YACL_SOURCE_FILES}
file(GLOB_RECURSE SRC_TEST_CC *_test.cc)
list(REMOVE_ITEM YACL_SOURCE_FILES ${SRC_TEST_CC})

#
# Object Library: yacl_obj
#

add_library(yacl_obj OBJECT ${YACL_SOURCE_FILES}) # main object lib

target_include_directories(
  yacl_obj
  PUBLIC # these dirs are only used when building inside the build tree
         "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
         "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
         "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
         # these dirs are only used when linking against a prebuilt package
         "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
         ${CMAKE_DEPS_INCLUDEDIR})

target_link_libraries(
  yacl_obj
  PUBLIC Deps::absl
         Deps::blake3
         Deps::cpu_features
         Deps::fmt
         Deps::fourq
         Deps::leveldb
         Deps::libsodium
         Deps::libtommath
         Deps::mcl
         Deps::openssl
         Deps::gflags
         Deps::spdlog
         Deps::sse2neon
         Deps::gmp
         Deps::msgpack)

#
# Object Library: yacl_proto
#

if(ENABLE_LINK)
  # build yacl_proto target
  add_library(yacl_proto OBJECT) # object lib for protobuf
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
endif()

#
# Static Library: yacl
#

add_library(yacl SHARED) # main shared lib

target_include_directories(
  yacl PUBLIC ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/include
              ${CMAKE_DEPS_INCLUDEDIR})

target_link_libraries(yacl PUBLIC yacl_obj)

# NOTE it's IMPORTANT to link all spi libs in PUBLIC scope.
# All spi libs must be included in binaries.
foreach(spilib IN LISTS YACL_SPI_LIBS)
  target_link_libraries(yacl PRIVATE $<TARGET_OBJECTS:${spilib}>)
endforeach()

# target_link_libraries(yacl PUBLIC yacl_proto)
# target_link_libraries(yacl PUBLIC Deps::msgpack)
# target_link_libraries(yacl PUBLIC Deps::brpc)
# target_link_libraries(yacl PUBLIC Deps::simplest_ot)
# target_link_libraries(yacl PUBLIC Deps::interconnection)

add_library(Yacl::yacl ALIAS yacl)

#
# Install headers
#

install(
  TARGETS yacl
  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
