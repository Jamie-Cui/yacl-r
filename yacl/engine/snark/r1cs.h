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

#include <cstdint>
#include <map>
#include <vector>

#include "yacl/math/mpint/mp_int.h"

namespace yacl::engine::snark {

// Variable index type. Index 0 is reserved for the constant "one" wire.
using Variable = int64_t;

// A linear combination is a sparse map from variable indices to coefficients.
// Represents sum_i (coeff_i * var_i).
class LinearCombination {
 public:
  LinearCombination() = default;

  void AddTerm(Variable var, const math::MPInt& coeff);

  // Evaluate the linear combination given a full witness vector.
  // Returns sum_i (coeff_i * witness[var_i]) mod modulus.
  math::MPInt Evaluate(const std::vector<math::MPInt>& witness,
                       const math::MPInt& modulus) const;

  const std::map<Variable, math::MPInt>& GetTerms() const { return terms_; }

  bool IsEmpty() const { return terms_.empty(); }

 private:
  std::map<Variable, math::MPInt> terms_;
};

// A single R1CS constraint: (a . w) * (b . w) = (c . w)
struct R1csConstraint {
  LinearCombination a;
  LinearCombination b;
  LinearCombination c;
};

// The full R1CS constraint system.
// Witness layout: w = [1, public_1, ..., public_l, private_1, ..., private_k]
class R1csSystem {
 public:
  R1csSystem() = default;

  void AddConstraint(R1csConstraint constraint);

  // Check whether the given witness satisfies all constraints mod modulus.
  bool IsSatisfied(const std::vector<math::MPInt>& witness,
                   const math::MPInt& modulus) const;

  int64_t GetNumConstraints() const {
    return static_cast<int64_t>(constraints_.size());
  }

  int64_t GetNumVariables() const { return num_variables_; }
  void SetNumVariables(int64_t n) { num_variables_ = n; }

  int64_t GetNumPublicInputs() const { return num_public_inputs_; }
  void SetNumPublicInputs(int64_t n) { num_public_inputs_ = n; }

  const std::vector<R1csConstraint>& GetConstraints() const {
    return constraints_;
  }

 private:
  std::vector<R1csConstraint> constraints_;
  int64_t num_variables_ = 0;
  int64_t num_public_inputs_ = 0;
};

}  // namespace yacl::engine::snark
