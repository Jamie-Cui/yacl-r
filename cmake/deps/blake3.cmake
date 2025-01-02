# Copyright 2024 Jamie Cui
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

ExternalProject_Add(blake3
  URL
    "https://github.com/BLAKE3-team/BLAKE3/archive/refs/tags/1.5.4.tar.gz"
  URL_HASH
    SHA256=ddd24f26a31d23373e63d9be2e723263ac46c8b6d49902ab08024b573fd2a416
  CMAKE_ARGS ${CMAKE_ARGS}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_THIRDPARTY_PREFIX}
    -DCMAKE_INSTALL_INCLUDEDIR=${CMAKE_THIRDPARTY_INCLUDEDIR}/blake3
  PREFIX ${CMAKE_THIRDPARTY_PREFIX}
  SOURCE_SUBDIR c
  EXCLUDE_FROM_ALL true
  LOG_DOWNLOAD On
  LOG_CONFIGURE On
  LOG_BUILD On
  LOG_INSTALL On)

add_library(libblake3 STATIC IMPORTED)
set_property(
  TARGET libblake3 PROPERTY
  IMPORTED_LOCATION
    ${CMAKE_THIRDPARTY_LIBDIR}/libblake3${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(libblake3 blake3)

# -----------------------------
# Alias Target for External Use
# -----------------------------
add_library(Thirdparty::blake3 ALIAS libblake3)
