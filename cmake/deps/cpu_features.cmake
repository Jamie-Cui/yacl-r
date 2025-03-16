# Copyright 2024 Ant Group Co., Ltd.
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
  cpu_features
  URL https://github.com/google/cpu_features/archive/refs/tags/v0.9.0.tar.gz
  URL_HASH
    SHA256=bdb3484de8297c49b59955c3b22dba834401bc2df984ef5cfc17acbe69c5018e
  CMAKE_ARGS -DCMAKE_POSITION_INDEPENDENT_CODE=On -DCMAKE_CXX_STANDARD=17
             -DCMAKE_C_STANDARD_REQUIRED=Yes
             -DCMAKE_INSTALL_PREFIX=${CMAKE_DEPS_PREFIX}
  PREFIX ${CMAKE_DEPS_PREFIX}
  EXCLUDE_FROM_ALL true
  LOG_DOWNLOAD On
  LOG_CONFIGURE On
  LOG_BUILD On
  LOG_INSTALL On)

add_library(libcpu_features STATIC IMPORTED)
set_property(
  TARGET libcpu_features
  PROPERTY IMPORTED_LOCATION
           ${CMAKE_DEPS_LIBDIR}/libcpu_features${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(libcpu_features cpu_features)

# -----------------------------
# Alias Target for External Use
# -----------------------------
add_library(Deps::cpu_features ALIAS libcpu_features)
