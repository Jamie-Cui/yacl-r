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

ExternalProject_Add(fourq
  URL
     "https://github.com/microsoft/FourQlib/archive/\
1031567f23278e1135b35cc04e5d74c2ac88c029.tar.gz"
  URL_HASH
    SHA256=7417c829d7933facda568c7a08924dfefb0c83dd1dab411e597af4c0cc0417f0
  PREFIX ${CMAKE_THIRDPARTY_PREFIX}
  PATCH_COMMAND
    patch -p1 -l --binary -i ${PROJECT_SOURCE_DIR}/bazel/patches/FourQlib.patch
  CONFIGURE_COMMAND "" # no configure
  BUILD_IN_SOURCE On
  BUILD_COMMAND
    ${CMAKE_MAKE_PROGRAM} ARCH=x64 GENERIC=TRUE EXTENDED_SET=FALSE -C FourQ_64bit_and_portable libFourQ
  INSTALL_COMMAND
    mkdir -p ${CMAKE_THIRDPARTY_LIBDIR}
  COMMAND
    mkdir -p ${CMAKE_THIRDPARTY_INCLUDEDIR}/fourq
  COMMAND
    ${CMAKE_MAKE_PROGRAM} PREFIX=${CMAKE_THIRDPARTY_PREFIX}
    -C FourQ_64bit_and_portable install
  COMMAND
    mv ${CMAKE_THIRDPARTY_INCLUDEDIR}/FourQ.h ${CMAKE_THIRDPARTY_INCLUDEDIR}/fourq/
  COMMAND
    mv ${CMAKE_THIRDPARTY_INCLUDEDIR}/FourQ_api.h ${CMAKE_THIRDPARTY_INCLUDEDIR}/fourq/
  COMMAND
    mv ${CMAKE_THIRDPARTY_INCLUDEDIR}/FourQ_internal.h ${CMAKE_THIRDPARTY_INCLUDEDIR}/fourq/
  COMMAND
    mv ${CMAKE_THIRDPARTY_INCLUDEDIR}/random.h ${CMAKE_THIRDPARTY_INCLUDEDIR}/fourq/
  LOG_DOWNLOAD On
  LOG_CONFIGURE On
  LOG_BUILD On
  LOG_INSTALL On)

add_library(libfourq STATIC IMPORTED)
set_property(
  TARGET libfourq PROPERTY
  IMPORTED_LOCATION
    ${CMAKE_THIRDPARTY_LIBDIR}/libFourQ${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(libfourq fourq)

# -----------------------------
# Alias Target for External Use
# -----------------------------
add_library(Thirdparty::fourq ALIAS libfourq)

# HACK we need to add the following global compiler flag in order to use fourq
add_compiler_flags("-D __LINUX__")
add_compiler_flags("-D _ARM64_")
