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

ExternalProject_Add(
  mcl
  URL https://github.com/herumi/mcl/archive/refs/tags/v1.99.tar.gz
  URL_HASH
    SHA256=5ff9702c1f1b021925d1334ca0a03c87783174075aeaf87801842d3c08b3d39e
  PREFIX ${CMAKE_DEPS_PREFIX}
  CONFIGURE_COMMAND ""
  BUILD_IN_SOURCE On
  BUILD_COMMAND make -j
  INSTALL_COMMAND cp -n -r include/mcl include/cybozu ${CMAKE_DEPS_INCLUDEDIR}
  COMMAND cp -n lib/libmcl${CMAKE_STATIC_LIBRARY_SUFFIX} ${CMAKE_DEPS_LIBDIR}
  COMMAND cp -n lib/libmclbn256${CMAKE_STATIC_LIBRARY_SUFFIX}
          ${CMAKE_DEPS_LIBDIR}
  COMMAND cp -n lib/libmclbn384${CMAKE_STATIC_LIBRARY_SUFFIX}
          ${CMAKE_DEPS_LIBDIR}
  BUILD_BYPRODUCTS ${CMAKE_DEPS_LIBDIR}/libmcl${CMAKE_STATIC_LIBRARY_SUFFIX}
  BUILD_BYPRODUCTS
    ${CMAKE_DEPS_LIBDIR}/libmclbn256${CMAKE_STATIC_LIBRARY_SUFFIX}
  BUILD_BYPRODUCTS
    ${CMAKE_DEPS_LIBDIR}/libmclbn384${CMAKE_STATIC_LIBRARY_SUFFIX}
  EXCLUDE_FROM_ALL On
  DOWNLOAD_EXTRACT_TIMESTAMP On
  LOG_DOWNLOAD On
  LOG_CONFIGURE On
  LOG_BUILD On
  LOG_INSTALL On)

add_library(libmcl_interface INTERFACE)

import_static_lib_from(libmcl mcl)
import_static_lib_from(libmclbn256 mcl)
import_static_lib_from(libmclbn384 mcl)

target_link_libraries(libmclbn256 INTERFACE libmcl)
target_link_libraries(libmclbn384 INTERFACE libmcl)
target_link_libraries(libmcl_interface INTERFACE libmclbn256 libmclbn384)

# -----------------------------
# Alias Target for External Use
# -----------------------------
add_library(Deps::mcl ALIAS libmcl_interface)
