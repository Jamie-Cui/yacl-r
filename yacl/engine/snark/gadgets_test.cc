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

#include "yacl/engine/snark/gadgets.h"

#include "gtest/gtest.h"

namespace yacl::engine::snark {
namespace {

const math::MPInt kMod(97);

TEST(GadgetsTest, BooleanConstraint) {
  CircuitBuilder cb;
  auto b = gadgets::Boolean(cb);

  auto r1cs = cb.Build();

  // b=0 should satisfy
  {
    std::map<Variable, math::MPInt> assignments;
    assignments[b] = math::MPInt(0);
    auto witness = cb.ComputeWitness(assignments, kMod);
    EXPECT_TRUE(r1cs.IsSatisfied(witness, kMod));
  }

  // b=1 should satisfy
  {
    std::map<Variable, math::MPInt> assignments;
    assignments[b] = math::MPInt(1);
    auto witness = cb.ComputeWitness(assignments, kMod);
    EXPECT_TRUE(r1cs.IsSatisfied(witness, kMod));
  }

  // b=2 should NOT satisfy
  {
    std::map<Variable, math::MPInt> assignments;
    assignments[b] = math::MPInt(2);
    auto witness = cb.ComputeWitness(assignments, kMod);
    EXPECT_FALSE(r1cs.IsSatisfied(witness, kMod));
  }
}

TEST(GadgetsTest, BoolAnd) {
  CircuitBuilder cb;
  auto a = gadgets::Boolean(cb);
  auto b = gadgets::Boolean(cb);
  auto c = gadgets::BoolAnd(cb, a, b);

  auto r1cs = cb.Build();

  // 1 AND 1 = 1
  {
    std::map<Variable, math::MPInt> assignments;
    assignments[a] = math::MPInt(1);
    assignments[b] = math::MPInt(1);
    assignments[c] = math::MPInt(1);
    auto witness = cb.ComputeWitness(assignments, kMod);
    EXPECT_TRUE(r1cs.IsSatisfied(witness, kMod));
  }

  // 1 AND 0 = 0
  {
    std::map<Variable, math::MPInt> assignments;
    assignments[a] = math::MPInt(1);
    assignments[b] = math::MPInt(0);
    assignments[c] = math::MPInt(0);
    auto witness = cb.ComputeWitness(assignments, kMod);
    EXPECT_TRUE(r1cs.IsSatisfied(witness, kMod));
  }
}

TEST(GadgetsTest, SelectGadget) {
  CircuitBuilder cb;
  auto cond = gadgets::Boolean(cb);
  auto a = cb.AllocPrivate();
  auto b = cb.AllocPrivate();
  auto result = gadgets::Select(cb, cond, a, b);

  auto r1cs = cb.Build();

  // cond=1: result should be a=10
  {
    std::map<Variable, math::MPInt> assignments;
    assignments[cond] = math::MPInt(1);
    assignments[a] = math::MPInt(10);
    assignments[b] = math::MPInt(20);
    // select = cond*(a-b)+b = 1*(-10)+20 = 10
    assignments[result] = math::MPInt(10);
    auto witness = cb.ComputeWitness(assignments, kMod);
    EXPECT_TRUE(r1cs.IsSatisfied(witness, kMod));
  }

  // cond=0: result should be b=20
  {
    std::map<Variable, math::MPInt> assignments;
    assignments[cond] = math::MPInt(0);
    assignments[a] = math::MPInt(10);
    assignments[b] = math::MPInt(20);
    // select = 0*(-10)+20 = 20
    assignments[result] = math::MPInt(20);
    auto witness = cb.ComputeWitness(assignments, kMod);
    EXPECT_TRUE(r1cs.IsSatisfied(witness, kMod));
  }
}

}  // namespace
}  // namespace yacl::engine::snark
