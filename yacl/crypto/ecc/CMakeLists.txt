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

# Add header files for installation
install(FILES
    ${CMAKE_CURRENT_LIST_DIR}/curve_meta.h
    ${CMAKE_CURRENT_LIST_DIR}/ec_point.h
    ${CMAKE_CURRENT_LIST_DIR}/ecc_spi.h
    ${CMAKE_CURRENT_LIST_DIR}/group_sketch.h
    ${CMAKE_CURRENT_LIST_DIR}/any_ptr.h
    ${CMAKE_CURRENT_LIST_DIR}/FourQlib/FourQ_group.h
    ${CMAKE_CURRENT_LIST_DIR}/mcl/mcl_ec_group.h
    ${CMAKE_CURRENT_LIST_DIR}/mcl/mcl_util.h
    ${CMAKE_CURRENT_LIST_DIR}/openssl/openssl_group.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yacl/crypto/ecc)

# add_subdirectory(benchmark) # TODO
# add_subdirectory(libsodium)
# add_subdirectory(toy)

add_yacl_test(ecc_test)

# ------------------------------------------------------------------------------
# For libspi_ecc
# ------------------------------------------------------------------------------

add_library(spi_ecc STATIC
    ${CMAKE_CURRENT_LIST_DIR}/curve_meta.cc
    ${CMAKE_CURRENT_LIST_DIR}/ec_point.cc
    ${CMAKE_CURRENT_LIST_DIR}/ecc_spi.cc
    ${CMAKE_CURRENT_LIST_DIR}/group_sketch.cc
    ${CMAKE_CURRENT_LIST_DIR}/FourQlib/FourQ_factory.cc
    ${CMAKE_CURRENT_LIST_DIR}/FourQlib/FourQ_group.cc
    ${CMAKE_CURRENT_LIST_DIR}/mcl/mcl_ec_factory.cc
    ${CMAKE_CURRENT_LIST_DIR}/mcl/mcl_ec_group.cc
    ${CMAKE_CURRENT_LIST_DIR}/mcl/mcl_util.cc
    ${CMAKE_CURRENT_LIST_DIR}/openssl/openssl_factory.cc
    ${CMAKE_CURRENT_LIST_DIR}/openssl/openssl_group.cc)

target_include_directories(spi_ecc PUBLIC
  ${CMAKE_THIRDPARTY_INCLUDEDIR}
  # these dirs are only used when building inside the build tree
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>")

target_link_libraries(yacl PUBLIC
    $<LINK_LIBRARY:WHOLE_ARCHIVE,spi_ecc>)
