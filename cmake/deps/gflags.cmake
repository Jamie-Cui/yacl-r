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
  gflags
  URL https://github.com/gflags/gflags/archive/v2.2.2.tar.gz
  URL_HASH
    SHA256=34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf
  CMAKE_ARGS -DCMAKE_POLICY_VERSION_MINIMUM=3.5
             -DBUILD_gflags_nothreads_LIB=Off
             -DGFLAGS_BUILD_SHARED_LIBS=Off # NOTE brpc requires shared lib
             -DGFLAGS_BUILD_STATIC_LIBS=On # NOTE brpc requires shared lib
             -DGFLAGS_BUILD_TESTING=Off
             -DCMAKE_POSITION_INDEPENDENT_CODE=On
             -DCMAKE_CXX_STANDARD=17
             -DCMAKE_C_STANDARD_REQUIRED=Yes
             -DCMAKE_INSTALL_PREFIX=${CMAKE_DEPS_PREFIX}
             -DSPDLOG_FMT_EXTERNAL=On
             -DCMAKE_CXX_FLAGS=-isystem\ ${CMAKE_DEPS_INCLUDEDIR}
  INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install
  COMMAND mv -n ${CMAKE_DEPS_PREFIX}/lib/libgflags${CMAKE_STATIC_LIBRARY_SUFFIX}
          ${CMAKE_DEPS_LIBDIR}/libgflags${CMAKE_STATIC_LIBRARY_SUFFIX}
  PREFIX ${CMAKE_DEPS_PREFIX}
  EXCLUDE_FROM_ALL true
  BUILD_BYPRODUCTS ${CMAKE_DEPS_LIBDIR}/libgflags${CMAKE_STATIC_LIBRARY_SUFFIX}
                   DOWNLOAD_EXTRACT_TIMESTAMP On
  LOG_DOWNLOAD On
  LOG_CONFIGURE On
  LOG_BUILD On
  LOG_INSTALL On)

import_static_lib_from(libgflags gflags)

# -----------------------------
# Alias Target for External Use
# -----------------------------
add_library(Deps::gflags ALIAS libgflags)
