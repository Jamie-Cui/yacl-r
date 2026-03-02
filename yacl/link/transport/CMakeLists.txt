# Copyright 2024 Ant Group Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Always include ASIO and basic channel implementations
set(YACL_SOURCE_FILES
    ${YACL_SOURCE_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/asio_link.cc
    ${CMAKE_CURRENT_LIST_DIR}/channel.cc
    ${CMAKE_CURRENT_LIST_DIR}/channel_mem.cc)

# brpc is optional
if(ENABLE_BRPC)
  # TODO(jamie): Add non-interconnection based brpc link implementation
  # Currently brpc link depends on interconnection which has been removed
endif()

# Header files for installation
set(LINK_TRANSPORT_HEADERS
    ${PROJECT_SOURCE_DIR}/include/yacl/link/transport/asio_link.h
    ${PROJECT_SOURCE_DIR}/include/yacl/link/transport/channel.h
    ${PROJECT_SOURCE_DIR}/include/yacl/link/transport/channel_mem.h)

if(ENABLE_BRPC)
  list(APPEND LINK_TRANSPORT_HEADERS
       # TODO(jamie): Add brpc link headers when implemented
  )
endif()

install(
  FILES ${LINK_TRANSPORT_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yacl/io/link)

set(YACL_SOURCE_FILES
    ${YACL_SOURCE_FILES}
    PARENT_SCOPE)
