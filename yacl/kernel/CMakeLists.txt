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

file(GLOB_RECURSE SRC_CC *.cc)
file(GLOB_RECURSE SRC_H *.h)
file(GLOB_RECURSE SRC_TEST_CC *_test.cc)

list(
  REMOVE_ITEM
  SRC_CC
  ${SRC_TEST_CC}
  ${CMAKE_CURRENT_LIST_DIR}/ot_kernel_bench.cc
  ${CMAKE_CURRENT_LIST_DIR}/svole_kernel_bench.cc
  ${CMAKE_CURRENT_LIST_DIR}/benchmark/ot_bench.cc
  ${CMAKE_CURRENT_LIST_DIR}/benchmark/vole_bench.cc
  ${CMAKE_CURRENT_LIST_DIR}/code/benchmark.cc)

set(YACL_SOURCE_FILES ${YACL_SOURCE_FILES} ${SRC_CC})

# simplest-ot
file(GLOB_RECURSE SIMPLEST_OT_PORTABLE_CC 
     ${CMAKE_CURRENT_LIST_DIR}/simplest-ot/simplest_ot_portable/*.c)
set(YACL_SOURCE_FILES ${YACL_SOURCE_FILES} ${SIMPLEST_OT_PORTABLE_CC})

file(GLOB_RECURSE SIMPLEST_OT_X86_ASM_CC 
     ${CMAKE_CURRENT_LIST_DIR}/simplest-ot/simplest_ot_x86_asm/*.c)
file(GLOB_RECURSE SIMPLEST_OT_X86_ASM_S 
     ${CMAKE_CURRENT_LIST_DIR}/simplest-ot/simplest_ot_x86_asm/*.S)
file(GLOB_RECURSE SIMPLEST_OT_X86_ASM_CCC 
     ${CMAKE_CURRENT_LIST_DIR}/simplest-ot/simplest_ot_x86_asm/*.cc)
# Exclude randombytes and Keccak (already in portable version)
list(REMOVE_ITEM SIMPLEST_OT_X86_ASM_CC 
     ${CMAKE_CURRENT_LIST_DIR}/simplest-ot/simplest_ot_x86_asm/randombytes.c)
list(REMOVE_ITEM SIMPLEST_OT_X86_ASM_CC 
     ${CMAKE_CURRENT_LIST_DIR}/simplest-ot/simplest_ot_x86_asm/Keccak-simple.c)
set(YACL_SOURCE_FILES ${YACL_SOURCE_FILES} 
     ${SIMPLEST_OT_X86_ASM_CC} ${SIMPLEST_OT_X86_ASM_S} ${SIMPLEST_OT_X86_ASM_CCC})

set(YACL_SOURCE_FILES
    ${YACL_SOURCE_FILES}
    PARENT_SCOPE)

add_yacl_test_if(ot_kernel_test)
add_yacl_test_if(svole_kernel_test)

# Add header files for installation
install(FILES ${SRC_H} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yacl/kernel)
