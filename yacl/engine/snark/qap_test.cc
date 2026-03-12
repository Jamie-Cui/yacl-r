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

#include "yacl/engine/snark/qap.h"

#include "gtest/gtest.h"

namespace yacl::engine::snark {
namespace {

const math::MPInt kMod(97);

TEST(PolyOpsTest, AddSubBasic) {
  Poly a = {math::MPInt(3), math::MPInt(5)};
  Poly b = {math::MPInt(1), math::MPInt(2), math::MPInt(4)};

  auto sum = poly_ops::Add(a, b, kMod);
  ASSERT_EQ(sum.size(), 3u);
  EXPECT_EQ(sum[0], math::MPInt(4));
  EXPECT_EQ(sum[1], math::MPInt(7));
  EXPECT_EQ(sum[2], math::MPInt(4));

  auto diff = poly_ops::Sub(a, b, kMod);
  ASSERT_EQ(diff.size(), 3u);
  EXPECT_EQ(diff[0], math::MPInt(2));
  EXPECT_EQ(diff[1], math::MPInt(3));
  // -4 mod 97 = 93
  EXPECT_EQ(diff[2], math::MPInt(93));
}

TEST(PolyOpsTest, MulBasic) {
  // (2 + 3x) * (1 + x) = 2 + 5x + 3x^2
  Poly a = {math::MPInt(2), math::MPInt(3)};
  Poly b = {math::MPInt(1), math::MPInt(1)};

  auto product = poly_ops::Mul(a, b, kMod);
  ASSERT_EQ(product.size(), 3u);
  EXPECT_EQ(product[0], math::MPInt(2));
  EXPECT_EQ(product[1], math::MPInt(5));
  EXPECT_EQ(product[2], math::MPInt(3));
}

TEST(PolyOpsTest, EvaluateHorner) {
  // p(x) = 1 + 2x + 3x^2
  Poly p = {math::MPInt(1), math::MPInt(2), math::MPInt(3)};

  // p(0) = 1
  EXPECT_EQ(poly_ops::Evaluate(p, math::MPInt(0), kMod), math::MPInt(1));
  // p(1) = 1 + 2 + 3 = 6
  EXPECT_EQ(poly_ops::Evaluate(p, math::MPInt(1), kMod), math::MPInt(6));
  // p(2) = 1 + 4 + 12 = 17
  EXPECT_EQ(poly_ops::Evaluate(p, math::MPInt(2), kMod), math::MPInt(17));
}

TEST(PolyOpsTest, FromRoots) {
  // (x - 1)(x - 2) = x^2 - 3x + 2
  std::vector<math::MPInt> roots = {math::MPInt(1), math::MPInt(2)};
  auto p = poly_ops::FromRoots(roots, kMod);

  EXPECT_EQ(poly_ops::Evaluate(p, math::MPInt(1), kMod), math::MPInt(0));
  EXPECT_EQ(poly_ops::Evaluate(p, math::MPInt(2), kMod), math::MPInt(0));
  // p(3) = (3-1)(3-2) = 2
  EXPECT_EQ(poly_ops::Evaluate(p, math::MPInt(3), kMod), math::MPInt(2));
}

TEST(PolyOpsTest, DivModExact) {
  // (x^2 - 3x + 2) / (x - 1) = (x - 2), remainder 0
  Poly a = {math::MPInt(2), math::MPInt(94), math::MPInt(1)};  // 2 - 3x + x^2
  Poly b = {math::MPInt(96), math::MPInt(1)};                  // -1 + x

  auto [q, r] = poly_ops::DivMod(a, b, kMod);

  // q should be (x - 2) = [-2, 1] = [95, 1] mod 97
  ASSERT_EQ(q.size(), 2u);
  EXPECT_EQ(q[0], math::MPInt(95));
  EXPECT_EQ(q[1], math::MPInt(1));

  // Remainder should be zero
  for (const auto& coeff : r) {
    EXPECT_TRUE(coeff.IsZero());
  }
}

TEST(QapTest, SimpleR1csToQap) {
  // Single constraint: x * y = z
  // Variables: [one, x, y, z]
  R1csSystem r1cs;
  r1cs.SetNumVariables(4);
  r1cs.SetNumPublicInputs(0);

  R1csConstraint c;
  c.a.AddTerm(1, math::MPInt(1));
  c.b.AddTerm(2, math::MPInt(1));
  c.c.AddTerm(3, math::MPInt(1));
  r1cs.AddConstraint(std::move(c));

  auto qap = R1csToQap(r1cs, kMod);

  ASSERT_EQ(qap.u.size(), 4u);
  ASSERT_EQ(qap.v.size(), 4u);
  ASSERT_EQ(qap.w.size(), 4u);

  // Check that t(x) = (x - 1) since there's 1 constraint
  EXPECT_EQ(poly_ops::Evaluate(qap.t, math::MPInt(1), kMod), math::MPInt(0));
}

TEST(QapTest, QapSatisfaction) {
  // x * y = z, with x=3, y=5, z=15
  R1csSystem r1cs;
  r1cs.SetNumVariables(4);
  r1cs.SetNumPublicInputs(0);

  R1csConstraint c;
  c.a.AddTerm(1, math::MPInt(1));
  c.b.AddTerm(2, math::MPInt(1));
  c.c.AddTerm(3, math::MPInt(1));
  r1cs.AddConstraint(std::move(c));

  auto qap = R1csToQap(r1cs, kMod);

  std::vector<math::MPInt> witness = {math::MPInt(1), math::MPInt(3),
                                      math::MPInt(5), math::MPInt(15)};

  // Compute a(x), b(x), c(x)
  Poly a_poly = {math::MPInt(0)};
  Poly b_poly = {math::MPInt(0)};
  Poly c_poly = {math::MPInt(0)};
  for (int64_t i = 0; i < 4; ++i) {
    a_poly = poly_ops::Add(
        a_poly, poly_ops::ScalarMul(qap.u[i], witness[i], kMod), kMod);
    b_poly = poly_ops::Add(
        b_poly, poly_ops::ScalarMul(qap.v[i], witness[i], kMod), kMod);
    c_poly = poly_ops::Add(
        c_poly, poly_ops::ScalarMul(qap.w[i], witness[i], kMod), kMod);
  }

  // a(x)*b(x) - c(x) should be divisible by t(x)
  auto ab = poly_ops::Mul(a_poly, b_poly, kMod);
  auto diff = poly_ops::Sub(ab, c_poly, kMod);
  auto [h, rem] = poly_ops::DivMod(diff, qap.t, kMod);

  for (const auto& coeff : rem) {
    EXPECT_TRUE(coeff.IsZero());
  }
}

}  // namespace
}  // namespace yacl::engine::snark
