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
  leveldb
  URL https://github.com/google/leveldb/archive/refs/tags/1.23.tar.gz
  URL_HASH
    SHA256=9a37f8a6174f09bd622bc723b55881dc541cd50747cbd08831c2a82d620f6d76
  CMAKE_ARGS -DLEVELDB_BUILD_TESTS=Off -DLEVELDB_BUILD_BENCHMARKS=Off
             -DLEVELDB_INSTALL=On -DCMAKE_INSTALL_PREFIX=${CMAKE_DEPS_PREFIX}
  PREFIX ${CMAKE_DEPS_PREFIX}
  EXCLUDE_FROM_ALL true
  DOWNLOAD_EXTRACT_TIMESTAMP On
  LOG_DOWNLOAD On
  LOG_CONFIGURE On
  LOG_BUILD On
  LOG_INSTALL On)

# ------------------------------------------------------------------------------
# How to use leveldb?
# ------------------------------------------------------------------------------

add_library(libleveldb STATIC IMPORTED)
set_target_properties(
  libleveldb
  PROPERTIES IMPORTED_LOCATION
             ${CMAKE_DEPS_LIBDIR}/libleveldb${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(libleveldb leveldb)

# -----------------------------
# Alias Target for External Use
# -----------------------------
add_library(Deps::leveldb ALIAS libleveldb)
