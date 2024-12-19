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

ExternalProject_Add(fmt
  URL
    https://github.com/fmtlib/fmt/archive/refs/tags/11.0.2.tar.gz
  URL_HASH
    SHA256=6cb1e6d37bdcb756dbbe59be438790db409cdb4868c66e888d5df9f13f7c027f
  CMAKE_ARGS
    ${CMAKE_ARGS} -DCMAKE_INSTALL_PREFIX=${CMAKE_THIRDPARTY_PREFIX}
  PREFIX ${CMAKE_THIRDPARTY_PREFIX}
  LOG_DOWNLOAD On
  LOG_CONFIGURE On
  LOG_BUILD On
  LOG_INSTALL On)

add_library(libfmt STATIC IMPORTED)
set_property(
  TARGET libfmt PROPERTY
  IMPORTED_LOCATION ${CMAKE_THIRDPARTY_LIBDIR}/libfmt.a)
add_dependencies(libfmt fmt)

add_library(External::fmt ALIAS libfmt)