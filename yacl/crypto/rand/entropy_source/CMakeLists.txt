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
  FILES ${CMAKE_CURRENT_LIST_DIR}/entropy_source.h
        ${CMAKE_CURRENT_LIST_DIR}/urandom_factory.h
        ${CMAKE_CURRENT_LIST_DIR}/rdseed_factory.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yacl/crypto/rand/entropy_source)

add_yacl_test(entropy_source_test)

# ------------------------------------------------------------------------------
# For libyacl_spi_entropy_source.so
# ------------------------------------------------------------------------------

add_library(yacl_spi_entropy_source SHARED
    ${CMAKE_CURRENT_LIST_DIR}/urandom_factory.cc
    ${CMAKE_CURRENT_LIST_DIR}/rdseed_factory.cc)

target_include_directories(yacl_spi_entropy_source PUBLIC
  ${CMAKE_THIRDPARTY_INCLUDEDIR}
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>")

target_link_libraries(yacl_spi_entropy_source PRIVATE yacl yacl_spi_utils)

target_link_libraries(yacl_all INTERFACE yacl_spi_entropy_source)
