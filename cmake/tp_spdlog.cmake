# Copyright 2024 Ant Group Co., Ltd.
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

ExternalProject_Add(spdlog
  URL
    https://github.com/gabime/spdlog/archive/refs/tags/v1.14.1.tar.gz
  URL_HASH
    SHA256=1586508029a7d0670dfcb2d97575dcdc242d3868a259742b69f100801ab4e16b
  CMAKE_ARGS ${CMAKE_ARGS}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_THIRDPARTY_PREFIX}
    -DSPDLOG_FMT_EXTERNAL=On
    -DCMAKE_CXX_FLAGS=-isystem\ ${CMAKE_THIRDPARTY_INCLUDEDIR}
  PREFIX ${CMAKE_THIRDPARTY_PREFIX}
  LOG_DOWNLOAD On
  LOG_CONFIGURE On
  LOG_BUILD On
  LOG_INSTALL On)

add_dependencies(spdlog fmt)

add_library(libspdlog STATIC IMPORTED)
set_property(
  TARGET libspdlog PROPERTY
  IMPORTED_LOCATION ${CMAKE_THIRDPARTY_LIBDIR}/libspdlog.a)
add_dependencies(libspdlog spdlog)

# -----------------------------
# Alias Target for External Use
# -----------------------------
add_library(Thirdparty::spdlog ALIAS libspdlog)

# HACK https://github.com/gabime/spdlog/issues/1897
add_compile_definitions(SPDLOG_FMT_EXTERNAL)

