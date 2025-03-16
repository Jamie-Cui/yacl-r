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
  protobuf
  URL https://github.com/protocolbuffers/protobuf/releases/download/v21.12/protobuf-all-21.12.tar.gz
  URL_HASH
    SHA256=2c6a36c7b5a55accae063667ef3c55f2642e67476d96d355ff0acb13dbb47f09
  CMAKE_ARGS -Dprotobuf_BUILD_PROTOBUF_BINARIES=Off
             -Dprotobuf_BUILD_PROTOC_BINARIES=On
             -Dprotobuf_BUILD_LIBPROTOC=On
             -Dprotobuf_BUILD_TESTS=Off
             -DBUILD_SHARED_LIBS=Off
             -DCMAKE_INSTALL_PREFIX=${CMAKE_DEPS_PREFIX}
  PREFIX ${CMAKE_DEPS_PREFIX}
  EXCLUDE_FROM_ALL true
  LOG_DOWNLOAD On
  LOG_CONFIGURE On
  LOG_BUILD On
  LOG_INSTALL On)

# ------------------------------------------------------------------------------
# How to use protobuf?
# ------------------------------------------------------------------------------

add_library(libprotobuf STATIC IMPORTED)
set_property(
  TARGET libprotobuf
  PROPERTY IMPORTED_LOCATION
           ${CMAKE_DEPS_LIBDIR}/libprotobuf${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(libprotobuf protobuf)

# add_library(libprotobuf STATIC IMPORTED) set_property( TARGET libprotobuf
# PROPERTY IMPORTED_LOCATION
# ${CMAKE_DEPS_LIBDIR}/libprotobuf${CMAKE_STATIC_LIBRARY_SUFFIX})
# add_dependencies(libprotobuf protobuf)

# -----------------------------
# Alias Target for External Use
# -----------------------------
add_library(Deps::protobuf ALIAS libprotobuf)
