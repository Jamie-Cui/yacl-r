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

#include "yacl/engine/snark/r1cs.h"
#include "yacl/math/mpint/mp_int.h"

namespace yacl::engine::snark {

// A DSL for building R1CS circuits.
//
// Variables are allocated as either public or private. Linear operations
// (Add, Sub, MulConst, AddConst) are free -- they only create new linear
// combinations. Mul() generates one R1CS constraint per call.
//
// Usage:
//   CircuitBuilder cb;
//   auto x = cb.AllocPublic();
//   auto y = cb.AllocPrivate();
//   auto z = cb.Mul(x, y);      // one constraint
//   cb.AssertEqual(z, some_var);
//   auto r1cs = cb.Build();
class CircuitBuilder {
 public:
  CircuitBuilder();

  // Allocate a new public input variable. Returns the variable index.
  Variable AllocPublic();

  // Allocate a new private (witness) variable. Returns the variable index.
  Variable AllocPrivate();

  // Get the constant "one" wire (variable index 0 in the final R1CS).
  Variable One() const { return 0; }

  // --- Linear operations (no constraints generated) ---

  // Returns a + b (as a new variable with an internal LC).
  Variable Add(Variable a, Variable b);

  // Returns a - b.
  Variable Sub(Variable a, Variable b);

  // Returns a * constant c (no constraint, just scales the LC).
  Variable MulConst(Variable a, const math::MPInt& c);

  // Returns a + constant c.
  Variable AddConst(Variable a, const math::MPInt& c);

  // --- Non-linear operations (generate constraints) ---

  // Returns a * b. Generates one R1CS constraint.
  Variable Mul(Variable a, Variable b);

  // --- Assertions ---

  // Assert a == b by adding constraint: (a - b) * 1 = 0.
  void AssertEqual(Variable a, Variable b);

  // Assert var is boolean (0 or 1): var * (1 - var) = 0.
  void AssertBoolean(Variable var);

  // Assert a * b == c.
  void AssertMulEquals(Variable a, Variable b, Variable c);

  // --- Build ---

  // Build the R1CS system. Remaps internal variable indices to the
  // standard layout: [one, public_1..public_l, private_1..private_k].
  R1csSystem Build() const;

  // Compute the full witness vector from user-provided assignments.
  // assignments maps the user-facing variable index to its value.
  // The result is in R1CS variable order: [1, public, private].
  std::vector<math::MPInt> ComputeWitness(
      const std::map<Variable, math::MPInt>& assignments,
      const math::MPInt& modulus) const;

 private:
  // Internal representation of a variable as a linear combination.
  struct VarInfo {
    LinearCombination lc;
    bool is_public = false;
    bool is_private = false;
    bool is_computed = false;  // has an LC expression
  };

  Variable AllocVar();

  // Get the LC for a variable (either its stored LC or a trivial one).
  LinearCombination GetLc(Variable var) const;

  // Translate a user-facing LC to the R1CS variable space.
  LinearCombination RemapLc(
      const LinearCombination& lc,
      const std::map<Variable, Variable>& var_map) const;

  int64_t next_var_ = 1;  // 0 is reserved for the "one" wire
  std::vector<Variable> public_vars_;
  std::vector<Variable> private_vars_;
  std::map<Variable, VarInfo> var_info_;

  // Internal constraints stored before remapping.
  struct InternalConstraint {
    LinearCombination a;
    LinearCombination b;
    LinearCombination c;
  };
  std::vector<InternalConstraint> constraints_;
};

}  // namespace yacl::engine::snark
