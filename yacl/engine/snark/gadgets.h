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
#include <vector>

#include "yacl/engine/snark/circuit_builder.h"

namespace yacl::engine::snark::gadgets {

// --- Boolean gadgets ---

// Allocate a boolean private variable (constrains it to {0, 1}).
Variable Boolean(CircuitBuilder& cb);

// Boolean AND: a * b (both must be boolean).
Variable BoolAnd(CircuitBuilder& cb, Variable a, Variable b);

// Boolean OR: a + b - a*b.
Variable BoolOr(CircuitBuilder& cb, Variable a, Variable b);

// Boolean NOT: 1 - a (a must be boolean).
Variable BoolNot(CircuitBuilder& cb, Variable a);

// Boolean XOR: a + b - 2*a*b.
Variable BoolXor(CircuitBuilder& cb, Variable a, Variable b);

// --- Bit decomposition ---

// Decompose a variable into `num_bits` boolean variables (LSB first).
// Each output bit is constrained to be boolean.
// A packing constraint ensures sum(bit_i * 2^i) == var.
std::vector<Variable> ToBits(CircuitBuilder& cb, Variable var,
                             int64_t num_bits);

// Recompose bits (LSB first) into a single variable.
// No new constraints; purely linear.
Variable FromBits(CircuitBuilder& cb, const std::vector<Variable>& bits);

// --- Range check ---

// Assert that var is in [0, 2^num_bits). Decomposes into bits internally.
void RangeCheck(CircuitBuilder& cb, Variable var, int64_t num_bits);

// --- Comparison ---

// Returns a boolean variable that is 1 iff a < b (unsigned, num_bits range).
// Both a and b must be in [0, 2^num_bits).
Variable LessThan(CircuitBuilder& cb, Variable a, Variable b,
                  int64_t num_bits);

// --- Selection ---

// Select: returns cond ? a : b.
// cond must be boolean. Computes cond*(a-b) + b.
Variable Select(CircuitBuilder& cb, Variable cond, Variable a, Variable b);

// --- MiMC hash ---

// MiMC-2n/n hash function in a circuit. Returns the hash output variable.
// Uses exponent 7 (x^7 per round) with the given number of rounds.
Variable MiMCHash(CircuitBuilder& cb, Variable input, Variable key,
                  int64_t rounds = 220);

}  // namespace yacl::engine::snark::gadgets
