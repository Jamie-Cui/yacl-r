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

file(GLOB SRC_CC *.cc)
file(GLOB SRC_H *.h)

set(YACL_SOURCE_FILES ${YACL_SOURCE_FILES} ${SRC_CC})

# Add header files for installation
install(FILES ${SRC_H}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yacl/io/link/algorithm)

set(YACL_SOURCE_FILES
    ${YACL_SOURCE_FILES}
    PARENT_SCOPE)

add_yacl_test_if(allgather_test)
add_yacl_test_if(barrier_test)
add_yacl_test_if(broadcast_test)
add_yacl_test_if(gather_test)
add_yacl_test_if(scatter_test)
