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
    ${YACL_SOURCE_FILES} ${CMAKE_CURRENT_LIST_DIR}/capsule.cc
    ${CMAKE_CURRENT_LIST_DIR}/hash.cc ${CMAKE_CURRENT_LIST_DIR}/kdf.cc
    ${CMAKE_CURRENT_LIST_DIR}/keys.cc ${CMAKE_CURRENT_LIST_DIR}/tpre.cc)

# Add header files for installation
install(
  FILES ${CMAKE_CURRENT_LIST_DIR}/capsule.h ${CMAKE_CURRENT_LIST_DIR}/hash.h
        ${CMAKE_CURRENT_LIST_DIR}/kdf.h ${CMAKE_CURRENT_LIST_DIR}/keys.h
        ${CMAKE_CURRENT_LIST_DIR}/tpre.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yacl/crypto/experimental/tpre)

set(YACL_SOURCE_FILES
    ${YACL_SOURCE_FILES}
    PARENT_SCOPE)

add_yacl_test_if(capsule_test)
add_yacl_test_if(hash_test)
add_yacl_test_if(kdf_test)
add_yacl_test_if(keys_test)
add_yacl_test_if(tpre_test)
