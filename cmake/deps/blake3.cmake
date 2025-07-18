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
  blake3
  URL https://github.com/BLAKE3-team/BLAKE3/archive/refs/tags/1.5.4.tar.gz
  URL_HASH
    SHA256=ddd24f26a31d23373e63d9be2e723263ac46c8b6d49902ab08024b573fd2a416
  CMAKE_ARGS -DCMAKE_POSITION_INDEPENDENT_CODE=On
             -DCMAKE_CXX_STANDARD=17
             -DCMAKE_C_STANDARD_REQUIRED=Yes
             -DCMAKE_INSTALL_PREFIX=${CMAKE_DEPS_PREFIX}
             -DCMAKE_INSTALL_INCLUDEDIR=${CMAKE_DEPS_INCLUDEDIR}/blake3
  PREFIX ${CMAKE_DEPS_PREFIX}
  SOURCE_SUBDIR c
  EXCLUDE_FROM_ALL true
  LOG_DOWNLOAD On
  LOG_CONFIGURE On
  LOG_BUILD On
  LOG_INSTALL On
  DOWNLOAD_EXTRACT_TIMESTAMP On)

import_static_lib_from(libblake3 blake3)

# -----------------------------
# Alias Target for External Use
# -----------------------------
add_library(Deps::blake3 ALIAS libblake3)
