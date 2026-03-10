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

option(BUILD_FUZZ "Build fuzzing targets (requires Clang with libFuzzer)" Off)

if(BUILD_FUZZ)
  if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(
      FATAL_ERROR
        "BUILD_FUZZ requires Clang (libFuzzer). Current compiler: ${CMAKE_CXX_COMPILER_ID}"
    )
  endif()

  set(YACL_FUZZ_FLAGS "-fsanitize=fuzzer,address,undefined" "-fno-omit-frame-pointer")
  message(STATUS "Fuzzing enabled with flags: ${YACL_FUZZ_FLAGS}")
endif()

macro(add_yacl_fuzz NAME)
  add_executable(${NAME} ${NAME}.cc)
  target_link_libraries(${NAME} PRIVATE Yacl::yacl)
  target_compile_options(${NAME} PRIVATE ${YACL_FUZZ_FLAGS})
  target_link_options(${NAME} PRIVATE ${YACL_FUZZ_FLAGS})
endmacro()

macro(add_yacl_fuzz_if NAME)
  if(BUILD_FUZZ)
    add_yacl_fuzz(${NAME})
  endif()
endmacro()
