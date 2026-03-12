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

#include <algorithm>

#include "yacl/base/exception.h"

namespace yacl::engine::snark {

namespace poly_ops {

namespace {

// Ensure polynomial has no trailing zero coefficients.
void Trim(Poly& p) {
  while (p.size() > 1 && p.back().IsZero()) {
    p.pop_back();
  }
}

// Negate a value mod p: returns (p - v) mod p.
math::MPInt NegMod(const math::MPInt& v, const math::MPInt& mod) {
  if (v.IsZero()) {
    return math::MPInt(0);
  }
  return mod - v.Mod(mod);
}

}  // namespace

Poly Add(const Poly& a, const Poly& b, const math::MPInt& mod) {
  int64_t max_len = std::max(a.size(), b.size());
  Poly result(max_len, math::MPInt(0));
  for (int64_t i = 0; i < static_cast<int64_t>(a.size()); ++i) {
    result[i] = a[i];
  }
  for (int64_t i = 0; i < static_cast<int64_t>(b.size()); ++i) {
    result[i] = result[i].AddMod(b[i], mod);
  }
  Trim(result);
  return result;
}

Poly Sub(const Poly& a, const Poly& b, const math::MPInt& mod) {
  int64_t max_len = std::max(a.size(), b.size());
  Poly result(max_len, math::MPInt(0));
  for (int64_t i = 0; i < static_cast<int64_t>(a.size()); ++i) {
    result[i] = a[i];
  }
  for (int64_t i = 0; i < static_cast<int64_t>(b.size()); ++i) {
    result[i] = result[i].SubMod(b[i], mod);
  }
  Trim(result);
  return result;
}

Poly Mul(const Poly& a, const Poly& b, const math::MPInt& mod) {
  if (a.empty() || b.empty()) {
    return {math::MPInt(0)};
  }
  int64_t result_size = static_cast<int64_t>(a.size() + b.size()) - 1;
  Poly result(result_size, math::MPInt(0));
  for (int64_t i = 0; i < static_cast<int64_t>(a.size()); ++i) {
    if (a[i].IsZero()) {
      continue;
    }
    for (int64_t j = 0; j < static_cast<int64_t>(b.size()); ++j) {
      math::MPInt term = a[i].MulMod(b[j], mod);
      result[i + j] = result[i + j].AddMod(term, mod);
    }
  }
  Trim(result);
  return result;
}

Poly ScalarMul(const Poly& a, const math::MPInt& s, const math::MPInt& mod) {
  Poly result(a.size(), math::MPInt(0));
  for (int64_t i = 0; i < static_cast<int64_t>(a.size()); ++i) {
    result[i] = a[i].MulMod(s, mod);
  }
  Trim(result);
  return result;
}

math::MPInt Evaluate(const Poly& p, const math::MPInt& x,
                     const math::MPInt& mod) {
  if (p.empty()) {
    return math::MPInt(0);
  }
  // Horner's method
  math::MPInt result = p.back();
  for (int64_t i = static_cast<int64_t>(p.size()) - 2; i >= 0; --i) {
    result = result.MulMod(x, mod).AddMod(p[i], mod);
  }
  return result;
}

Poly FromRoots(const std::vector<math::MPInt>& roots,
               const math::MPInt& mod) {
  Poly result = {math::MPInt(1)};
  for (const auto& root : roots) {
    // Multiply by (x - root)
    Poly factor = {NegMod(root, mod), math::MPInt(1)};
    result = Mul(result, factor, mod);
  }
  return result;
}

std::pair<Poly, Poly> DivMod(const Poly& a, const Poly& b,
                             const math::MPInt& mod) {
  YACL_ENFORCE(!b.empty() && !b.back().IsZero(), "Division by zero polynomial");

  if (a.size() < b.size()) {
    return {{math::MPInt(0)}, a};
  }

  Poly remainder = a;
  int64_t q_size = static_cast<int64_t>(a.size() - b.size()) + 1;
  Poly quotient(q_size, math::MPInt(0));

  math::MPInt b_lead_inv = b.back().InvertMod(mod);

  for (int64_t i = q_size - 1; i >= 0; --i) {
    math::MPInt coeff =
        remainder[i + static_cast<int64_t>(b.size()) - 1].MulMod(b_lead_inv,
                                                                  mod);
    quotient[i] = coeff;
    for (int64_t j = 0; j < static_cast<int64_t>(b.size()); ++j) {
      math::MPInt sub = coeff.MulMod(b[j], mod);
      remainder[i + j] = remainder[i + j].SubMod(sub, mod);
    }
  }

  // Remainder is in remainder[0..b.size()-2]
  Poly rem(remainder.begin(),
           remainder.begin() + static_cast<int64_t>(b.size()) - 1);
  Trim(quotient);
  Trim(rem);
  return {quotient, rem};
}

}  // namespace poly_ops

namespace {

// Lagrange interpolation that returns coefficient-form polynomial.
// Given points (x_i, y_i), returns polynomial p such that p(x_i) = y_i.
Poly LagrangeInterpolate(const std::vector<math::MPInt>& xs,
                         const std::vector<math::MPInt>& ys,
                         const math::MPInt& mod) {
  YACL_ENFORCE(xs.size() == ys.size());
  int64_t n = static_cast<int64_t>(xs.size());
  Poly result(n, math::MPInt(0));

  for (int64_t i = 0; i < n; ++i) {
    if (ys[i].IsZero()) {
      continue;
    }
    // Compute Lagrange basis polynomial L_i(x)
    Poly basis = {math::MPInt(1)};
    math::MPInt denom(1);
    for (int64_t j = 0; j < n; ++j) {
      if (i == j) {
        continue;
      }
      // basis *= (x - x_j)
      math::MPInt neg_xj;
      if (xs[j].IsZero()) {
        neg_xj = math::MPInt(0);
      } else {
        neg_xj = mod - xs[j].Mod(mod);
      }
      Poly factor = {neg_xj, math::MPInt(1)};
      basis = poly_ops::Mul(basis, factor, mod);
      // denom *= (x_i - x_j)
      denom = denom.MulMod(xs[i].SubMod(xs[j], mod), mod);
    }
    // L_i(x) = basis / denom, scaled by y_i
    math::MPInt scale = ys[i].MulMod(denom.InvertMod(mod), mod);
    basis = poly_ops::ScalarMul(basis, scale, mod);
    result = poly_ops::Add(result, basis, mod);
  }
  return result;
}

}  // namespace

QapSystem R1csToQap(const R1csSystem& r1cs, const math::MPInt& field_order) {
  int64_t m = r1cs.GetNumConstraints();
  int64_t n = r1cs.GetNumVariables();

  YACL_ENFORCE(m > 0, "R1CS must have at least one constraint");
  YACL_ENFORCE(n > 0, "R1CS must have at least one variable");

  QapSystem qap;
  qap.u.resize(n);
  qap.v.resize(n);
  qap.w.resize(n);

  // Domain points: {1, 2, ..., m}
  std::vector<math::MPInt> domain(m);
  for (int64_t j = 0; j < m; ++j) {
    domain[j] = math::MPInt(j + 1);
  }

  // Target polynomial t(x) = (x-1)(x-2)...(x-m)
  qap.t = poly_ops::FromRoots(domain, field_order);

  // For each variable i, interpolate u_i, v_i, w_i from constraint values
  for (int64_t i = 0; i < n; ++i) {
    std::vector<math::MPInt> u_vals(m, math::MPInt(0));
    std::vector<math::MPInt> v_vals(m, math::MPInt(0));
    std::vector<math::MPInt> w_vals(m, math::MPInt(0));

    for (int64_t j = 0; j < m; ++j) {
      const auto& constraint = r1cs.GetConstraints()[j];
      auto it_a = constraint.a.GetTerms().find(i);
      if (it_a != constraint.a.GetTerms().end()) {
        u_vals[j] = it_a->second.Mod(field_order);
      }
      auto it_b = constraint.b.GetTerms().find(i);
      if (it_b != constraint.b.GetTerms().end()) {
        v_vals[j] = it_b->second.Mod(field_order);
      }
      auto it_c = constraint.c.GetTerms().find(i);
      if (it_c != constraint.c.GetTerms().end()) {
        w_vals[j] = it_c->second.Mod(field_order);
      }
    }

    // Check if all values are zero (skip interpolation)
    bool all_u_zero = true;
    bool all_v_zero = true;
    bool all_w_zero = true;
    for (int64_t j = 0; j < m; ++j) {
      if (!u_vals[j].IsZero()) {
        all_u_zero = false;
      }
      if (!v_vals[j].IsZero()) {
        all_v_zero = false;
      }
      if (!w_vals[j].IsZero()) {
        all_w_zero = false;
      }
    }

    qap.u[i] = all_u_zero ? Poly{math::MPInt(0)}
                           : LagrangeInterpolate(domain, u_vals, field_order);
    qap.v[i] = all_v_zero ? Poly{math::MPInt(0)}
                           : LagrangeInterpolate(domain, v_vals, field_order);
    qap.w[i] = all_w_zero ? Poly{math::MPInt(0)}
                           : LagrangeInterpolate(domain, w_vals, field_order);
  }

  return qap;
}

}  // namespace yacl::engine::snark
