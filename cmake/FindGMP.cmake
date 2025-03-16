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
#
# Copyright 2006 Laurent Montel, <montel@kde.org>
# Copyright 2007, Francesco Biscani, <bluescarni@gmail.com>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# ------------------------------------------------------------------------------------------

# Try to find the GMP libraries:
# GMP_FOUND - System has GMP lib
# GMP_INCLUDE_DIR - The GMP include directory
# GMP_LIBRARIES - Libraries needed to use GMP

if (GMP_INCLUDE_DIR AND GMP_LIBRARIES)
	# Force search at every time, in case configuration changes
	unset(GMP_INCLUDE_DIR CACHE)
	unset(GMP_LIBRARIES CACHE)
endif (GMP_INCLUDE_DIR AND GMP_LIBRARIES)

find_path(GMP_INCLUDE_DIR gmp.h
	PATHS
		ENV GMP_ROOT
		ENV GMP_INCLUDE_DIR
		${GMP_ROOT}
		/usr
		/usr/local
		$ENV{HOME}/.local
	PATH_SUFFIXES
		include
)

if(STBIN)
	find_library(GMP_LIBRARIES NAMES libgmp.a libgmp.dll.a gmp.lib libgmp-10 libgmp gmp)
else(STBIN)
	find_library(GMP_LIBRARIES NAMES libgmp.so libgmp.dll.so gmp.lib libgmp-10 libgmp gmp)
endif(STBIN)

if(GMP_INCLUDE_DIR AND GMP_LIBRARIES)
   set(GMP_FOUND TRUE)
endif()

if(GMP_FOUND)
	message(STATUS "Configured GMP: -I${GMP_INCLUDE_DIR} -L${GMP_LIBRARIES}")
	add_library(libgmp STATIC IMPORTED)
	target_include_directories(libgmp INTERFACE ${GMP_INCLUDE_DIR})
	set_target_properties(libgmp PROPERTIES IMPORTED_LOCATION ${GMP_LIBRARIES})
	add_library(Deps::gmp ALIAS libgmp)
else(GMP_FOUND)
	message(STATUS "Could NOT find GMP!")
endif(GMP_FOUND)

mark_as_advanced(GMP_INCLUDE_DIR GMP_LIBRARIES)
