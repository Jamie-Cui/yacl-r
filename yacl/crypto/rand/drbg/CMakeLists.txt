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
install(
  FILES ${CMAKE_CURRENT_LIST_DIR}/drbg.h
        ${CMAKE_CURRENT_LIST_DIR}/native_factory.h
        ${CMAKE_CURRENT_LIST_DIR}/openssl_factory.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yacl/crypto/rand/drbg)

set(YACL_SOURCE_FILES
    ${YACL_SOURCE_FILES}
    PARENT_SCOPE)

# ------------
# Test
# ------------

add_yacl_test(drbg_test)

# ------------------------------------------------------------------------------
# For libyacl_spi_drbg.so
# ------------------------------------------------------------------------------

add_library(yacl_spi_drbg OBJECT ${CMAKE_CURRENT_LIST_DIR}/native_factory.cc
                                 ${CMAKE_CURRENT_LIST_DIR}/openssl_factory.cc)

target_include_directories(
  yacl_spi_drbg PUBLIC ${CMAKE_DEPS_INCLUDEDIR}
                       "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>")

target_link_libraries(yacl_spi_drbg PRIVATE yacl_obj)

target_link_libraries(yacl PUBLIC $<TARGET_OBJECTS:yacl_spi_drbg>)
add_dependencies(yacl yacl_spi_drbg)
