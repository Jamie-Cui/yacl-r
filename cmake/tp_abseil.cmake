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

ExternalProject_Add(abseil
  URL
    "https://github.com/abseil/abseil-cpp/archive/refs/tags/\
20240722.0.tar.gz"
  URL_HASH
    SHA256=f50e5ac311a81382da7fa75b97310e4b9006474f9560ac46f54a9967f07d4ae3
  CMAKE_ARGS ${CMAKE_ARGS}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_THIRDPARTY_PREFIX}
    -DABSL_BUILD_MONOLITHIC_SHARED_LIBS=On
    -DBUILD_SHARED_LIBS=On
  PREFIX ${CMAKE_THIRDPARTY_PREFIX}
  LOG_DOWNLOAD On
  LOG_CONFIGURE On
  LOG_BUILD On
  LOG_INSTALL On)

add_library(libabseil SHARED IMPORTED)
set_property(
  TARGET libabseil PROPERTY
  IMPORTED_LOCATION ${CMAKE_THIRDPARTY_LIBDIR}/libabseil_dll${CMAKE_SHARED_LIBRARY_SUFFIX})
add_dependencies(libabseil abseil)

# -----------------------------
# Alias Target for External Use
# -----------------------------
add_library(Thirdparty::absl ALIAS libabseil)
