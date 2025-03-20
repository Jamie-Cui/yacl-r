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
  CMAKE_ARGS -DMCL_STATIC_LIB=On
             -DCMAKE_POSITION_INDEPENDENT_CODE=On
             -DCMAKE_CXX_STANDARD=17
             -DCMAKE_C_STANDARD_REQUIRED=Yes
             -DCMAKE_INSTALL_PREFIX=${CMAKE_DEPS_PREFIX}
             -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
             -Wno-shift-count-overflow
  EXCLUDE_FROM_ALL LOG_DOWNLOAD
  On
  LOG_CONFIGURE On
  LOG_BUILD On
  LOG_INSTALL On)

add_library(libmcl_interface INTERFACE)

add_library(libmcl STATIC IMPORTED)
set_target_properties(
  libmcl PROPERTIES IMPORTED_LOCATION
                    ${CMAKE_DEPS_LIBDIR}/libmcl${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(libmcl mcl)

add_library(libmclbn256 STATIC IMPORTED)
set_target_properties(
  libmclbn256
  PROPERTIES IMPORTED_LOCATION
             ${CMAKE_DEPS_LIBDIR}/libmclbn256${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(libmclbn256 mcl)

add_library(libmclbn384 STATIC IMPORTED)
set_target_properties(
  libmclbn384
  PROPERTIES IMPORTED_LOCATION
             ${CMAKE_DEPS_LIBDIR}/libmclbn384${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(libmclbn384 mcl)

target_link_libraries(libmcl_interface INTERFACE libmcl libmclbn256 libmclbn384)

# -----------------------------
# Alias Target for External Use
# -----------------------------
add_library(Deps::mcl ALIAS libmcl_interface)
