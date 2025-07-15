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

macro(import_static_lib_from LIBNAME LIB)
  add_library(${LIBNAME} STATIC IMPORTED)
  set_target_properties(
    ${LIBNAME}
    PROPERTIES IMPORTED_LOCATION
               ${CMAKE_DEPS_LIBDIR}/${LIBNAME}${CMAKE_STATIC_LIBRARY_SUFFIX})
  add_dependencies(${LIBNAME} ${LIB})
endmacro()

macro(import_shared_lib_from LIBNAME LIB)
  add_library(${LIBNAME} SHARED IMPORTED)
  set_target_properties(
    ${LIBNAME}
    PROPERTIES IMPORTED_LOCATION
               ${CMAKE_DEPS_LIBDIR}/${LIBNAME}${CMAKE_SHARED_LIBRARY_SUFFIX})
  add_dependencies(${LIBNAME} ${LIB})
endmacro()
