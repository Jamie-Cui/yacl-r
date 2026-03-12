# Copyright 2024 Jamie Cui
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
    ${CMAKE_CURRENT_LIST_DIR}/r1cs.cc
    ${CMAKE_CURRENT_LIST_DIR}/qap.cc
    ${CMAKE_CURRENT_LIST_DIR}/groth16.cc
    ${CMAKE_CURRENT_LIST_DIR}/circuit_builder.cc
    ${CMAKE_CURRENT_LIST_DIR}/gadgets.cc
    ${CMAKE_CURRENT_LIST_DIR}/serialization.cc)

set(YACL_SOURCE_FILES
    ${YACL_SOURCE_FILES}
    PARENT_SCOPE)

add_yacl_test_if(r1cs_test)
add_yacl_test_if(qap_test)
add_yacl_test_if(groth16_test)
add_yacl_test_if(circuit_builder_test)
add_yacl_test_if(gadgets_test)
add_yacl_fuzz_if(groth16_fuzz)
