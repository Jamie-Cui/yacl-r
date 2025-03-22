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

# ------------------------------------------------------------------------------
# For libyacl_spi_pairing.so
# ------------------------------------------------------------------------------

add_library(
  yacl_spi_pairing OBJECT
  ${CMAKE_CURRENT_LIST_DIR}/factory/mcl_bls12_381.cc
  ${CMAKE_CURRENT_LIST_DIR}/factory/mcl_factory.cc
  ${CMAKE_CURRENT_LIST_DIR}/factory/mcl_pairing_group.cc
  ${CMAKE_CURRENT_LIST_DIR}/factory/pairing_spi.cc)

target_include_directories(
  yacl_spi_pairing PUBLIC ${CMAKE_DEPS_INCLUDEDIR}
                          "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>")

target_link_libraries(
  yacl_spi_pairing
  PRIVATE yacl_obj yacl_spi_mpint # NOTE pairing need spi_mpint
          yacl_spi_galois_field # NOTE pairing need spi_galois_field
          yacl_spi_ecc # NOTE pairing need spi_ecc
)

# NOTE MCL_ALL_PAIRING_FOR_YACL need to be defined in: 1. ecc_spi 2.
# galois_field_spi 3. paring_spi
target_compile_definitions(yacl_spi_pairing PUBLIC MCL_ALL_PAIRING_FOR_YACL)

target_link_libraries(yacl PUBLIC $<TARGET_OBJECTS:yacl_spi_pairing>)
add_dependencies(yacl yacl_spi_pairing)

# ------------
# Test
# ------------

add_yacl_test(pairing_test)
