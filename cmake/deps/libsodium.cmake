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
  libsodium_repo # HACK avoid name conflicts
  URL https://github.com/jedisct1/libsodium/releases/download/1.0.18-RELEASE/libsodium-1.0.18.tar.gz
  URL_HASH
    SHA256=6f504490b342a4f8a4c4a02fc9b866cbef8622d5df4e5452b46be121e46636c1
  PREFIX ${CMAKE_DEPS_PREFIX}
  CONFIGURE_COMMAND ./configure --prefix=${CMAKE_DEPS_PREFIX} --enable-shared=no
                    --libdir=${CMAKE_DEPS_LIBDIR}
  BUILD_BYPRODUCTS ${CMAKE_DEPS_LIBDIR}/libsodium${CMAKE_STATIC_LIBRARY_SUFFIX}
  BUILD_IN_SOURCE On
  EXCLUDE_FROM_ALL true
  DOWNLOAD_EXTRACT_TIMESTAMP On
  LOG_DOWNLOAD On
  LOG_CONFIGURE On
  LOG_BUILD On
  LOG_INSTALL On)

import_static_lib_from(libsodium libsodium_repo)

# -----------------------------
# Alias Target for External Use
# -----------------------------
add_library(Deps::libsodium ALIAS libsodium)
