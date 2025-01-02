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

ExternalProject_Add(googletest
  URL
    https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
  URL_HASH
    SHA256=7b42b4d6ed48810c5362c265a17faebe90dc2373c885e5216439d37927f02926
  CMAKE_ARGS
    ${CMAKE_ARGS}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_THIRDPARTY_PREFIX}
    -DBUILD_GMOCK=Off
  PREFIX ${CMAKE_THIRDPARTY_PREFIX}
  EXCLUDE_FROM_ALL true
  LOG_DOWNLOAD On
  LOG_CONFIGURE On
  LOG_BUILD On
  LOG_INSTALL On)

add_library(libgtest STATIC IMPORTED)
set_property(
  TARGET libgtest PROPERTY
  IMPORTED_LOCATION
  ${CMAKE_THIRDPARTY_LIBDIR}/libgtest${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(libgtest googletest)

add_library(libgtest_main STATIC IMPORTED)
set_property(
  TARGET libgtest_main PROPERTY
  IMPORTED_LOCATION
  ${CMAKE_THIRDPARTY_LIBDIR}/libgtest_main${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(libgtest_main googletest)

add_library(libgtest_interface INTERFACE)
target_link_libraries(libgtest_interface
  INTERFACE libgtest_main libgtest)

# -----------------------------
# Alias Target for External Use
# -----------------------------
add_library(Thirdparty::gtest ALIAS libgtest_interface)
