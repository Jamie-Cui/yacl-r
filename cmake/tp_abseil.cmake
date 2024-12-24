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

ExternalProject_Add(abseil
  URL
    "https://github.com/abseil/abseil-cpp/archive/refs/tags/\
20240722.0.tar.gz"
  URL_HASH
    SHA256=f50e5ac311a81382da7fa75b97310e4b9006474f9560ac46f54a9967f07d4ae3
  CMAKE_ARGS ${CMAKE_ARGS}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_THIRDPARTY_PREFIX}
    # -DABSL_BUILD_MONOLITHIC_SHARED_LIBS=On
    # -DBUILD_SHARED_LIBS=On
  PREFIX ${CMAKE_THIRDPARTY_PREFIX}
  LOG_DOWNLOAD On
  LOG_CONFIGURE On
  LOG_BUILD On
  LOG_INSTALL On)

# ------------------------------------------------------------------------------
# How to use absl?
# ------------------------------------------------------------------------------

# add_library(libabseil_dll SHARED IMPORTED)
# set_property(TARGET libabseil_dll
#     PROPERTY IMPORTED_LOCATION "${CMAKE_THRIDPARTY_LIB_DIRECOTRY}/libabseil_dll${CMAKE_SHARED_LIBRARY_SUFFIX}")
# add_dependencies(libabseil_dll abseil-cpp)

add_library(libabsl_interface INTERFACE)

# HACK
macro(import_absl_static_lib name)
  add_library(${name} STATIC IMPORTED)
  set_property(
    TARGET ${name}
    PROPERTY IMPORTED_LOCATION ${CMAKE_THRIDPARTY_LIB_DIRECOTRY}/${name}${CMAKE_STATIC_LIBRARY_SUFFIX})
  add_dependencies(${name} abseil-cpp)
  target_link_libraries(libabsl_interface
    INTERFACE
    ${name})
endmacro()

import_absl_static_lib(libabsl_raw_hash_set)
import_absl_static_lib(libabsl_hash)
import_absl_static_lib(libabsl_city)
import_absl_static_lib(libabsl_low_level_hash)
import_absl_static_lib(libabsl_random_internal_pool_urbg)
import_absl_static_lib(libabsl_random_internal_randen)
import_absl_static_lib(libabsl_random_internal_randen_hwaes)
import_absl_static_lib(libabsl_random_internal_randen_hwaes_impl)
import_absl_static_lib(libabsl_random_internal_randen_slow)
import_absl_static_lib(libabsl_random_internal_platform)
import_absl_static_lib(libabsl_random_internal_seed_material)
import_absl_static_lib(libabsl_random_seed_gen_exception)
import_absl_static_lib(libabsl_statusor)
import_absl_static_lib(libabsl_status)
import_absl_static_lib(libabsl_cord)
import_absl_static_lib(libabsl_cordz_info)
import_absl_static_lib(libabsl_cord_internal)
import_absl_static_lib(libabsl_cordz_functions)
import_absl_static_lib(libabsl_exponential_biased)
import_absl_static_lib(libabsl_cordz_handle)
import_absl_static_lib(libabsl_strerror)
import_absl_static_lib(libabsl_str_format_internal)
import_absl_static_lib(libabsl_synchronization)
import_absl_static_lib(libabsl_stacktrace)
import_absl_static_lib(libabsl_symbolize)
import_absl_static_lib(libabsl_malloc_internal)
import_absl_static_lib(libabsl_time)
import_absl_static_lib(libabsl_strings)
import_absl_static_lib(libabsl_strings_internal)
import_absl_static_lib(libabsl_base)
import_absl_static_lib(libabsl_spinlock_wait)
import_absl_static_lib(libabsl_int128)
import_absl_static_lib(libabsl_throw_delegate)
import_absl_static_lib(libabsl_time_zone)
import_absl_static_lib(libabsl_raw_logging_internal)
import_absl_static_lib(libabsl_debugging_internal)
import_absl_static_lib(libabsl_demangle_internal)
import_absl_static_lib(libabsl_demangle_rust)
import_absl_static_lib(libabsl_decode_rust_punycode)
import_absl_static_lib(libabsl_utf8_for_code_point)

# -----------------------------
# Alias Target for External Use
# -----------------------------
add_library(Thirdparty::absl ALIAS libabsl_interface)
