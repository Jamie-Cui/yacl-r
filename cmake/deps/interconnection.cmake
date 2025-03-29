# Copyright 2025 Jamie Cui
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

message(STATUS "Downloading interconnection")

FetchContent_Declare(
  interconnection
  URL https://github.com/secretflow/interconnection/archive/30e4220b7444d0bb077a9040f1b428632124e31a.tar.gz
  URL_HASH
    SHA256=341f6de0fa7dd618f9723009b9cb5b1da1788aacb9e12acfb0c9b19e5c5a7354
  SOURCE_DIR ${CMAKE_DEPS_SRCDIR}/interconnection)

message(STATUS "Downloading interconnection - Success")

FetchContent_GetProperties(interconnection)

if(NOT interconnection_POPULATED)
  FetchContent_Populate(interconnection)
endif()

add_library(
  interconnection_proto OBJECT
  ${CMAKE_DEPS_SRCDIR}/interconnection/interconnection/common/header.proto
  ${CMAKE_DEPS_SRCDIR}/interconnection/interconnection/link/transport.proto)

protobuf_generate(
  LANGUAGE
  cpp
  TARGET
  interconnection_proto
  IMPORT_DIRS
  ${CMAKE_DEPS_SRCDIR}/interconnection
  PROTOC_OUT_DIR
  ${CMAKE_DEPS_INCLUDEDIR})

target_include_directories(
  interconnection_proto PRIVATE
  ${CMAKE_DEPS_INCLUDEDIR})

target_link_libraries(interconnection_proto PUBLIC protobuf::libprotobuf)

# -----------------------------
# Alias Target for External Use
# -----------------------------
add_library(Deps::interconnection ALIAS interconnection_proto)
