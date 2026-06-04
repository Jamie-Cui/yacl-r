# Copyright 2024 Ant Group Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set(YACL_SOURCE_FILES
    ${YACL_SOURCE_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/ot_kernel.cc
    ${CMAKE_CURRENT_LIST_DIR}/svole_kernel.cc
    ${CMAKE_CURRENT_LIST_DIR}/plaintext_executor.cc
    ${CMAKE_CURRENT_LIST_DIR}/ss_executor.cc)

set(YACL_SOURCE_FILES
    ${YACL_SOURCE_FILES}
    PARENT_SCOPE)

add_yacl_test_if(ot_kernel_test)
add_yacl_test_if(svole_kernel_test)
add_yacl_test_if(ss_executor_test)
add_yacl_test_if(plaintext_executor_test)

# HACK
if(BUILD_TEST)
  set_tests_properties(plaintext_executor_test PROPERTIES WORKING_DIRECTORY
                                                          ${PROJECT_SOURCE_DIR})
endif()

# ------------
# Bench
# ------------

add_yacl_bench_if(ot_kernel_bench NO_MAIN)
add_yacl_bench_if(svole_kernel_bench NO_MAIN)
