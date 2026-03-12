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
#include <deque>
#include <memory>
#include <string>
#include <vector>

#include "absl/types/span.h"

#include "yacl/engine/secret_share/types.h"
#include "yacl/kernel/ot_kernel.h"
#include "yacl/link/context.h"

namespace yacl::engine {

// SSEngine: 2-party semi-honest additive secret-sharing engine.
//
// Supports two domains:
//   Arithmetic: additive shares in Z_{2^64}  (x0 + x1 = x mod 2^64)
//   Boolean:    XOR shares in GF(2)^64       (x0 ^ x1 = x)
//
// Offline/Online split:
//   Offline: PreprocessArithTriples / PreprocessBoolTriples fill Beaver triple
//            queues using OT-based protocols.
//   Online:  MulA and AndB each consume one triple. All other operations are
//            local (no communication) or use 1 round of Send/Recv.
//
// Role assignment:
//   rank 0: OT sender for triple gen; OT receiver for B2A.
//   rank 1: OT receiver for triple gen; OT sender for B2A.
//
// Threading: NOT thread-safe. link::Context is single-threaded by design.
class SSEngine {
 public:
  SSEngine(int64_t rank, std::shared_ptr<link::Context> lctx);

  SSEngine(const SSEngine&) = delete;
  SSEngine& operator=(const SSEngine&) = delete;

  // ---------------------------------------------------------------
  // Preprocessing (Offline Phase)
  // ---------------------------------------------------------------

  // Generate n arithmetic Beaver triples using OT-based Gilboa multiplication.
  void PreprocessArithTriples(int64_t n);

  // Generate n boolean Beaver AND triples using OT-based protocol.
  void PreprocessBoolTriples(int64_t n);

  // ---------------------------------------------------------------
  // Arithmetic Operations over Z_{2^64}
  // ---------------------------------------------------------------

  // Secret-share a value. Rank-0 provides the secret; rank-1 passes 0.
  AShare ShareA(uint64_t secret);

  // Reconstruct secret from additive shares. Both parties call this.
  uint64_t RevealA(AShare share);

  // Local: no communication.
  AShare AddA(AShare x, AShare y);
  AShare SubA(AShare x, AShare y);
  AShare NegA(AShare x);
  AShare MulConstA(AShare x, uint64_t c);

  // Multiply two shared values. Consumes 1 arithmetic triple, 1 round.
  AShare MulA(AShare x, AShare y);

  // ---------------------------------------------------------------
  // Boolean Operations over GF(2)^64
  // ---------------------------------------------------------------

  BShare ShareB(uint64_t secret);
  uint64_t RevealB(BShare share);

  // Local: no communication.
  BShare XorB(BShare x, BShare y);
  BShare NotB(BShare x);

  // AND of two shared values. Consumes 1 boolean triple, 1 round.
  BShare AndB(BShare x, BShare y);

  // ---------------------------------------------------------------
  // Conversions
  // ---------------------------------------------------------------

  // Arithmetic-to-Boolean. Consumes 64 boolean triples.
  BShare A2B(AShare x);

  // Boolean-to-Arithmetic. Uses fresh OT internally (no triple consumption).
  AShare B2A(BShare x);

  // ---------------------------------------------------------------
  // Inspection
  // ---------------------------------------------------------------

  int64_t ArithTripleCount() const;
  int64_t BoolTripleCount() const;
  int64_t Rank() const;

 private:
  // Internal helpers
  std::string NextTag(std::string_view prefix);
  void SendU64(std::string_view tag, uint64_t val);
  uint64_t RecvU64(std::string_view tag);
  void SendU64Span(std::string_view tag, absl::Span<const uint64_t> data);
  std::vector<uint64_t> RecvU64Vec(std::string_view tag, int64_t count);

  ATriple ConsumeATriple();
  BTriple ConsumeBTriple();

  // OT helper: run chosen-input 1-out-of-2 OT on top of eval_rot.
  // Sender provides (m0[i], m1[i]) for each OT; receiver provides choice[i].
  // Returns received values on receiver side; empty on sender side.
  // Messages and results are uint64_t (truncated from 128-bit ROT blocks).
  struct OtMessages {
    uint64_t m0;
    uint64_t m1;
  };
  std::vector<uint64_t> RunChosenOT(const std::shared_ptr<link::Context>& ctx,
                                    bool is_sender, int64_t num_ots,
                                    const std::vector<OtMessages>& sender_msgs,
                                    const std::vector<bool>& receiver_choices);

  // Triple-generation helpers: run one OT direction and return cross-term.
  std::vector<uint64_t> RunArithOtDirection(
      bool is_sender, int64_t n, int64_t num_ots,
      const std::vector<uint64_t>& a_local,
      const std::vector<uint64_t>& b_local);
  std::vector<uint64_t> RunBoolOtDirection(
      bool is_sender, int64_t n, int64_t num_ots,
      const std::vector<uint64_t>& a_local,
      const std::vector<uint64_t>& b_local);

  const int64_t rank_;
  std::shared_ptr<link::Context> lctx_;
  int64_t tag_ctr_{0};

  std::deque<ATriple> arith_triples_;
  std::deque<BTriple> bool_triples_;
};

}  // namespace yacl::engine
