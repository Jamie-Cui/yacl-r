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

set(YACL_SOURCE_FILES
    ${YACL_SOURCE_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/elapsed_timer.cc
    ${CMAKE_CURRENT_LIST_DIR}/matrix_utils.cc
    ${CMAKE_CURRENT_LIST_DIR}/parallel.cc
    ${CMAKE_CURRENT_LIST_DIR}/platform_utils.cc
    ${CMAKE_CURRENT_LIST_DIR}/thread_pool.cc)

file(GLOB SRC_H *.h)

# Add header files for installation
install(FILES ${SRC_H} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yacl/utils)

add_subdirectory(spi)

set(YACL_SOURCE_FILES
    ${YACL_SOURCE_FILES}
    PARENT_SCOPE)

add_yacl_test(elapsed_timer_test)
add_yacl_test(hamming_test)
add_yacl_test(matrix_transpose_test)
add_yacl_test(parallel_test)
add_yacl_test(platform_utils_test)
add_yacl_test(scope_guard_test)
add_yacl_test(segment_tree_test)
add_yacl_test(serializer_test)
