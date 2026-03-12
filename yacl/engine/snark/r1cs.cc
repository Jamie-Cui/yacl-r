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

#include "yacl/base/exception.h"

namespace yacl::engine::snark {

void LinearCombination::AddTerm(Variable var, const math::MPInt& coeff) {
  auto it = terms_.find(var);
  if (it != terms_.end()) {
    it->second += coeff;
    if (it->second.IsZero()) {
      terms_.erase(it);
    }
  } else {
    if (!coeff.IsZero()) {
      terms_[var] = coeff;
    }
  }
}

math::MPInt LinearCombination::Evaluate(
    const std::vector<math::MPInt>& witness,
    const math::MPInt& modulus) const {
  math::MPInt result(0);
  for (const auto& [var, coeff] : terms_) {
    YACL_ENFORCE(var >= 0 && var < static_cast<int64_t>(witness.size()),
                 "Variable index {} out of range [0, {})", var, witness.size());
    math::MPInt term = coeff.MulMod(witness[var], modulus);
    result = result.AddMod(term, modulus);
  }
  return result;
}

void R1csSystem::AddConstraint(R1csConstraint constraint) {
  constraints_.push_back(std::move(constraint));
}

bool R1csSystem::IsSatisfied(const std::vector<math::MPInt>& witness,
                             const math::MPInt& modulus) const {
  YACL_ENFORCE(static_cast<int64_t>(witness.size()) == num_variables_,
               "Witness size {} != num_variables {}", witness.size(),
               num_variables_);
  YACL_ENFORCE(witness[0].IsOne(),
               "Witness[0] must be 1 (constant wire)");

  for (int64_t i = 0; i < static_cast<int64_t>(constraints_.size()); ++i) {
    const auto& c = constraints_[i];
    math::MPInt a_val = c.a.Evaluate(witness, modulus);
    math::MPInt b_val = c.b.Evaluate(witness, modulus);
    math::MPInt c_val = c.c.Evaluate(witness, modulus);

    math::MPInt ab = a_val.MulMod(b_val, modulus);
    if (ab != c_val) {
      return false;
    }
  }
  return true;
}

}  // namespace yacl::engine::snark
