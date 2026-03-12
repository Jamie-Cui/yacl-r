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

#include "yacl/engine/snark/r1cs.h"

#include "gtest/gtest.h"

namespace yacl::engine::snark {
namespace {

// A simple prime modulus for testing.
const math::MPInt kTestMod(97);

TEST(LinearCombinationTest, EvaluateSimple) {
  // LC = 3*x0 + 5*x1
  LinearCombination lc;
  lc.AddTerm(0, math::MPInt(3));
  lc.AddTerm(1, math::MPInt(5));

  // witness = [2, 7]
  std::vector<math::MPInt> witness = {math::MPInt(2), math::MPInt(7)};
  auto result = lc.Evaluate(witness, kTestMod);
  // 3*2 + 5*7 = 6 + 35 = 41
  EXPECT_EQ(result, math::MPInt(41));
}

TEST(LinearCombinationTest, CancellingTerms) {
  LinearCombination lc;
  lc.AddTerm(0, math::MPInt(5));
  lc.AddTerm(0, math::MPInt(-5));
  EXPECT_TRUE(lc.IsEmpty());
}

TEST(R1csSystemTest, SimpleMultiplicationConstraint) {
  // x * y = z, where x=3, y=5, z=15
  // Variables: [one=1, x, y, z]
  R1csSystem r1cs;
  r1cs.SetNumVariables(4);
  r1cs.SetNumPublicInputs(0);

  R1csConstraint c;
  c.a.AddTerm(1, math::MPInt(1));  // a = x
  c.b.AddTerm(2, math::MPInt(1));  // b = y
  c.c.AddTerm(3, math::MPInt(1));  // c = z

  r1cs.AddConstraint(std::move(c));

  // Witness: [1, 3, 5, 15]
  std::vector<math::MPInt> witness = {math::MPInt(1), math::MPInt(3),
                                      math::MPInt(5), math::MPInt(15)};
  EXPECT_TRUE(r1cs.IsSatisfied(witness, kTestMod));

  // Wrong witness: z=16
  std::vector<math::MPInt> bad_witness = {math::MPInt(1), math::MPInt(3),
                                          math::MPInt(5), math::MPInt(16)};
  EXPECT_FALSE(r1cs.IsSatisfied(bad_witness, kTestMod));
}

TEST(R1csSystemTest, MultipleConstraints) {
  // Constraint 1: x * x = x_sq
  // Constraint 2: x_sq * x = x_cu
  // Variables: [one, x, x_sq, x_cu]
  R1csSystem r1cs;
  r1cs.SetNumVariables(4);
  r1cs.SetNumPublicInputs(1);  // x is public

  R1csConstraint c1;
  c1.a.AddTerm(1, math::MPInt(1));
  c1.b.AddTerm(1, math::MPInt(1));
  c1.c.AddTerm(2, math::MPInt(1));
  r1cs.AddConstraint(std::move(c1));

  R1csConstraint c2;
  c2.a.AddTerm(2, math::MPInt(1));
  c2.b.AddTerm(1, math::MPInt(1));
  c2.c.AddTerm(3, math::MPInt(1));
  r1cs.AddConstraint(std::move(c2));

  // x=4, x_sq=16, x_cu=64
  std::vector<math::MPInt> witness = {math::MPInt(1), math::MPInt(4),
                                      math::MPInt(16), math::MPInt(64)};
  EXPECT_TRUE(r1cs.IsSatisfied(witness, kTestMod));
}

TEST(R1csSystemTest, WithConstantWire) {
  // (x + 5) * 1 = y, i.e., x + 5 = y
  // Variables: [one, x, y]
  R1csSystem r1cs;
  r1cs.SetNumVariables(3);
  r1cs.SetNumPublicInputs(1);

  R1csConstraint c;
  c.a.AddTerm(1, math::MPInt(1));  // x
  c.a.AddTerm(0, math::MPInt(5));  // + 5*one
  c.b.AddTerm(0, math::MPInt(1));  // * 1
  c.c.AddTerm(2, math::MPInt(1));  // = y
  r1cs.AddConstraint(std::move(c));

  // x=10, y=15
  std::vector<math::MPInt> witness = {math::MPInt(1), math::MPInt(10),
                                      math::MPInt(15)};
  EXPECT_TRUE(r1cs.IsSatisfied(witness, kTestMod));
}

}  // namespace
}  // namespace yacl::engine::snark
