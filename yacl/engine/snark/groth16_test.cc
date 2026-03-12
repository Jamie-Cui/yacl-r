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

#include "yacl/engine/snark/groth16.h"

#include "gtest/gtest.h"

#include "yacl/engine/snark/circuit_builder.h"
#include "yacl/engine/snark/serialization.h"

namespace yacl::engine::snark {
namespace {

// Test: x * x = y (proving knowledge of square root)
TEST(Groth16Test, SquareRoot) {
  // Build circuit: public y, private x, constraint x*x = y
  CircuitBuilder cb;
  auto y = cb.AllocPublic();   // public output
  auto x = cb.AllocPrivate();  // private input (the square root)
  auto x_sq = cb.Mul(x, x);
  cb.AssertEqual(x_sq, y);

  auto r1cs = cb.Build();

  // Setup
  auto [pk, vk] = snark::Setup(r1cs);

  // Prove with x=3, y=9
  std::map<Variable, math::MPInt> assignments;
  assignments[x] = math::MPInt(3);
  assignments[y] = math::MPInt(9);

  // Compute the multiplication result for the witness
  auto order = pk.pairing->GetOrder();
  assignments[x_sq] = math::MPInt(9);
  auto witness = cb.ComputeWitness(assignments, order);
  auto proof = Prove(pk, witness, r1cs);

  // Verify
  std::vector<math::MPInt> public_inputs = {math::MPInt(9)};
  EXPECT_TRUE(Verify(vk, public_inputs, proof));

  // Verify with wrong public input should fail
  std::vector<math::MPInt> wrong_inputs = {math::MPInt(10)};
  EXPECT_FALSE(Verify(vk, wrong_inputs, proof));
}

// Test: x^3 + x + 5 = 35, so x = 3
TEST(Groth16Test, CubicEquation) {
  CircuitBuilder cb;
  auto out = cb.AllocPublic();   // public output
  auto x = cb.AllocPrivate();    // private input

  // x^2
  auto x_sq = cb.Mul(x, x);
  // x^3
  auto x_cu = cb.Mul(x_sq, x);
  // x^3 + x
  auto sum = cb.Add(x_cu, x);
  // x^3 + x + 5
  auto result = cb.AddConst(sum, math::MPInt(5));

  cb.AssertEqual(result, out);

  auto r1cs = cb.Build();
  auto [pk, vk] = snark::Setup(r1cs);

  auto order = pk.pairing->GetOrder();

  // x=3: x^2=9, x^3=27, x^3+x+5=35
  std::map<Variable, math::MPInt> assignments;
  assignments[x] = math::MPInt(3);
  assignments[out] = math::MPInt(35);
  assignments[x_sq] = math::MPInt(9);
  assignments[x_cu] = math::MPInt(27);

  auto witness = cb.ComputeWitness(assignments, order);
  auto proof = Prove(pk, witness, r1cs);

  std::vector<math::MPInt> public_inputs = {math::MPInt(35)};
  EXPECT_TRUE(Verify(vk, public_inputs, proof));
}

// Test proof serialization round-trip
TEST(Groth16Test, ProofSerialization) {
  CircuitBuilder cb;
  auto y = cb.AllocPublic();
  auto x = cb.AllocPrivate();
  auto x_sq = cb.Mul(x, x);
  cb.AssertEqual(x_sq, y);

  auto r1cs = cb.Build();
  auto [pk, vk] = snark::Setup(r1cs);
  auto order = pk.pairing->GetOrder();

  std::map<Variable, math::MPInt> assignments;
  assignments[x] = math::MPInt(7);
  assignments[y] = math::MPInt(49);
  assignments[x_sq] = math::MPInt(49);

  auto witness = cb.ComputeWitness(assignments, order);
  auto proof = Prove(pk, witness, r1cs);

  // Serialize and deserialize
  auto buf = SerializeProof(proof, pk.pairing);
  auto proof2 = DeserializeProof(buf, pk.pairing);

  // Verify deserialized proof
  std::vector<math::MPInt> public_inputs = {math::MPInt(49)};
  EXPECT_TRUE(Verify(vk, public_inputs, proof2));
}

// Test VK serialization round-trip
TEST(Groth16Test, VkSerialization) {
  CircuitBuilder cb;
  auto y = cb.AllocPublic();
  auto x = cb.AllocPrivate();
  auto x_sq = cb.Mul(x, x);
  cb.AssertEqual(x_sq, y);

  auto r1cs = cb.Build();
  auto [pk, vk] = snark::Setup(r1cs);
  auto order = pk.pairing->GetOrder();

  std::map<Variable, math::MPInt> assignments;
  assignments[x] = math::MPInt(5);
  assignments[y] = math::MPInt(25);
  assignments[x_sq] = math::MPInt(25);

  auto witness = cb.ComputeWitness(assignments, order);
  auto proof = Prove(pk, witness, r1cs);

  // Serialize and deserialize VK
  auto vk_buf = SerializeVerificationKey(vk);
  auto vk2 = DeserializeVerificationKey(vk_buf, vk.pairing);

  std::vector<math::MPInt> public_inputs = {math::MPInt(25)};
  EXPECT_TRUE(Verify(vk2, public_inputs, proof));
}

}  // namespace
}  // namespace yacl::engine::snark
