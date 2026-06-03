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

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SYSTEM_PROCESSOR MATCHES
                                          "x86_64|AMD64|amd64")
  file(
    GLOB_RECURSE
    SIMPLEST_OT_SRC
    ${CMAKE_CURRENT_LIST_DIR}/simplest_ot_x86_asm/*.c
    ${CMAKE_CURRENT_LIST_DIR}/simplest_ot_x86_asm/*.S)
  set(SIMPLEST_OT_SRC ${SIMPLEST_OT_SRC}
                      ${CMAKE_CURRENT_LIST_DIR}/x86_asm_ot_interface.cc)
else()
  file(GLOB_RECURSE SIMPLEST_OT_SRC
       ${CMAKE_CURRENT_LIST_DIR}/simplest_ot_portable/*.c)
  set(SIMPLEST_OT_SRC ${SIMPLEST_OT_SRC}
                      ${CMAKE_CURRENT_LIST_DIR}/portable_ot_interface.cc)
endif()

# others
set(YACL_SOURCE_FILES
    ${YACL_SOURCE_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/base_ot.cc
    ${CMAKE_CURRENT_LIST_DIR}/ferret_ote.cc
    ${CMAKE_CURRENT_LIST_DIR}/gywz_ote.cc
    ${CMAKE_CURRENT_LIST_DIR}/iknp_ote.cc
    ${CMAKE_CURRENT_LIST_DIR}/kkrt_ote.cc
    ${CMAKE_CURRENT_LIST_DIR}/kos_ote.cc
    ${CMAKE_CURRENT_LIST_DIR}/ot_store.cc
    ${CMAKE_CURRENT_LIST_DIR}/ot_store_utils.cc
    ${CMAKE_CURRENT_LIST_DIR}/sgrr_ote.cc
    ${CMAKE_CURRENT_LIST_DIR}/slice_base.cc
    ${CMAKE_CURRENT_LIST_DIR}/softspoken_ote.cc
    ${SIMPLEST_OT_SRC})

set(YACL_SOURCE_FILES
    ${YACL_SOURCE_FILES}
    PARENT_SCOPE)

add_yacl_test_if(base_ot_test)
add_yacl_test_if(ferret_ote_test)
add_yacl_test_if(gywz_ote_test)
add_yacl_test_if(iknp_ote_test)
add_yacl_test_if(kkrt_ote_test)
add_yacl_test_if(kos_ote_test)
add_yacl_test_if(ot_store_test)
add_yacl_test_if(sgrr_ote_test)
add_yacl_test_if(softspoken_ote_test)
