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

message(STATUS "Downloading simplest_ot")

FetchContent_Declare(
  simplest_ot
  URL https://github.com/secretflow/simplest-ot/archive/60197bc7dad327bb55759e8e854885411e999167.tar.gz
  URL_HASH
    SHA256=c8816bf147e320f51c516f4c511f2d1a732ac0d0f171d29f442cbe2b5173ddba
  SOURCE_DIR ${CMAKE_DEPS_SRCDIR}/simplest_ot)

message(STATUS "Downloading simplest_ot - Success")

FetchContent_GetProperties(simplest_ot)

if(NOT simplest_ot_POPULATED)
  FetchContent_Populate(simplest_ot)
endif()

# Build portable simplest ot just for now
#
# TODO(jamie) build x86 version on x86 machines
add_library(
  libsimplest_ot STATIC
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/crypto_hash.h
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/fe.h
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/fe.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/fe_0.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/fe_1.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/fe_add.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/fe_cmov.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/fe_copy.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/fe_frombytes.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/fe_invert.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/fe_isnegative.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/fe_isnonzero.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/fe_mul.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/fe_neg.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/fe_pow22523.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/fe_sq.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/fe_sq2.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/fe_sub.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/fe_tobytes.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge.h
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge_add.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge_add.h
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge_double_scalarmult.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge_frombytes.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge_madd.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge_madd.h
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge_msub.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge_msub.h
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge_p1p1_to_p2.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge_p1p1_to_p3.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge_p2_0.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge_p2_dbl.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge_p2_dbl.h
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge_p2_dbl.q
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge_p3_0.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge_p3_dbl.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge_p3_to_cached.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge_p3_to_p2.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge_p3_tobytes.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge_precomp_0.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge_scalarmult_base.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge_sub.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge_sub.h
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ge_tobytes.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/Keccak-simple-settings.h
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/Keccak-simple.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ot_config.h
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ot_receiver.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ot_receiver.h
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ot_sender.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/ot_sender.h
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/randombytes.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/randombytes.h
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/sc.h
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/sc_muladd.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/sc_random.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/sc_reduce.c
  ${CMAKE_DEPS_SRCDIR}/simplest_ot/simplest_ot_portable/verify.c)

target_include_directories(libsimplest_ot
                           PUBLIC ${CMAKE_DEPS_SRCDIR}/simplest_ot)

# -----------------------------
# Alias Target for External Use
# -----------------------------
add_library(Deps::simplest_ot ALIAS libsimplest_ot)
