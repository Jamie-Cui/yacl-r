# Copyright 2025 Jamie Cui
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

ExternalProject_Add(
  asio
  URL https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-30-2.tar.gz
  URL_HASH
    SHA256=755bd7f85a4b269c67ae0ea254907c078d408cce8e1a352ad2ed664d233780e8
  PREFIX ${CMAKE_DEPS_PREFIX}
  CONFIGURE_COMMAND "" # no configure
  BUILD_COMMAND "" # header-only, no build
  INSTALL_COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_DEPS_INCLUDEDIR}/asio
  COMMAND ${CMAKE_COMMAND} -E copy_directory 
          ${CMAKE_DEPS_PREFIX}/src/asio/asio/include 
          ${CMAKE_DEPS_INCLUDEDIR}/asio
  EXCLUDE_FROM_ALL true
  DOWNLOAD_EXTRACT_TIMESTAMP On
  LOG_DOWNLOAD On
  LOG_CONFIGURE On
  LOG_BUILD On
  LOG_INSTALL On)

add_library(libasio INTERFACE IMPORTED GLOBAL)
target_include_directories(libasio INTERFACE ${CMAKE_DEPS_INCLUDEDIR}/asio)
target_compile_definitions(libasio INTERFACE ASIO_STANDALONE ASIO_NO_DEPRECATED)

add_dependencies(libasio asio)

add_library(Deps::asio ALIAS libasio)
