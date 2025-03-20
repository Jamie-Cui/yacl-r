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
  libtommath
  URL https://github.com/libtom/libtommath/archive/42b3fb07e7d504f61a04c7fca12e996d76a25251.tar.gz
  URL_HASH
    SHA256=7cfbdb64431129de4257e7d3349200fdbd4f229b470ff3417b30d0f39beed41f
  CMAKE_ARGS -DCMAKE_POSITION_INDEPENDENT_CODE=On -DCMAKE_CXX_STANDARD=17
             -DCMAKE_C_STANDARD_REQUIRED=Yes
             -DCMAKE_INSTALL_PREFIX=${CMAKE_DEPS_PREFIX}
  PREFIX ${CMAKE_DEPS_PREFIX}
  EXCLUDE_FROM_ALL true
  LOG_DOWNLOAD On
  LOG_CONFIGURE On
  LOG_BUILD On
  LOG_INSTALL On)

add_library(liblibtommath STATIC IMPORTED)
set_target_properties(
  liblibtommath
  PROPERTIES IMPORTED_LOCATION
             ${CMAKE_DEPS_LIBDIR}/libtommath${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(liblibtommath libtommath)

# -----------------------------
# Alias Target for External Use
# -----------------------------
add_library(Deps::libtommath ALIAS liblibtommath)
