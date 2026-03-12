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
  LANGUAGES CXX C ASM)

find_program(PATCH_EXECUTABLE patch)
if(NOT PATCH_EXECUTABLE)
  message(
    FATAL_ERROR
      "patch is required to build yacl-r. Please install patch and try again.")
endif()

# --------------------------------------------------------------------
# User options
# --------------------------------------------------------------------

option(BUILD_TEST "Build the testing binaries (On/Off)." On)

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

# Configure default install path
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set_property(CACHE CMAKE_INSTALL_PREFIX PROPERTY VALUE
                                                   ${PROJECT_SOURCE_DIR}/output)
endif()

# Set module path
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Enable compile_commands.json for lsp
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Generic this project's CMake modules
include(AddCompilerFlags)
include(AddYaclFuzz)
include(AddYaclTest)
include(Coverage)
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
set(CMAKE_CXX_STANDARD 20) # Enable C++20
set(CMAKE_C_STANDARD_REQUIRED Yes) # Force C standard to be present

if(NOT BUILD_TEST)
  set(CMAKE_SKIP_RPATH TRUE) # Do not use RPATH for testing
endif()

# Additional compiler flags
add_compiler_flags(-Wall)
add_compiler_flags(-Wextra)
add_compiler_flags(-Werror)

# Platform-specific CPU feature flags
# These flags are x86_64 specific and may not be available on ARM/other platforms
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|amd64")
  # For platform_utils
  add_compiler_flags(-mbmi2)

  # For cryptos
  add_compiler_flags(-maes)
  add_compiler_flags(-mavx)
  add_compiler_flags(-mpclmul)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|ARM64")
  # ARM64 has different instruction sets - BMI2/AES/AVX/PCLMUL are x86-specific
  # ARM64 has native AES instructions via ARMv8-A crypto extensions
  # No additional flags needed as they are handled by compiler intrinsics
  message(STATUS "ARM64 platform detected - using ARM-specific optimizations")
else()
  message(
    WARNING
      "Unknown platform ${CMAKE_SYSTEM_PROCESSOR} - some optimizations may not be available"
  )
endif()

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
add_compiler_flags(-Wno-stringop-overflow) # mcl
add_compiler_flags(-Wno-mismatched-new-delete) # msgpack
add_compiler_flags(-Wno-array-bounds) # mcl

# HACK we need to add the following global compiler flag in order to compile fourq
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  add_compile_definitions(__LINUX__)
endif()
if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|ARM64")
  add_compile_definitions(_ARM64_)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
  add_compile_definitions(_AMD64_)
endif()

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

# ASIO is always required for link module
include(deps/asio)

include(deps/blake3)
include(deps/cpu_features)
include(deps/fmt)
include(deps/fourq)
include(deps/gtest)
include(deps/gflags)
include(deps/leveldb)
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
message(STATUS "(opt) Build Test          : ${BUILD_TEST}")
message(STATUS "(opt) Build Fuzz          : ${BUILD_FUZZ}")
message(STATUS "Install Prefix            : ${CMAKE_INSTALL_PREFIX}")
message(STATUS "==============================================================")

enable_testing()

#
# Iteracte all files
#

set(YACL_SOURCE_FILES "")
set(YACL_SPI_LIBS "")

add_subdirectory(yacl/base)
add_subdirectory(yacl/crypto)
add_subdirectory(yacl/math)
add_subdirectory(yacl/utils)
add_subdirectory(yacl/io)
add_subdirectory(yacl/engine)
add_subdirectory(yacl/kernel)
add_subdirectory(yacl/link)

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

# Add include path for simplest-ot
target_include_directories(yacl_obj PRIVATE 
    ${PROJECT_SOURCE_DIR}/yacl/kernel/simplest-ot)

target_link_libraries(
  yacl_obj
  PUBLIC Deps::blake3
         Deps::cpu_features
         Deps::fmt
         Deps::fourq
         Deps::leveldb
         Deps::libtommath
         Deps::mcl
         Deps::openssl
         Deps::gflags
         Deps::spdlog
         Deps::sse2neon
         Deps::gmp
         Deps::msgpack)

# ASIO is always needed for link module
target_link_libraries(yacl_obj PUBLIC Deps::asio)

#
# Static Library: yacl
#

option(BUILD_YACL_SHARED "Build yacl as a shared library" Off)

if(BUILD_YACL_SHARED)
  add_library(yacl SHARED) # main shared lib
else()
  add_library(yacl STATIC) # main static lib
endif()

target_include_directories(
  yacl PUBLIC ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/include
              ${CMAKE_DEPS_INCLUDEDIR})

target_link_libraries(yacl PUBLIC yacl_obj)

# NOTE it's IMPORTANT to link all spi libs in PUBLIC scope.
# All spi libs must be included in binaries.
foreach(spilib IN LISTS YACL_SPI_LIBS)
  target_link_libraries(yacl PRIVATE $<TARGET_OBJECTS:${spilib}>)
endforeach()

add_library(Yacl::yacl ALIAS yacl)

#
# Install headers
#

install(
  TARGETS yacl
  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

install(DIRECTORY ${CMAKE_DEPS_INCLUDEDIR} DESTINATION ${CMAKE_INSTALL_PREFIX})
