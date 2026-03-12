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

#include "yacl/crypto/pairing/pairing.h"
#include "yacl/engine/snark/r1cs.h"
#include "yacl/math/mpint/mp_int.h"

namespace yacl::engine::snark {

struct ProvingKey {
  crypto::EcPoint alpha_g1;
  crypto::EcPoint beta_g1;
  crypto::EcPoint beta_g2;
  crypto::EcPoint delta_g1;
  crypto::EcPoint delta_g2;
  std::vector<crypto::EcPoint> h_query;     // [tau^i * t(tau) / delta]_1
  std::vector<crypto::EcPoint> l_query;     // private input commitments
  std::vector<crypto::EcPoint> a_query;     // [u_i(tau)]_1 for all vars
  std::vector<crypto::EcPoint> b_g1_query;  // [v_i(tau)]_1 for all vars
  std::vector<crypto::EcPoint> b_g2_query;  // [v_i(tau)]_2 for all vars
  std::shared_ptr<crypto::PairingGroup> pairing;
};

struct VerificationKey {
  crypto::EcPoint alpha_g1;
  crypto::EcPoint beta_g2;
  crypto::EcPoint gamma_g2;
  crypto::EcPoint delta_g2;
  std::optional<crypto::GtElement> alpha_beta_gt;  // precomputed e(alpha, beta)
  std::vector<crypto::EcPoint> ic;                // public input commitments
  std::shared_ptr<crypto::PairingGroup> pairing;
};

struct Proof {
  crypto::EcPoint a;  // G1
  crypto::EcPoint b;  // G2
  crypto::EcPoint c;  // G1
};

struct SetupResult {
  ProvingKey pk;
  VerificationKey vk;
};

// Run the trusted setup. Generates proving and verification keys from the R1CS.
// pairing_name: name of the pairing curve (e.g., "bn_snark1", "bls12-381").
SetupResult Setup(const R1csSystem& r1cs,
                  const std::string& pairing_name = "bn_snark1");

// Generate a proof given the proving key and full witness vector.
// Witness layout: [1, public_1..public_l, private_1..private_k]
Proof Prove(const ProvingKey& pk, const std::vector<math::MPInt>& witness,
            const R1csSystem& r1cs);

// Verify a proof given the verification key and public inputs.
// public_inputs should NOT include the constant "one" wire.
bool Verify(const VerificationKey& vk,
            const std::vector<math::MPInt>& public_inputs, const Proof& proof);

}  // namespace yacl::engine::snark
