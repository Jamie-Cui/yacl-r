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

file(GLOB_RECURSE SRC_PROTO *.proto)

set(YACL_SOURCE_FILES
    ${YACL_SOURCE_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/context.cc
    ${CMAKE_CURRENT_LIST_DIR}/factory_brpc.cc
    ${CMAKE_CURRENT_LIST_DIR}/factory_brpc_blackbox.cc
    ${CMAKE_CURRENT_LIST_DIR}/factory_mem.cc
    ${CMAKE_CURRENT_LIST_DIR}/trace.cc)

set(YACL_PROTO_SOURCE_FILES ${YACL_PROTO_SOURCE_FILES} ${SRC_PROTO})

add_subdirectory(algorithm)
add_subdirectory(transport)

set(YACL_SOURCE_FILES
    ${YACL_SOURCE_FILES}
    PARENT_SCOPE)
set(YACL_PROTO_SOURCE_FILES
    ${YACL_PROTO_SOURCE_FILES}
    PARENT_SCOPE)

add_yacl_test_if(context_test)
add_yacl_test_if(factory_test)

# Add header files for installation
install(
  FILES ${PROJECT_SOURCE_DIR}/include/yacl/link/context.h
        ${PROJECT_SOURCE_DIR}/include/yacl/link/factory_brpc.h
        ${PROJECT_SOURCE_DIR}/include/yacl/link/factory_mem.h
        ${PROJECT_SOURCE_DIR}/include/yacl/link/trace.h
        ${PROJECT_SOURCE_DIR}/include/yacl/link/link.h
        ${PROJECT_SOURCE_DIR}/include/yacl/link/retry_options.h
        ${PROJECT_SOURCE_DIR}/include/yacl/link/ssl_options.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yacl/link)
