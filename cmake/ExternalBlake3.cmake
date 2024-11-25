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

FetchContent_Declare(
  blake3
  URL https://github.com/BLAKE3-team/BLAKE3/archive/refs/tags/1.5.4.tar.gz
  URL_HASH
    SHA256=ddd24f26a31d23373e63d9be2e723263ac46c8b6d49902ab08024b573fd2a416
  SOURCE_SUBDIR c)

FetchContent_MakeAvailable(blake3)

include_directories(${blake3_SOURCE_DIR})

