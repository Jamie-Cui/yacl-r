# Copyright 2024 Ant Group Co., Ltd.
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

ExternalProject_Add(mcl
  URL
    https://github.com/herumi/mcl/archive/refs/tags/v1.99.tar.gz
  URL_HASH
    SHA256=5ff9702c1f1b021925d1334ca0a03c87783174075aeaf87801842d3c08b3d39e
  PREFIX ${CMAKE_THIRDPARTY_PREFIX}
  CMAKE_ARGS ${CMAKE_ARGS}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_THIRDPARTY_PREFIX}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS} -Wno-shift-count-overflow
  LOG_DOWNLOAD On
  LOG_CONFIGURE On
  LOG_BUILD On
  LOG_INSTALL On)

add_library(libmcl STATIC IMPORTED)
set_property(
  TARGET libmcl PROPERTY
  IMPORTED_LOCATION
    ${CMAKE_THIRDPARTY_LIBDIR}/libmcl${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(libmcl mcl)

# -----------------------------
# Alias Target for External Use
# -----------------------------
add_library(Thirdparty::mcl ALIAS libmcl)
