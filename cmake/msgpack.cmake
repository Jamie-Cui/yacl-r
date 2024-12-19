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

ExternalProject_Add(msgpack
  URL
    https://github.com/msgpack/msgpack-c/archive/refs/tags/cpp-6.1.0.tar.gz
  URL_HASH
    SHA256=5e63e4d9b12ab528fccf197f7e6908031039b1fc89cd8da0e97fbcbf5a6c6d3a
  CMAKE_ARGS ${CMAKE_ARGS}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_THIRDPARTY_PREFIX}
    -DMSGPACK_CXX17=On
    -DMSGPACK_USE_BOOST=Off
    -DMSGPACK_BUILD_EXAMPLES=Off
    -DMSGPACK_BUILD_EXAMPLES=Off
    -DBUILD_SHARED_LIBS=Off
    -DMSGPACK_BUILD_TESTS=Off
  PREFIX ${CMAKE_THIRDPARTY_PREFIX}
  LOG_DOWNLOAD On
  LOG_CONFIGURE On
  LOG_BUILD On
  LOG_INSTALL On)

# msgpack is header-only, therefore no need to link

# HACK
add_compile_definitions(MSGPACK_NO_BOOST)
