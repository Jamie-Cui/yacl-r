# Copyright 2024 Jamie Cui
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

ExternalProject_Add(
  brpc
  URL https://github.com/apache/brpc/archive/refs/tags/1.10.0.tar.gz
  URL_HASH
    SHA256=fe4eb10b4ca1a59e0f71086552b2d8897afd66df93b53c18ad83f6a93717cc2d
  CMAKE_ARGS -DBUILD_BRPC_TOOLS=Off -DWITH_DEBUG_SYMBOLS=Off
             -DDOWNLOAD_GTEST=Off
             -DCMAKE_INSTALL_PREFIX=${CMAKE_THIRDPARTY_PREFIX}
  PREFIX ${CMAKE_THIRDPARTY_PREFIX}
  EXCLUDE_FROM_ALL true
  LOG_DOWNLOAD On
  LOG_CONFIGURE On
  # LOG_BUILD On
  LOG_INSTALL On)

add_dependencies(brpc Thirdparty::protobuf)
add_dependencies(brpc Thirdparty::gflags)
add_dependencies(brpc Thirdparty::leveldb)

add_library(libbrpc STATIC IMPORTED)
set_property(
  TARGET libbrpc
  PROPERTY IMPORTED_LOCATION
           ${CMAKE_THIRDPARTY_LIBDIR}/libbrpc${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(libbrpc brpc)

add_library(libbrpc_interface INTERFACE)

target_link_libraries(
  libbrpc_interface INTERFACE libbrpc Thirdparty::gflags Thirdparty::protobuf
                              Thirdparty::leveldb Thirdparty::openssl)

# HACK for macos see:
# https://github.com/apache/brpc/blob/c93d0e06f50182bf41d973cad6f8714f4b1d021e/BUILD.bazel#L59
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  target_link_libraries(
    libbrpc_interface
    INTERFACE "-framework CoreFoundation"
              "-framework CoreGraphics"
              "-framework CoreData"
              "-framework CoreText"
              "-framework Security"
              "-framework Foundation"
              "-Wl,-U,__Z13GetStackTracePPvii"
              "-Wl,-U,_ProfilerStart"
              "-Wl,-U,_ProfilerStop"
              "-Wl,-U,__Z13GetStackTracePPvii"
              "-Wl,-U,_RegisterThriftProtocol"
              "-Wl,-U,_mallctl"
              "-Wl,-U,_malloc_stats_print")
endif()

# -----------------------------
# Alias Target for External Use
# -----------------------------

add_library(Thirdparty::brpc ALIAS libbrpc_interface)
