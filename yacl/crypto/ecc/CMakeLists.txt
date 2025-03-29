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

set(YACL_SOURCE_FILES ${YACL_SOURCE_FILES}
                      ${CMAKE_CURRENT_LIST_DIR}/mcl/mcl_util.cc)

set(YACL_SOURCE_FILES
    ${YACL_SOURCE_FILES}
    PARENT_SCOPE)

# Add header files for installation
install(
  FILES ${CMAKE_CURRENT_LIST_DIR}/curve_meta.h
        ${CMAKE_CURRENT_LIST_DIR}/ec_point.h
        ${CMAKE_CURRENT_LIST_DIR}/ecc_spi.h
        ${CMAKE_CURRENT_LIST_DIR}/group_sketch.h
        ${CMAKE_CURRENT_LIST_DIR}/any_ptr.h
        ${CMAKE_CURRENT_LIST_DIR}/FourQlib/FourQ_group.h
        ${CMAKE_CURRENT_LIST_DIR}/mcl/mcl_ec_group.h
        ${CMAKE_CURRENT_LIST_DIR}/mcl/mcl_util.h
        ${CMAKE_CURRENT_LIST_DIR}/openssl/openssl_group.h
        ${CMAKE_CURRENT_LIST_DIR}/toy/common.h
        ${CMAKE_CURRENT_LIST_DIR}/toy/montgomery.h
        ${CMAKE_CURRENT_LIST_DIR}/toy/weierstrass.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yacl/crypto/ecc)

# ------------------------------------------------------------------------------
# For libyacl_spi_ecc
# ------------------------------------------------------------------------------

add_library(
  yacl_spi_ecc OBJECT
  ${CMAKE_CURRENT_LIST_DIR}/curve_meta.cc
  ${CMAKE_CURRENT_LIST_DIR}/ec_point.cc
  ${CMAKE_CURRENT_LIST_DIR}/ecc_spi.cc
  ${CMAKE_CURRENT_LIST_DIR}/group_sketch.cc
  ${CMAKE_CURRENT_LIST_DIR}/FourQlib/FourQ_factory.cc
  ${CMAKE_CURRENT_LIST_DIR}/FourQlib/FourQ_group.cc
  # FIXME ecc with lib25519 ${CMAKE_CURRENT_LIST_DIR}/lib25519/ed25519_group.cc
  # ${CMAKE_CURRENT_LIST_DIR}/lib25519/lib25519_factory.cc
  # ${CMAKE_CURRENT_LIST_DIR}/lib25519/lib25519_group.cc FIXME ecc with
  # libsodium ${CMAKE_CURRENT_LIST_DIR}/libsodium/ed25519_group.cc
  # ${CMAKE_CURRENT_LIST_DIR}/libsodium/x25519_group.cc
  # ${CMAKE_CURRENT_LIST_DIR}/libsodium/sodium_factory.cc
  # ${CMAKE_CURRENT_LIST_DIR}/libsodium/sodium_grop.cc
  ${CMAKE_CURRENT_LIST_DIR}/mcl/mcl_ec_factory.cc
  ${CMAKE_CURRENT_LIST_DIR}/mcl/mcl_ec_group.cc
  ${CMAKE_CURRENT_LIST_DIR}/openssl/openssl_factory.cc
  ${CMAKE_CURRENT_LIST_DIR}/openssl/openssl_group.cc
  ${CMAKE_CURRENT_LIST_DIR}/toy/common.cc
  ${CMAKE_CURRENT_LIST_DIR}/toy/factory.cc
  ${CMAKE_CURRENT_LIST_DIR}/toy/montgomery.cc
  ${CMAKE_CURRENT_LIST_DIR}/toy/weierstrass.cc)

target_include_directories(
  yacl_spi_ecc PUBLIC ${CMAKE_DEPS_INCLUDEDIR}
                      "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>")

# NOTE yacl_spi_ecc requires yacl_spi_mpint
target_link_libraries(yacl_spi_ecc PRIVATE yacl_obj yacl_spi_mpint)

target_link_libraries(yacl PUBLIC $<TARGET_OBJECTS:yacl_spi_ecc>)
add_dependencies(yacl yacl_spi_ecc)

# NOTE MCL_ALL_PAIRING_FOR_YACL need to be defined in: 1. ecc_spi 2.
# galois_field_spi 3. paring_spi
target_compile_definitions(yacl_spi_ecc PUBLIC MCL_ALL_PAIRING_FOR_YACL)

# ------------
# Test
# ------------

add_yacl_test(ecc_test)
