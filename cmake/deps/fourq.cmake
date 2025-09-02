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

ExternalProject_Add(
  fourq
  URL https://github.com/microsoft/FourQlib/archive/1031567f23278e1135b35cc04e5d74c2ac88c029.tar.gz
  URL_HASH
    SHA256=7417c829d7933facda568c7a08924dfefb0c83dd1dab411e597af4c0cc0417f0
  PREFIX ${CMAKE_DEPS_PREFIX}
  PATCH_COMMAND patch -p1 -l -f -i
                ${PROJECT_SOURCE_DIR}/cmake/patches/FourQlib.patch || true
  CONFIGURE_COMMAND "" # no configure
  BUILD_IN_SOURCE On
  BUILD_COMMAND make ARCH=x64 GENERIC=TRUE EXTENDED_SET=FALSE -C
                FourQ_64bit_and_portable libFourQ
  INSTALL_COMMAND mkdir -p ${CMAKE_DEPS_LIBDIR}
  COMMAND mkdir -p ${CMAKE_DEPS_INCLUDEDIR}/fourq
  # HACK fourq does not support ninja, so use gnumake
  COMMAND make PREFIX=${CMAKE_DEPS_PREFIX} -C FourQ_64bit_and_portable install
  COMMAND mv -n ${CMAKE_DEPS_PREFIX}/lib/libFourQ${CMAKE_STATIC_LIBRARY_SUFFIX}
          ${CMAKE_DEPS_LIBDIR}/libFourQ${CMAKE_STATIC_LIBRARY_SUFFIX}
  BUILD_BYPRODUCTS ${CMAKE_DEPS_LIBDIR}/libFourQ${CMAKE_STATIC_LIBRARY_SUFFIX}
  EXCLUDE_FROM_ALL true
  DOWNLOAD_EXTRACT_TIMESTAMP On
  LOG_DOWNLOAD On
  LOG_CONFIGURE On
  LOG_BUILD On
  LOG_INSTALL On)

import_static_lib_from(libFourQ fourq)

# -----------------------------
# Alias Target for External Use
# -----------------------------
add_library(Deps::fourq ALIAS libFourQ)
