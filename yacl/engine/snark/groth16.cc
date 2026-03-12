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

#include "yacl/base/exception.h"
#include "yacl/engine/snark/qap.h"

namespace yacl::engine::snark {

SetupResult Setup(const R1csSystem& r1cs, const std::string& pairing_name) {
  // Create pairing group
  auto pairing =
      crypto::PairingGroupFactory::Instance().Create(pairing_name);
  auto g1 = pairing->GetGroup1();
  auto g2 = pairing->GetGroup2();
  auto gt = pairing->GetGroupT();
  auto order = pairing->GetOrder();

  int64_t n = r1cs.GetNumVariables();
  int64_t l = r1cs.GetNumPublicInputs();  // number of public inputs (not
                                           // counting constant wire)
  int64_t m = r1cs.GetNumConstraints();

  YACL_ENFORCE(n > 0, "R1CS must have variables");
  YACL_ENFORCE(m > 0, "R1CS must have constraints");
  YACL_ENFORCE(l >= 0 && l < n, "Invalid number of public inputs");

  // Convert R1CS to QAP
  auto qap = R1csToQap(r1cs, order);

  // Sample toxic waste
  math::MPInt tau, alpha, beta, gamma, delta;
  math::MPInt::RandomLtN(order, &tau);
  math::MPInt::RandomLtN(order, &alpha);
  math::MPInt::RandomLtN(order, &beta);
  math::MPInt::RandomLtN(order, &gamma);
  math::MPInt::RandomLtN(order, &delta);

  // Ensure delta and gamma are nonzero (extremely unlikely but be safe)
  while (delta.IsZero()) {
    math::MPInt::RandomLtN(order, &delta);
  }
  while (gamma.IsZero()) {
    math::MPInt::RandomLtN(order, &gamma);
  }

  math::MPInt gamma_inv = gamma.InvertMod(order);
  math::MPInt delta_inv = delta.InvertMod(order);

  // Evaluate QAP polynomials at tau
  std::vector<math::MPInt> u_tau(n);
  std::vector<math::MPInt> v_tau(n);
  std::vector<math::MPInt> w_tau(n);
  for (int64_t i = 0; i < n; ++i) {
    u_tau[i] = poly_ops::Evaluate(qap.u[i], tau, order);
    v_tau[i] = poly_ops::Evaluate(qap.v[i], tau, order);
    w_tau[i] = poly_ops::Evaluate(qap.w[i], tau, order);
  }

  // Evaluate target polynomial at tau
  math::MPInt t_tau = poly_ops::Evaluate(qap.t, tau, order);

  // Build proving key
  ProvingKey pk;
  pk.pairing = std::move(pairing);
  pk.alpha_g1 = g1->MulBase(alpha);
  pk.beta_g1 = g1->MulBase(beta);
  pk.beta_g2 = g2->MulBase(beta);
  pk.delta_g1 = g1->MulBase(delta);
  pk.delta_g2 = g2->MulBase(delta);

  // h_query[i] = [tau^i * t(tau) / delta]_1, for i = 0..m-1
  pk.h_query.resize(m);
  math::MPInt t_tau_delta = t_tau.MulMod(delta_inv, order);
  math::MPInt tau_power(1);
  for (int64_t i = 0; i < m; ++i) {
    math::MPInt scalar = tau_power.MulMod(t_tau_delta, order);
    pk.h_query[i] = g1->MulBase(scalar);
    tau_power = tau_power.MulMod(tau, order);
  }

  // a_query[i] = [u_i(tau)]_1, b_g1_query[i] = [v_i(tau)]_1,
  // b_g2_query[i] = [v_i(tau)]_2
  pk.a_query.resize(n);
  pk.b_g1_query.resize(n);
  pk.b_g2_query.resize(n);
  for (int64_t i = 0; i < n; ++i) {
    pk.a_query[i] = g1->MulBase(u_tau[i]);
    pk.b_g1_query[i] = g1->MulBase(v_tau[i]);
    pk.b_g2_query[i] = g2->MulBase(v_tau[i]);
  }

  // l_query[j] = [(beta*u_j(tau) + alpha*v_j(tau) + w_j(tau)) / delta]_1
  // for private variables j = l+1..n-1  (indices relative to full witness)
  int64_t num_private = n - 1 - l;  // exclude constant wire and public inputs
  pk.l_query.resize(num_private);
  for (int64_t j = 0; j < num_private; ++j) {
    int64_t idx = j + 1 + l;  // actual variable index in witness
    math::MPInt val = beta.MulMod(u_tau[idx], order)
                          .AddMod(alpha.MulMod(v_tau[idx], order), order)
                          .AddMod(w_tau[idx], order)
                          .MulMod(delta_inv, order);
    pk.l_query[j] = g1->MulBase(val);
  }

  // Build verification key
  VerificationKey vk;
  vk.pairing = pk.pairing;
  vk.alpha_g1 = g1->CopyPoint(pk.alpha_g1);
  vk.beta_g2 = g2->CopyPoint(pk.beta_g2);
  vk.gamma_g2 = g2->MulBase(gamma);
  vk.delta_g2 = g2->CopyPoint(pk.delta_g2);

  // Precompute e(alpha, beta)
  vk.alpha_beta_gt = vk.pairing->Pairing(vk.alpha_g1, vk.beta_g2);

  // ic[i] = [(beta*u_i(tau) + alpha*v_i(tau) + w_i(tau)) / gamma]_1
  // for i = 0..l (constant wire + public inputs)
  vk.ic.resize(l + 1);
  for (int64_t i = 0; i <= l; ++i) {
    math::MPInt val = beta.MulMod(u_tau[i], order)
                          .AddMod(alpha.MulMod(v_tau[i], order), order)
                          .AddMod(w_tau[i], order)
                          .MulMod(gamma_inv, order);
    vk.ic[i] = g1->MulBase(val);
  }

  // Erase toxic waste
  tau.SetZero();
  alpha.SetZero();
  beta.SetZero();
  gamma.SetZero();
  delta.SetZero();

  return {std::move(pk), std::move(vk)};
}

Proof Prove(const ProvingKey& pk, const std::vector<math::MPInt>& witness,
            const R1csSystem& r1cs) {
  auto g1 = pk.pairing->GetGroup1();
  auto g2 = pk.pairing->GetGroup2();
  auto order = pk.pairing->GetOrder();
  int64_t n = r1cs.GetNumVariables();
  int64_t l = r1cs.GetNumPublicInputs();

  YACL_ENFORCE(static_cast<int64_t>(witness.size()) == n,
               "Witness size {} != num_variables {}", witness.size(), n);

  // Convert R1CS to QAP
  auto qap = R1csToQap(r1cs, order);

  // Compute a(x) = sum(w_i * u_i(x)), b(x) = sum(w_i * v_i(x)),
  //         c(x) = sum(w_i * w_i(x))
  Poly a_poly = {math::MPInt(0)};
  Poly b_poly = {math::MPInt(0)};
  Poly c_poly = {math::MPInt(0)};
  for (int64_t i = 0; i < n; ++i) {
    if (witness[i].IsZero()) {
      continue;
    }
    a_poly = poly_ops::Add(a_poly,
                           poly_ops::ScalarMul(qap.u[i], witness[i], order),
                           order);
    b_poly = poly_ops::Add(b_poly,
                           poly_ops::ScalarMul(qap.v[i], witness[i], order),
                           order);
    c_poly = poly_ops::Add(c_poly,
                           poly_ops::ScalarMul(qap.w[i], witness[i], order),
                           order);
  }

  // Compute h(x) = (a(x)*b(x) - c(x)) / t(x)
  Poly ab = poly_ops::Mul(a_poly, b_poly, order);
  Poly ab_minus_c = poly_ops::Sub(ab, c_poly, order);
  auto [h_poly, remainder] = poly_ops::DivMod(ab_minus_c, qap.t, order);

  // Verify remainder is zero (the witness must satisfy the R1CS)
  bool remainder_zero = true;
  for (const auto& coeff : remainder) {
    if (!coeff.IsZero()) {
      remainder_zero = false;
      break;
    }
  }
  YACL_ENFORCE(remainder_zero,
               "Witness does not satisfy R1CS (non-zero remainder)");

  // Sample blinding factors
  math::MPInt r, s;
  math::MPInt::RandomLtN(order, &r);
  math::MPInt::RandomLtN(order, &s);

  // Compute proof element A (in G1):
  // A = alpha_g1 + sum(w_i * a_query[i]) + r * delta_g1
  auto proof_a = g1->CopyPoint(pk.alpha_g1);
  for (int64_t i = 0; i < n; ++i) {
    if (witness[i].IsZero()) {
      continue;
    }
    auto term = g1->Mul(pk.a_query[i], witness[i]);
    g1->AddInplace(&proof_a, term);
  }
  g1->AddInplace(&proof_a, g1->Mul(pk.delta_g1, r));

  // Compute proof element B (in G2):
  // B = beta_g2 + sum(w_i * b_g2_query[i]) + s * delta_g2
  auto proof_b = g2->CopyPoint(pk.beta_g2);
  for (int64_t i = 0; i < n; ++i) {
    if (witness[i].IsZero()) {
      continue;
    }
    auto term = g2->Mul(pk.b_g2_query[i], witness[i]);
    g2->AddInplace(&proof_b, term);
  }
  g2->AddInplace(&proof_b, g2->Mul(pk.delta_g2, s));

  // Also compute B in G1 for the C calculation
  auto b_g1 = g1->CopyPoint(pk.beta_g1);
  for (int64_t i = 0; i < n; ++i) {
    if (witness[i].IsZero()) {
      continue;
    }
    auto term = g1->Mul(pk.b_g1_query[i], witness[i]);
    g1->AddInplace(&b_g1, term);
  }
  g1->AddInplace(&b_g1, g1->Mul(pk.delta_g1, s));

  // Compute proof element C (in G1):
  // C = sum(h_i * h_query[i]) + sum(w_j * l_query[j]) + s*A + r*B_g1 -
  // r*s*delta_g1
  auto proof_c = g1->MulBase(math::MPInt(0));  // start at infinity

  // Add h polynomial contribution
  for (int64_t i = 0;
       i < static_cast<int64_t>(h_poly.size()) &&
       i < static_cast<int64_t>(pk.h_query.size());
       ++i) {
    if (h_poly[i].IsZero()) {
      continue;
    }
    auto term = g1->Mul(pk.h_query[i], h_poly[i]);
    g1->AddInplace(&proof_c, term);
  }

  // Add private variable contributions (l_query)
  int64_t num_private = n - 1 - l;
  for (int64_t j = 0; j < num_private; ++j) {
    int64_t idx = j + 1 + l;
    if (witness[idx].IsZero()) {
      continue;
    }
    auto term = g1->Mul(pk.l_query[j], witness[idx]);
    g1->AddInplace(&proof_c, term);
  }

  // Add s*A
  g1->AddInplace(&proof_c, g1->Mul(proof_a, s));
  // Add r*B_g1
  g1->AddInplace(&proof_c, g1->Mul(b_g1, r));
  // Subtract r*s*delta_g1
  math::MPInt rs = r.MulMod(s, order);
  g1->SubInplace(&proof_c, g1->Mul(pk.delta_g1, rs));

  return Proof{std::move(proof_a), std::move(proof_b), std::move(proof_c)};
}

bool Verify(const VerificationKey& vk,
            const std::vector<math::MPInt>& public_inputs,
            const Proof& proof) {
  auto g1 = vk.pairing->GetGroup1();
  auto g2 = vk.pairing->GetGroup2();
  auto gt = vk.pairing->GetGroupT();

  YACL_ENFORCE(
      static_cast<int64_t>(public_inputs.size()) + 1 ==
          static_cast<int64_t>(vk.ic.size()),
      "Public inputs size {} + 1 != ic size {}", public_inputs.size(),
      vk.ic.size());

  // Compute L = ic[0] + sum(x_i * ic[i+1]) for public inputs
  auto acc = g1->CopyPoint(vk.ic[0]);
  for (int64_t i = 0; i < static_cast<int64_t>(public_inputs.size()); ++i) {
    if (public_inputs[i].IsZero()) {
      continue;
    }
    auto term = g1->Mul(vk.ic[i + 1], public_inputs[i]);
    g1->AddInplace(&acc, term);
  }

  // Verify: e(A, B) == e(alpha, beta) * e(L, gamma_g2) * e(C, delta_g2)
  auto lhs = vk.pairing->Pairing(proof.a, proof.b);
  YACL_ENFORCE(vk.alpha_beta_gt.has_value(), "VK missing precomputed pairing");
  auto rhs_1 = gt->DeepCopy(*vk.alpha_beta_gt);
  auto rhs_2 = vk.pairing->Pairing(acc, vk.gamma_g2);
  auto rhs_3 = vk.pairing->Pairing(proof.c, vk.delta_g2);

  auto rhs = gt->Mul(rhs_1, rhs_2);
  rhs = gt->Mul(rhs, rhs_3);

  return gt->Equal(lhs, rhs);
}

}  // namespace yacl::engine::snark
