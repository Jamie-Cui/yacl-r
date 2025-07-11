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

set(YACL_SOURCE_FILES ${YACL_SOURCE_FILES}
                      ${CMAKE_CURRENT_LIST_DIR}/key_utils.cc)

# Add header files for installation
install(FILES ${CMAKE_CURRENT_LIST_DIR}/key_utils.h
              ${CMAKE_CURRENT_LIST_DIR}/ossl_wrappers.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yacl/crypto)

add_subdirectory(aead)
add_subdirectory(aes)
add_subdirectory(block_cipher)
add_subdirectory(ecc)
add_subdirectory(envelope)
add_subdirectory(experimental)
add_subdirectory(hash)
add_subdirectory(hmac)
add_subdirectory(oprf)
add_subdirectory(ossl_provider)
add_subdirectory(pairing)
add_subdirectory(pke)
add_subdirectory(rand)
add_subdirectory(sign)
add_subdirectory(tools)

set(YACL_SOURCE_FILES
    ${YACL_SOURCE_FILES}
    PARENT_SCOPE)

set(YACL_SPI_LIBS
    ${YACL_SPI_LIBS}
    PARENT_SCOPE)

# ------------
# Test
# ------------

add_yacl_test_if(key_utils_test)
