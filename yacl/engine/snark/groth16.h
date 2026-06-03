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

#include <memory>
#include <optional>
#include <vector>

#include "yacl/base/buffer.h"
#include "yacl/base/byte_container_view.h"
#include "yacl/engine/snark/r1cs.h"
#include "yacl/math/mpint/mp_int.h"
#include "yacl/math/pairing/pairing.h"

namespace yacl::engine::snark {

class Groth16 {
 public:
  struct ProvingKey {
    EcPoint alpha_g1;
    EcPoint beta_g1;
    EcPoint beta_g2;
    EcPoint delta_g1;
    EcPoint delta_g2;
    std::vector<EcPoint> h_query;     // [tau^i * t(tau) / delta]_1
    std::vector<EcPoint> l_query;     // private input commitments
    std::vector<EcPoint> a_query;     // [u_i(tau)]_1 for all vars
    std::vector<EcPoint> b_g1_query;  // [v_i(tau)]_1 for all vars
    std::vector<EcPoint> b_g2_query;  // [v_i(tau)]_2 for all vars
    std::shared_ptr<PairingGroup> pairing;
  };

  struct VerificationKey {
    EcPoint alpha_g1;
    EcPoint beta_g2;
    EcPoint gamma_g2;
    EcPoint delta_g2;
    // precomputed e(alpha, beta)
    std::optional<GtElement> alpha_beta_gt;
    std::vector<EcPoint> ic;  // public input commitments
    std::shared_ptr<PairingGroup> pairing;
  };

  struct Proof {
    EcPoint a;  // G1
    EcPoint b;  // G2
    EcPoint c;  // G1
  };

  struct SetupResult {
    ProvingKey pk;
    VerificationKey vk;
  };

  // Run the trusted setup. Generates proving and verification keys from the
  // R1CS. pairing_name: name of the pairing curve (e.g., "bn_snark1",
  // "bls12-381").
  static SetupResult Setup(const R1csSystem& r1cs,
                           const std::string& pairing_name = "bn_snark1");

  // Generate a proof given the proving key and full witness vector.
  // Witness layout: [1, public_1..public_l, private_1..private_k]
  static Proof Prove(const ProvingKey& pk,
                     const std::vector<math::MPInt>& witness,
                     const R1csSystem& r1cs);

  // Verify a proof given the verification key and public inputs.
  // public_inputs should NOT include the constant "one" wire.
  static bool Verify(const VerificationKey& vk,
                     const std::vector<math::MPInt>& public_inputs,
                     const Proof& proof);

  // Serialize and deserialize Groth16 proof material.
  static Buffer SerializeProof(const Proof& proof,
                               const std::shared_ptr<PairingGroup>& pairing);
  static Proof DeserializeProof(ByteContainerView buf,
                                const std::shared_ptr<PairingGroup>& pairing);

  // Serialize and deserialize Groth16 verification keys.
  static Buffer SerializeVerificationKey(const VerificationKey& vk);
  static VerificationKey DeserializeVerificationKey(
      ByteContainerView buf, const std::shared_ptr<PairingGroup>& pairing);
};

}  // namespace yacl::engine::snark
