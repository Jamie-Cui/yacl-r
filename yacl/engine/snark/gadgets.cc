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

#include "yacl/base/exception.h"

namespace yacl::engine::snark::gadgets {

Variable Boolean(CircuitBuilder& cb) {
  Variable v = cb.AllocPrivate();
  cb.AssertBoolean(v);
  return v;
}

Variable BoolAnd(CircuitBuilder& cb, Variable a, Variable b) {
  // a * b (both assumed boolean by caller)
  return cb.Mul(a, b);
}

Variable BoolOr(CircuitBuilder& cb, Variable a, Variable b) {
  // a + b - a*b
  auto ab = cb.Mul(a, b);
  auto sum = cb.Add(a, b);
  return cb.Sub(sum, ab);
}

Variable BoolNot(CircuitBuilder& cb, Variable a) {
  // 1 - a
  return cb.AddConst(cb.MulConst(a, math::MPInt(-1)), math::MPInt(1));
}

Variable BoolXor(CircuitBuilder& cb, Variable a, Variable b) {
  // a + b - 2*a*b
  auto ab = cb.Mul(a, b);
  auto two_ab = cb.MulConst(ab, math::MPInt(2));
  auto sum = cb.Add(a, b);
  return cb.Sub(sum, two_ab);
}

std::vector<Variable> ToBits(CircuitBuilder& cb, Variable var,
                             int64_t num_bits) {
  YACL_ENFORCE(num_bits > 0, "num_bits must be positive");

  std::vector<Variable> bits(num_bits);
  for (int64_t i = 0; i < num_bits; ++i) {
    bits[i] = Boolean(cb);
  }

  // Constraint: sum(bit_i * 2^i) == var
  auto packed = FromBits(cb, bits);
  cb.AssertEqual(packed, var);

  return bits;
}

Variable FromBits(CircuitBuilder& cb, const std::vector<Variable>& bits) {
  YACL_ENFORCE(!bits.empty(), "bits must not be empty");

  // Start with bit[0] * 2^0
  Variable result = bits[0];
  math::MPInt power(2);
  for (int64_t i = 1; i < static_cast<int64_t>(bits.size()); ++i) {
    auto scaled = cb.MulConst(bits[i], power);
    result = cb.Add(result, scaled);
    power *= math::MPInt(2);
  }
  return result;
}

void RangeCheck(CircuitBuilder& cb, Variable var, int64_t num_bits) {
  ToBits(cb, var, num_bits);
}

Variable LessThan(CircuitBuilder& cb, Variable a, Variable b,
                  int64_t num_bits) {
  YACL_ENFORCE(num_bits > 0 && num_bits < 253,
               "num_bits must be in [1, 252]");

  // Compute diff = 2^num_bits + a - b.
  // If a < b, then diff is in [0, 2^num_bits) and bit num_bits is 0.
  // If a >= b, then diff is in [2^num_bits, 2^(num_bits+1)) and bit
  // num_bits is 1.
  math::MPInt offset(1);
  offset <<= static_cast<size_t>(num_bits);
  auto a_plus_offset = cb.AddConst(a, offset);
  auto diff = cb.Sub(a_plus_offset, b);

  // Decompose diff into num_bits+1 bits
  auto bits = ToBits(cb, diff, num_bits + 1);

  // The high bit is 1 if a >= b, 0 if a < b.
  // Return NOT of high bit.
  return BoolNot(cb, bits[num_bits]);
}

Variable Select(CircuitBuilder& cb, Variable cond, Variable a, Variable b) {
  // cond * (a - b) + b
  auto diff = cb.Sub(a, b);
  auto selected = cb.Mul(cond, diff);
  return cb.Add(selected, b);
}

Variable MiMCHash(CircuitBuilder& cb, Variable input, Variable key,
                  int64_t rounds) {
  YACL_ENFORCE(rounds > 0, "rounds must be positive");

  // MiMC with x^7 permutation.
  // Round constants are derived deterministically (here using simple indices).
  Variable state = cb.Add(input, key);

  for (int64_t r = 0; r < rounds; ++r) {
    // Add round constant (use r+1 as a simple deterministic constant)
    if (r > 0) {
      state = cb.AddConst(state, math::MPInt(r));
    }

    // x^7 = x * x * x * x * x * x * x
    // = ((x^2)^2 * x) * x^2  -- 4 multiplications
    auto x2 = cb.Mul(state, state);           // x^2
    auto x4 = cb.Mul(x2, x2);                // x^4
    auto x6 = cb.Mul(x4, x2);                // x^6
    state = cb.Mul(x6, state);                // x^7

    // Add key
    state = cb.Add(state, key);
  }

  return state;
}

}  // namespace yacl::engine::snark::gadgets
