# Copyright 2024 Jamie Cui
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

set(Protobuf_DIR "${CMAKE_DEPS_LIBDIR}/cmake/protobuf")

find_package(Protobuf PATHS ${CMAKE_DEPS_LIBDIR})

if(NOT Protobuf_FOUND OR (NOT Protobuf_VERSION VERSION_EQUAL 3.21.12.0))

  message(STATUS "Downloading Protobuf")

  FetchContent_Declare(
    protobuf
    URL https://github.com/protocolbuffers/protobuf/releases/download/v21.12/protobuf-all-21.12.tar.gz
    URL_HASH
      SHA256=2c6a36c7b5a55accae063667ef3c55f2642e67476d96d355ff0acb13dbb47f09
    SOURCE_DIR ${CMAKE_DEPS_SRCDIR}/protobuf)

  message(STATUS "Downloading Protobuf - Success")

  FetchContent_GetProperties(protobuf)

  if(NOT protobuf_POPULATED)
    FetchContent_Populate(protobuf)

    message(STATUS "Configuring Protobuf")
    execute_process(
      COMMAND mkdir -p ${CMAKE_DEPS_SRCDIR}/protobuf-stamp
      COMMAND
        cmake ${CMAKE_DEPS_SRCDIR}/protobuf
        -Dprotobuf_BUILD_PROTOBUF_BINARIES=Off #
        -Dprotobuf_BUILD_PROTOC_BINARIES=On #
        -Dprotobuf_BUILD_LIBPROTOC=On #
        -Dprotobuf_BUILD_TESTS=Off #
        -DBUILD_SHARED_LIBS=Off #
        -DCMAKE_INSTALL_PREFIX=${CMAKE_DEPS_PREFIX}
      OUTPUT_FILE ${CMAKE_DEPS_SRCDIR}/protobuf-stamp/protobuf-configure-out.log
      ERROR_FILE ${CMAKE_DEPS_SRCDIR}/protobuf-stamp/protobuf-configure-err.log
      WORKING_DIRECTORY ${CMAKE_DEPS_SRCDIR}/protobuf-build
      RESULT_VARIABLE PROTOBUF_CONFIGURE_FAIL)

    if(PROTOBUF_CONFIGURE_FAIL AND NOT PROTOBUF_CONFIGURE_FAIL EQUAL 0)
      message(FATAL_ERROR "Configuring Protobuf - Failed")
    else()
      message(STATUS "Configuring Protobuf - Success")
    endif()

    message(STATUS "Building Protobuf (this may takes a while)")
    execute_process(
      COMMAND cmake --build .
      # OUTPUT_FILE ${CMAKE_DEPS_SRCDIR}/protobuf-stamp/protobuf-build-out.log
      # ERROR_FILE ${CMAKE_DEPS_SRCDIR}/protobuf-stamp/protobuf-build-err.log
      WORKING_DIRECTORY ${CMAKE_DEPS_SRCDIR}/protobuf-build
      RESULT_VARIABLE PROTOBUF_BUILD_FAIL)

    if(PROTOBUF_BUILD_FAIL AND NOT PROTOBUF_BUILD_FAIL EQUAL 0)
      message(FATAL_ERROR "Building Protobuf - Failed")
    else()
      message(STATUS "Building Protobuf - Success")
    endif()

    message(STATUS "Installing Protobuf")
    execute_process(
      COMMAND cmake --install .
      OUTPUT_FILE ${CMAKE_DEPS_SRCDIR}/protobuf-stamp/protobuf-install-out.log
      ERROR_FILE ${CMAKE_DEPS_SRCDIR}/protobuf-stamp/protobuf-install-err.log
      WORKING_DIRECTORY ${CMAKE_DEPS_SRCDIR}/protobuf-build
      RESULT_VARIABLE PROTOBUF_INSTALL_FAIL)

    if(PROTOBUF_INSTLL_FAIL AND NOT PROTOBUF_INSTALL_FAIL EQUAL 0)
      message(FATAL_ERROR "Installing Protobuf - Failed")
    else()
      message(STATUS "Installing Protobuf - Success")
    endif()

    set(Protobuf_DIR "${CMAKE_DEPS_LIBDIR}/cmake/protobuf")
    find_package(Protobuf REQUIRED PATHS ${CMAKE_DEPS_LIBDIR})

    if(NOT Protobuf_FOUND OR (NOT Protobuf_VERSION VERSION_EQUAL 3.21.12.0))
      message(
        FATAL_ERROR
          "Somthing is wrong ... Protobuf_FOUND: ${Protobuf_FOUND}, and Protobuf version: ${Protobuf_VERSION}"
      )
    endif()
  endif()

else()
  message(STATUS "Protobuf found")
  message(STATUS "Protobuf version: ${Protobuf_VERSION}")
endif()

# ------------------------------------------------------------------------------
# How to use protobuf?
# ------------------------------------------------------------------------------

add_library(libprotobuf STATIC IMPORTED)
set_target_properties(
  libprotobuf
  PROPERTIES IMPORTED_LOCATION
             ${CMAKE_DEPS_LIBDIR}/libprotobuf${CMAKE_STATIC_LIBRARY_SUFFIX})

# -----------------------------
# Alias Target for External Use
# -----------------------------
add_library(Deps::protobuf ALIAS libprotobuf)
