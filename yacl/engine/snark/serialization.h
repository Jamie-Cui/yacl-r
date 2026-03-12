// Copyright 2024 Jamie Cui
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "yacl/base/buffer.h"
#include "yacl/engine/snark/groth16.h"

namespace yacl::engine::snark {

// Serialize a Groth16 proof to a binary buffer.
Buffer SerializeProof(const Proof& proof,
                      const std::shared_ptr<crypto::PairingGroup>& pairing);

// Deserialize a Groth16 proof from a binary buffer.
Proof DeserializeProof(ByteContainerView buf,
                       const std::shared_ptr<crypto::PairingGroup>& pairing);

// Serialize a verification key to a binary buffer.
Buffer SerializeVerificationKey(const VerificationKey& vk);

// Deserialize a verification key from a binary buffer.
VerificationKey DeserializeVerificationKey(
    ByteContainerView buf, const std::shared_ptr<crypto::PairingGroup>& pairing);

}  // namespace yacl::engine::snark
