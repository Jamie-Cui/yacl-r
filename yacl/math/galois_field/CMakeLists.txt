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
                      ${CMAKE_CURRENT_LIST_DIR}/gf_intrinsic.cc)

# Add header files for installation NOTE no install for factory folder files
install(FILES ${CMAKE_CURRENT_LIST_DIR}/gf.h
              ${CMAKE_CURRENT_LIST_DIR}/gf_intrinsic.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yacl/math/galois_field)

set(YACL_SOURCE_FILES
    ${YACL_SOURCE_FILES}
    PARENT_SCOPE)

# ------------------------------------------------------------------------------
# For libspi_galois_field
# ------------------------------------------------------------------------------

add_library(
  yacl_spi_galois_field OBJECT
  ${CMAKE_CURRENT_LIST_DIR}/factory/gf_spi.cc
  ${CMAKE_CURRENT_LIST_DIR}/factory/mcl_factory.cc
  # ${CMAKE_CURRENT_LIST_DIR}/factory/intel_factory.cc
  ${CMAKE_CURRENT_LIST_DIR}/factory/mpint_factory.cc)

target_include_directories(
  yacl_spi_galois_field PUBLIC ${CMAKE_DEPS_INCLUDEDIR}
                               "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>")

# NOTE libyacl_spi_galois_field requires libyacl_spi_mpint
target_link_libraries(yacl_spi_galois_field PRIVATE yacl_obj yacl_spi_mpint)

# NOTE MCL_ALL_PAIRING_FOR_YACL need to be defined in:
# 1. ecc_spi
# 2. galois_field_spi
# 3. paring_spi
target_compile_definitions(yacl_spi_galois_field
                           PUBLIC MCL_ALL_PAIRING_FOR_YACL)

list(APPEND YACL_SPI_LIBS yacl_spi_galois_field)

set(YACL_SPI_LIBS
    ${YACL_SPI_LIBS}
    PARENT_SCOPE)

# ------------------------------------------------------------------------------
# For tests
# ------------------------------------------------------------------------------

add_yacl_test_if(gf_test)
add_yacl_test_if(gf_intrinsic_test)
# add_yacl_test_if(mpint_factory_test) add_yacl_test_if(mcl_factory_test)
# add_yacl_test_if(intel_factory_test)
