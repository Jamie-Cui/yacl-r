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

#include <vector>

#include "yacl/engine/snark/r1cs.h"
#include "yacl/math/mpint/mp_int.h"

namespace yacl::engine::snark {

// Polynomial in coefficient form: poly[i] is the coefficient of x^i.
using Poly = std::vector<math::MPInt>;

// Polynomial arithmetic operations mod a prime modulus.
namespace poly_ops {

Poly Add(const Poly& a, const Poly& b, const math::MPInt& mod);
Poly Sub(const Poly& a, const Poly& b, const math::MPInt& mod);
Poly Mul(const Poly& a, const Poly& b, const math::MPInt& mod);
Poly ScalarMul(const Poly& a, const math::MPInt& s, const math::MPInt& mod);
math::MPInt Evaluate(const Poly& p, const math::MPInt& x,
                     const math::MPInt& mod);

// Build polynomial with roots at the given points: product of (x - root_i).
Poly FromRoots(const std::vector<math::MPInt>& roots, const math::MPInt& mod);

// Polynomial division: returns {quotient, remainder} such that a = q*b + r.
std::pair<Poly, Poly> DivMod(const Poly& a, const Poly& b,
                             const math::MPInt& mod);

}  // namespace poly_ops

// QAP (Quadratic Arithmetic Program) representation.
// For n variables and m constraints, u[i], v[i], w[i] are polynomials
// for variable i, and t(x) is the target polynomial.
struct QapSystem {
  std::vector<Poly> u;  // u[i](x) for each variable i
  std::vector<Poly> v;  // v[i](x) for each variable i
  std::vector<Poly> w;  // w[i](x) for each variable i
  Poly t;               // target polynomial t(x) = prod(x - r_j)
};

// Convert an R1CS system to a QAP using Lagrange interpolation over
// domain {1, 2, ..., m} where m = number of constraints.
QapSystem R1csToQap(const R1csSystem& r1cs, const math::MPInt& field_order);

}  // namespace yacl::engine::snark
