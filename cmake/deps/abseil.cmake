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
  abseil
  URL https://github.com/abseil/abseil-cpp/archive/refs/tags/20240722.0.tar.gz
  URL_HASH
    SHA256=f50e5ac311a81382da7fa75b97310e4b9006474f9560ac46f54a9967f07d4ae3
  CMAKE_ARGS -DCMAKE_POSITION_INDEPENDENT_CODE=On #
             -DCMAKE_CXX_STANDARD=17 #
             -DCMAKE_C_STANDARD_REQUIRED=Yes #
             -DCMAKE_INSTALL_PREFIX=${CMAKE_DEPS_PREFIX} #
  PREFIX ${CMAKE_DEPS_PREFIX}
  INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install
  COMMAND bash ${PROJECT_SOURCE_DIR}/cmake/scripts/unify-static-libs.sh
          libabsl${CMAKE_STATIC_LIBRARY_SUFFIX} ${CMAKE_DEPS_LIBDIR} libabsl_*
  EXCLUDE_FROM_ALL true
  LOG_DOWNLOAD On
  LOG_CONFIGURE On
  LOG_BUILD On
  LOG_INSTALL On)

# ------------------------------------------------------------------------------
# How to use absl?
# ------------------------------------------------------------------------------

add_library(liblibabsl STATIC IMPORTED)
set_property(
  TARGET liblibabsl
  PROPERTY IMPORTED_LOCATION
           ${CMAKE_DEPS_LIBDIR}/libabsl${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(liblibabsl abseil)

# -----------------------------
# Alias Target for External Use
# -----------------------------
add_library(Deps::absl ALIAS liblibabsl)
