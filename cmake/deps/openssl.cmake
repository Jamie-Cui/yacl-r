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
  openssl
  PREFIX ${CMAKE_DEPS_PREFIX}
  URL https://github.com/openssl/openssl/archive/refs/tags/openssl-3.3.2.tar.gz
  URL_HASH
    SHA256=bedbb16955555f99b1a7b1ba90fc97879eb41025081be359ecd6a9fcbdf1c8d2
  CONFIGURE_COMMAND
    ./Configure no-legacy no-weak-ssl-ciphers no-tests no-shared no-ui-console
    no-docs no-apps --banner=Finished --release --libdir=${CMAKE_INSTALL_LIBDIR}
    --prefix=${CMAKE_DEPS_PREFIX} -w
  BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} build_sw
  INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install_sw
  BUILD_IN_SOURCE On
  DOWNLOAD_EXTRACT_TIMESTAMP On
  EXCLUDE_FROM_ALL true
  LOG_DOWNLOAD On
  LOG_CONFIGURE On
  LOG_BUILD On
  LOG_INSTALL On)

import_static_lib_from(libcrypto openssl)
import_static_lib_from(libssl openssl)

target_link_libraries(libssl INTERFACE libcrypto)

# -----------------------------
# Alias Target for External Use
# -----------------------------
add_library(Deps::openssl ALIAS libssl)
