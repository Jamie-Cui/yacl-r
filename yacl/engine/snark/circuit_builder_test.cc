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

#include "yacl/engine/snark/circuit_builder.h"

#include "gtest/gtest.h"

namespace yacl::engine::snark {
namespace {

const math::MPInt kMod(97);

TEST(CircuitBuilderTest, SimpleMultiplication) {
  CircuitBuilder cb;
  auto x = cb.AllocPublic();
  auto y = cb.AllocPrivate();
  auto z = cb.Mul(x, y);

  auto r1cs = cb.Build();
  EXPECT_EQ(r1cs.GetNumConstraints(), 1);
  EXPECT_EQ(r1cs.GetNumPublicInputs(), 1);

  // x=3, y=5, z=15
  std::map<Variable, math::MPInt> assignments;
  assignments[x] = math::MPInt(3);
  assignments[y] = math::MPInt(5);
  assignments[z] = math::MPInt(15);

  auto witness = cb.ComputeWitness(assignments, kMod);
  EXPECT_TRUE(r1cs.IsSatisfied(witness, kMod));
}

TEST(CircuitBuilderTest, LinearOperations) {
  CircuitBuilder cb;
  auto a = cb.AllocPublic();
  auto b = cb.AllocPublic();

  (void)cb.Add(a, b);
  (void)cb.Sub(a, b);
  (void)cb.MulConst(a, math::MPInt(3));
  (void)cb.AddConst(b, math::MPInt(10));

  // No constraints should be generated for linear ops
  auto r1cs = cb.Build();
  EXPECT_EQ(r1cs.GetNumConstraints(), 0);
}

TEST(CircuitBuilderTest, AssertBoolean) {
  CircuitBuilder cb;
  auto x = cb.AllocPrivate();
  cb.AssertBoolean(x);

  auto r1cs = cb.Build();

  // x=0 should satisfy
  {
    std::map<Variable, math::MPInt> assignments;
    assignments[x] = math::MPInt(0);
    auto witness = cb.ComputeWitness(assignments, kMod);
    EXPECT_TRUE(r1cs.IsSatisfied(witness, kMod));
  }

  // x=1 should satisfy
  {
    std::map<Variable, math::MPInt> assignments;
    assignments[x] = math::MPInt(1);
    auto witness = cb.ComputeWitness(assignments, kMod);
    EXPECT_TRUE(r1cs.IsSatisfied(witness, kMod));
  }

  // x=2 should NOT satisfy
  {
    std::map<Variable, math::MPInt> assignments;
    assignments[x] = math::MPInt(2);
    auto witness = cb.ComputeWitness(assignments, kMod);
    EXPECT_FALSE(r1cs.IsSatisfied(witness, kMod));
  }
}

TEST(CircuitBuilderTest, AssertEqual) {
  CircuitBuilder cb;
  auto a = cb.AllocPrivate();
  auto b = cb.AllocPrivate();
  cb.AssertEqual(a, b);

  auto r1cs = cb.Build();

  // a=5, b=5 should satisfy
  {
    std::map<Variable, math::MPInt> assignments;
    assignments[a] = math::MPInt(5);
    assignments[b] = math::MPInt(5);
    auto witness = cb.ComputeWitness(assignments, kMod);
    EXPECT_TRUE(r1cs.IsSatisfied(witness, kMod));
  }

  // a=5, b=6 should NOT satisfy
  {
    std::map<Variable, math::MPInt> assignments;
    assignments[a] = math::MPInt(5);
    assignments[b] = math::MPInt(6);
    auto witness = cb.ComputeWitness(assignments, kMod);
    EXPECT_FALSE(r1cs.IsSatisfied(witness, kMod));
  }
}

TEST(CircuitBuilderTest, ChainedMultiplications) {
  // x * x = x_sq, x_sq * x = x_cu
  CircuitBuilder cb;
  auto x = cb.AllocPrivate();
  auto x_sq = cb.Mul(x, x);
  auto x_cu = cb.Mul(x_sq, x);

  auto r1cs = cb.Build();
  EXPECT_EQ(r1cs.GetNumConstraints(), 2);

  // x=4, x_sq=16, x_cu=64
  std::map<Variable, math::MPInt> assignments;
  assignments[x] = math::MPInt(4);
  assignments[x_sq] = math::MPInt(16);
  assignments[x_cu] = math::MPInt(64);

  auto witness = cb.ComputeWitness(assignments, kMod);
  EXPECT_TRUE(r1cs.IsSatisfied(witness, kMod));
}

}  // namespace
}  // namespace yacl::engine::snark
