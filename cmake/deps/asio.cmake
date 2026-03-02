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

message(STATUS "Downloading asio")

FetchContent_Declare(
  asio
  URL https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-30-2.tar.gz
  URL_HASH
    SHA256=c4553b87b5fc48a3c6c6d128e1c4464f1e9f15d68acd3f14c6c53019e5dbe7e7
  DOWNLOAD_EXTRACT_TIMESTAMP On SOURCE_DIR ${CMAKE_DEPS_SRCDIR}/asio)

message(STATUS "Downloading asio - Success")

FetchContent_GetProperties(asio)

if(NOT asio_POPULATED)
  FetchContent_Populate(asio)
endif()

add_library(libasio INTERFACE)
target_include_directories(libasio
                           INTERFACE ${CMAKE_DEPS_SRCDIR}/asio/asio/include)
target_compile_definitions(libasio INTERFACE ASIO_STANDALONE ASIO_NO_DEPRECATED)

add_library(Deps::asio ALIAS libasio)