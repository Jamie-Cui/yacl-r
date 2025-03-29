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

file(MAKE_DIRECTORY ${CMAKE_DEPS_INCLUDEDIR}/sse2neon)

ExternalProject_Add(
  sse2neon
  URL https://github.com/DLTcollab/sse2neon/archive/8df2f48dbd0674ae5087f7a6281af6f55fa5a8e2.tar.gz
  URL_HASH
    SHA256=787e0a7a64f1461b48232a7f9b9e9c14fa4a35a30875f2fb91aec6ddeaddfc0f
  PREFIX ${CMAKE_DEPS_PREFIX}
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  BUILD_IN_SOURCE On
  COMMAND cp -a sse2neon.h ${CMAKE_DEPS_INCLUDEDIR}/sse2neon
  EXCLUDE_FROM_ALL true
  LOG_DOWNLOAD On
  LOG_CONFIGURE On
  LOG_BUILD On
  LOG_INSTALL On)

add_library(libsse2neon INTERFACE)
add_dependencies(libsse2neon sse2neon)

# -----------------------------
# Alias Target for External Use
# -----------------------------
add_library(Deps::sse2neon ALIAS libsse2neon)
