# Copyright 2026 Jamie Cui
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

# utils
add_subdirectory(utils)

# io
add_subdirectory(io)

# math
add_subdirectory(math)

# coding theory primitives
add_subdirectory(coding)

# link (networking)
add_subdirectory(link)

# other
add_subdirectory(snark)
add_subdirectory(ot)
add_subdirectory(mpc)
add_subdirectory(vole)
add_subdirectory(rand)
add_subdirectory(aead)
add_subdirectory(aes)
add_subdirectory(block_cipher)
add_subdirectory(dpf)
add_subdirectory(envelope)
add_subdirectory(hash)
add_subdirectory(hmac)
add_subdirectory(kem)
add_subdirectory(oprf)
add_subdirectory(pke)
add_subdirectory(sign)
add_subdirectory(theoretical_tools)
add_subdirectory(experimental)
add_subdirectory(tpre)
# add_subdirectory(vss)

set(YACL_SOURCE_FILES
    ${YACL_SOURCE_FILES}
    PARENT_SCOPE)

set(YACL_SPI_LIBS
    ${YACL_SPI_LIBS}
    PARENT_SCOPE)
