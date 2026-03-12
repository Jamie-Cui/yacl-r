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

#include "yacl/base/exception.h"

namespace yacl::engine::snark {

CircuitBuilder::CircuitBuilder() {
  // Variable 0 is the constant "one" wire.
  VarInfo one_info;
  one_info.lc.AddTerm(0, math::MPInt(1));
  one_info.is_computed = true;
  var_info_[0] = std::move(one_info);
}

Variable CircuitBuilder::AllocVar() {
  Variable v = next_var_++;
  return v;
}

Variable CircuitBuilder::AllocPublic() {
  Variable v = AllocVar();
  VarInfo info;
  info.is_public = true;
  info.lc.AddTerm(v, math::MPInt(1));
  var_info_[v] = std::move(info);
  public_vars_.push_back(v);
  return v;
}

Variable CircuitBuilder::AllocPrivate() {
  Variable v = AllocVar();
  VarInfo info;
  info.is_private = true;
  info.lc.AddTerm(v, math::MPInt(1));
  var_info_[v] = std::move(info);
  private_vars_.push_back(v);
  return v;
}

LinearCombination CircuitBuilder::GetLc(Variable var) const {
  auto it = var_info_.find(var);
  if (it != var_info_.end()) {
    return it->second.lc;
  }
  // Unknown variable -- treat as a simple variable reference
  LinearCombination lc;
  lc.AddTerm(var, math::MPInt(1));
  return lc;
}

Variable CircuitBuilder::Add(Variable a, Variable b) {
  Variable result = AllocVar();
  auto lc_a = GetLc(a);
  auto lc_b = GetLc(b);

  LinearCombination combined;
  for (const auto& [var, coeff] : lc_a.GetTerms()) {
    combined.AddTerm(var, coeff);
  }
  for (const auto& [var, coeff] : lc_b.GetTerms()) {
    combined.AddTerm(var, coeff);
  }

  VarInfo info;
  info.is_computed = true;
  info.lc = std::move(combined);
  var_info_[result] = std::move(info);
  return result;
}

Variable CircuitBuilder::Sub(Variable a, Variable b) {
  Variable result = AllocVar();
  auto lc_a = GetLc(a);
  auto lc_b = GetLc(b);

  LinearCombination combined;
  for (const auto& [var, coeff] : lc_a.GetTerms()) {
    combined.AddTerm(var, coeff);
  }
  for (const auto& [var, coeff] : lc_b.GetTerms()) {
    combined.AddTerm(var, -coeff);
  }

  VarInfo info;
  info.is_computed = true;
  info.lc = std::move(combined);
  var_info_[result] = std::move(info);
  return result;
}

Variable CircuitBuilder::MulConst(Variable a, const math::MPInt& c) {
  Variable result = AllocVar();
  auto lc_a = GetLc(a);

  LinearCombination scaled;
  for (const auto& [var, coeff] : lc_a.GetTerms()) {
    scaled.AddTerm(var, coeff * c);
  }

  VarInfo info;
  info.is_computed = true;
  info.lc = std::move(scaled);
  var_info_[result] = std::move(info);
  return result;
}

Variable CircuitBuilder::AddConst(Variable a, const math::MPInt& c) {
  Variable result = AllocVar();
  auto lc_a = GetLc(a);

  LinearCombination combined;
  for (const auto& [var, coeff] : lc_a.GetTerms()) {
    combined.AddTerm(var, coeff);
  }
  // Add c * one_wire
  combined.AddTerm(0, c);

  VarInfo info;
  info.is_computed = true;
  info.lc = std::move(combined);
  var_info_[result] = std::move(info);
  return result;
}

Variable CircuitBuilder::Mul(Variable a, Variable b) {
  // Multiplication requires a constraint. We allocate a new private variable
  // for the result.
  Variable result = AllocPrivate();

  InternalConstraint constraint;
  constraint.a = GetLc(a);
  constraint.b = GetLc(b);
  constraint.c = GetLc(result);
  constraints_.push_back(std::move(constraint));

  return result;
}

void CircuitBuilder::AssertEqual(Variable a, Variable b) {
  // (a - b) * 1 = 0
  auto lc_a = GetLc(a);
  auto lc_b = GetLc(b);

  LinearCombination diff;
  for (const auto& [var, coeff] : lc_a.GetTerms()) {
    diff.AddTerm(var, coeff);
  }
  for (const auto& [var, coeff] : lc_b.GetTerms()) {
    diff.AddTerm(var, -coeff);
  }

  InternalConstraint constraint;
  constraint.a = std::move(diff);
  constraint.b.AddTerm(0, math::MPInt(1));  // 1
  // c is empty (zero)
  constraints_.push_back(std::move(constraint));
}

void CircuitBuilder::AssertBoolean(Variable var) {
  // var * (1 - var) = 0
  auto lc_var = GetLc(var);

  LinearCombination one_minus_var;
  one_minus_var.AddTerm(0, math::MPInt(1));
  for (const auto& [v, coeff] : lc_var.GetTerms()) {
    one_minus_var.AddTerm(v, -coeff);
  }

  InternalConstraint constraint;
  constraint.a = lc_var;
  constraint.b = std::move(one_minus_var);
  // c is empty (zero)
  constraints_.push_back(std::move(constraint));
}

void CircuitBuilder::AssertMulEquals(Variable a, Variable b, Variable c) {
  InternalConstraint constraint;
  constraint.a = GetLc(a);
  constraint.b = GetLc(b);
  constraint.c = GetLc(c);
  constraints_.push_back(std::move(constraint));
}

LinearCombination CircuitBuilder::RemapLc(
    const LinearCombination& lc,
    const std::map<Variable, Variable>& var_map) const {
  LinearCombination result;
  for (const auto& [var, coeff] : lc.GetTerms()) {
    auto it = var_map.find(var);
    YACL_ENFORCE(it != var_map.end(),
                 "Variable {} not found in variable map", var);
    result.AddTerm(it->second, coeff);
  }
  return result;
}

R1csSystem CircuitBuilder::Build() const {
  // Build variable map: internal var -> R1CS var index.
  // R1CS layout: [0=one, 1..l=public, l+1..n-1=private]
  std::map<Variable, Variable> var_map;
  var_map[0] = 0;  // one wire

  Variable next_idx = 1;
  for (Variable v : public_vars_) {
    var_map[v] = next_idx++;
  }
  for (Variable v : private_vars_) {
    var_map[v] = next_idx++;
  }

  // Also map any intermediate computed variables that appear in constraints
  // but weren't explicitly allocated as public/private.
  // Collect all variables referenced in constraints.
  auto collect_vars = [](const LinearCombination& lc,
                         std::map<Variable, Variable>& vmap,
                         Variable& next) {
    for (const auto& [var, coeff] : lc.GetTerms()) {
      if (vmap.find(var) == vmap.end()) {
        vmap[var] = next++;
      }
    }
  };

  for (const auto& c : constraints_) {
    collect_vars(c.a, var_map, next_idx);
    collect_vars(c.b, var_map, next_idx);
    collect_vars(c.c, var_map, next_idx);
  }

  R1csSystem r1cs;
  r1cs.SetNumVariables(next_idx);
  r1cs.SetNumPublicInputs(static_cast<int64_t>(public_vars_.size()));

  for (const auto& c : constraints_) {
    R1csConstraint rc;
    rc.a = RemapLc(c.a, var_map);
    rc.b = RemapLc(c.b, var_map);
    rc.c = RemapLc(c.c, var_map);
    r1cs.AddConstraint(std::move(rc));
  }

  return r1cs;
}

std::vector<math::MPInt> CircuitBuilder::ComputeWitness(
    const std::map<Variable, math::MPInt>& assignments,
    const math::MPInt& modulus) const {
  // Build variable map same as Build()
  std::map<Variable, Variable> var_map;
  var_map[0] = 0;

  Variable next_idx = 1;
  for (Variable v : public_vars_) {
    var_map[v] = next_idx++;
  }
  for (Variable v : private_vars_) {
    var_map[v] = next_idx++;
  }

  // Collect extra variables from constraints
  for (const auto& c : constraints_) {
    for (const auto& [var, coeff] : c.a.GetTerms()) {
      if (var_map.find(var) == var_map.end()) {
        var_map[var] = next_idx++;
      }
    }
    for (const auto& [var, coeff] : c.b.GetTerms()) {
      if (var_map.find(var) == var_map.end()) {
        var_map[var] = next_idx++;
      }
    }
    for (const auto& [var, coeff] : c.c.GetTerms()) {
      if (var_map.find(var) == var_map.end()) {
        var_map[var] = next_idx++;
      }
    }
  }

  // Build a map from internal variable to its value
  std::map<Variable, math::MPInt> values;
  values[0] = math::MPInt(1);  // one wire

  // Set assigned values
  for (const auto& [var, val] : assignments) {
    values[var] = val.Mod(modulus);
  }

  // Helper to evaluate an LC if all referenced variables are known.
  auto try_eval_lc = [&](const LinearCombination& lc,
                         math::MPInt* out) -> bool {
    math::MPInt val(0);
    for (const auto& [ref_var, coeff] : lc.GetTerms()) {
      auto it = values.find(ref_var);
      if (it == values.end()) {
        return false;
      }
      val = val.AddMod(coeff.MulMod(it->second, modulus), modulus);
    }
    *out = val;
    return true;
  };

  // Evaluate computed variables (linear combinations).
  // Multiple passes to handle dependency chains among computed variables.
  bool changed = true;
  while (changed) {
    changed = false;
    for (const auto& [var, info] : var_info_) {
      if (info.is_computed && values.find(var) == values.end()) {
        math::MPInt val;
        if (try_eval_lc(info.lc, &val)) {
          values[var] = val;
          changed = true;
        }
      }
    }

    // Propagate multiplication results: for each constraint a*b=c,
    // if a and b values are computable and c's output variable is unknown,
    // compute and set c's value.
    for (const auto& c : constraints_) {
      math::MPInt a_val;
      math::MPInt b_val;
      if (!try_eval_lc(c.a, &a_val) || !try_eval_lc(c.b, &b_val)) {
        continue;
      }
      math::MPInt expected = a_val.MulMod(b_val, modulus);

      // If c is a single variable with coefficient 1 and that variable
      // has no value yet, assign it.
      const auto& c_terms = c.c.GetTerms();
      if (c_terms.size() == 1) {
        auto it = c_terms.begin();
        if (it->second.IsOne() && values.find(it->first) == values.end()) {
          values[it->first] = expected;
          changed = true;
        }
      }
    }
  }

  // Build witness vector in R1CS order
  std::vector<math::MPInt> witness(next_idx, math::MPInt(0));
  witness[0] = math::MPInt(1);

  for (const auto& [internal_var, r1cs_idx] : var_map) {
    auto it = values.find(internal_var);
    if (it != values.end()) {
      witness[r1cs_idx] = it->second;
    }
  }

  return witness;
}

}  // namespace yacl::engine::snark
