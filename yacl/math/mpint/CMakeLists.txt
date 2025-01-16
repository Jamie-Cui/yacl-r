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
  FILES ${CMAKE_CURRENT_LIST_DIR}/montgomery_math.h
        ${CMAKE_CURRENT_LIST_DIR}/mp_int.h
        ${CMAKE_CURRENT_LIST_DIR}/mp_int_enforce.h
        ${CMAKE_CURRENT_LIST_DIR}/mp_int_private.h
        ${CMAKE_CURRENT_LIST_DIR}/tommath_ext_types.h
        ${CMAKE_CURRENT_LIST_DIR}/tommath_ext_features.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yacl/math/mpint)

add_yacl_test(montgomery_math_test)
add_yacl_test(mp_int_test)
add_yacl_test(tommath_ext_test)

# ------------------------------------------------------------------------------
# For libyacl_spi_mpint
# ------------------------------------------------------------------------------

add_library(yacl_spi_mpint SHARED
    ${CMAKE_CURRENT_LIST_DIR}/montgomery_math.cc
    ${CMAKE_CURRENT_LIST_DIR}/mp_int.cc
    ${CMAKE_CURRENT_LIST_DIR}/tommath_ext_types.cc
    ${CMAKE_CURRENT_LIST_DIR}/tommath_ext_features.cc)

target_include_directories(yacl_spi_mpint PUBLIC
  ${GMP_INCLUDE_DIR}
  ${CMAKE_THIRDPARTY_INCLUDEDIR}
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>")

# NOTE libyacl_spi_mpint requires gmp
target_link_libraries(yacl_spi_mpint
  PRIVATE
  yacl
  yacl_spi_utils
  ${GMP_LIBRARIES})

target_link_libraries(yacl_all INTERFACE yacl_spi_mpint)
