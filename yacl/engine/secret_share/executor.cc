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

#include "yacl/engine/secret_share/executor.h"

#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <span>
#include "fmt/format.h"

#include "yacl/base/byte_container_view.h"
#include "yacl/base/exception.h"
#include "yacl/crypto/rand/rand.h"
#include "yacl/kernel/ot_kernel.h"
#include "yacl/kernel/type/ot_store.h"

namespace yacl::engine {

namespace {
constexpr int64_t kBits = 64;
// Ferret OT extension requires at least 2 * lpn_param.t OTs per batch.
// Default LPN param t = 1280, so minimum is 2560.
constexpr int64_t kMinOts = 2560;
}  // namespace

// ================================================================
// Constructor
// ================================================================

SSExecutor::SSExecutor(int64_t rank, std::shared_ptr<link::Context> lctx)
    : rank_(rank), lctx_(std::move(lctx)) {
  YACL_ENFORCE(rank_ == 0 || rank_ == 1, "rank must be 0 or 1, got {}", rank_);
  YACL_ENFORCE(lctx_ != nullptr);
  YACL_ENFORCE(lctx_->WorldSize() == 2, "SSExecutor requires 2 parties, got {}",
               lctx_->WorldSize());
}

// ================================================================
// Helpers
// ================================================================

std::string SSExecutor::NextTag(std::string_view prefix) {
  return fmt::format("ss/{}/{}", prefix, tag_ctr_++);
}

void SSExecutor::SendU64(std::string_view tag, uint64_t val) {
  lctx_->SendAsync(1 - rank_, ByteContainerView(&val, sizeof(val)),
                   std::string(tag));
}

uint64_t SSExecutor::RecvU64(std::string_view tag) {
  auto buf = lctx_->Recv(1 - rank_, std::string(tag));
  YACL_ENFORCE(buf.size() == static_cast<int64_t>(sizeof(uint64_t)));
  uint64_t val = 0;
  std::memcpy(&val, buf.data(), sizeof(val));
  return val;
}

void SSExecutor::SendU64Span(std::string_view tag,
                             std::span<const uint64_t> data) {
  lctx_->SendAsync(
      1 - rank_, ByteContainerView(data.data(), data.size() * sizeof(uint64_t)),
      std::string(tag));
}

std::vector<uint64_t> SSExecutor::RecvU64Vec(std::string_view tag,
                                             int64_t count) {
  auto buf = lctx_->Recv(1 - rank_, std::string(tag));
  YACL_ENFORCE(buf.size() == count * static_cast<int64_t>(sizeof(uint64_t)));
  std::vector<uint64_t> result(count);
  std::memcpy(result.data(), buf.data(), buf.size());
  return result;
}

ATriple SSExecutor::ConsumeATriple() {
  YACL_ENFORCE(!arith_triples_.empty(),
               "No arithmetic triples. Call PreprocessArithTriples first.");
  auto t = arith_triples_.front();
  arith_triples_.pop_front();
  return t;
}

BTriple SSExecutor::ConsumeBTriple() {
  YACL_ENFORCE(!bool_triples_.empty(),
               "No boolean triples. Call PreprocessBoolTriples first.");
  auto t = bool_triples_.front();
  bool_triples_.pop_front();
  return t;
}

int64_t SSExecutor::ArithTripleCount() const {
  return static_cast<int64_t>(arith_triples_.size());
}

int64_t SSExecutor::BoolTripleCount() const {
  return static_cast<int64_t>(bool_triples_.size());
}

int64_t SSExecutor::Rank() const { return rank_; }

// ================================================================
// RunChosenOT: ROT-based 1-out-of-2 OT for uint64_t messages
// ================================================================
//
// Protocol (built on top of eval_rot):
//   1. Both parties run eval_rot for random OT correlations.
//   2. Receiver sends flip bits: d[i] = rot_choice[i] XOR desired_choice[i].
//   3. Sender computes corrections using XOR masking:
//        pad0 = (uint64_t) rot_block[i][d ? 1 : 0]
//        pad1 = (uint64_t) rot_block[i][d ? 0 : 1]
//        e0[i] = m0[i] ^ pad0,  e1[i] = m1[i] ^ pad1
//   4. Receiver computes: result[i] = e_{desired}[i] ^ rot_block[i].
//
std::vector<uint64_t> SSExecutor::RunChosenOT(
    const std::shared_ptr<link::Context>& parent_ctx, bool is_sender,
    int64_t num_ots, const std::vector<OtMessages>& sender_msgs,
    const std::vector<bool>& receiver_choices) {
  YACL_ENFORCE(num_ots > 0);

  auto ctx = std::shared_ptr<link::Context>(parent_ctx->Spawn());

  crypto::OtKernel kernel(is_sender ? crypto::OtKernel::Role::Sender
                                    : crypto::OtKernel::Role::Receiver);
  kernel.init(ctx);

  // Ferret OT extension requires a minimum batch size. Pad if needed.
  int64_t padded = std::max(num_ots, kMinOts);
  int64_t flip_words = (padded + 63) / 64;

  if (is_sender) {
    YACL_ENFORCE(static_cast<int64_t>(sender_msgs.size()) == num_ots);

    crypto::OtSendStore rot_send(padded, crypto::OtStoreType::Normal);
    kernel.eval_rot(ctx, padded, &rot_send);

    // Receive flip bits from receiver.
    auto flip_buf = ctx->Recv(ctx->NextRank(), "ot_flip");
    YACL_ENFORCE(flip_buf.size() ==
                 flip_words * static_cast<int64_t>(sizeof(uint64_t)));
    std::vector<uint64_t> flip_packed(flip_words);
    std::memcpy(flip_packed.data(), flip_buf.data(), flip_buf.size());

    // Compute and send corrections (only for real OTs).
    std::vector<uint64_t> corrections(2 * num_ots);
    for (int64_t i = 0; i < num_ots; ++i) {
      bool d = static_cast<bool>((flip_packed[i / 64] >> (i % 64)) & 1);
      auto s0 = static_cast<uint64_t>(rot_send.GetBlock(i, 0));
      auto s1 = static_cast<uint64_t>(rot_send.GetBlock(i, 1));
      uint64_t pad0 = d ? s1 : s0;
      uint64_t pad1 = d ? s0 : s1;
      corrections[(2 * i)] = sender_msgs[i].m0 ^ pad0;
      corrections[(2 * i) + 1] = sender_msgs[i].m1 ^ pad1;
    }
    ctx->SendAsync(ctx->NextRank(),
                   ByteContainerView(corrections.data(),
                                     corrections.size() * sizeof(uint64_t)),
                   "ot_corr");

    return {};
  }

  // Receiver side.
  YACL_ENFORCE(static_cast<int64_t>(receiver_choices.size()) == num_ots);

  crypto::OtRecvStore rot_recv(padded, crypto::OtStoreType::Normal);
  kernel.eval_rot(ctx, padded, &rot_recv);

  // Send flip bits (real choices for first num_ots, zeros for padding).
  std::vector<uint64_t> flip_packed(flip_words, 0);
  for (int64_t i = 0; i < num_ots; ++i) {
    bool rot_choice = static_cast<bool>(rot_recv.GetChoice(i));
    if (rot_choice != receiver_choices[i]) {
      flip_packed[i / 64] |= (1ULL << (i % 64));
    }
  }
  ctx->SendAsync(ctx->NextRank(),
                 ByteContainerView(flip_packed.data(),
                                   flip_packed.size() * sizeof(uint64_t)),
                 "ot_flip");

  // Receive corrections (only num_ots real corrections).
  auto corr_buf = ctx->Recv(ctx->NextRank(), "ot_corr");
  YACL_ENFORCE(corr_buf.size() ==
               2 * num_ots * static_cast<int64_t>(sizeof(uint64_t)));
  const auto* corrections = reinterpret_cast<const uint64_t*>(corr_buf.data());

  // Compute results.
  std::vector<uint64_t> results(num_ots);
  for (int64_t i = 0; i < num_ots; ++i) {
    auto rot_block = static_cast<uint64_t>(rot_recv.GetBlock(i));
    int desired_idx = receiver_choices[i] ? 1 : 0;
    results[i] = corrections[(2 * i) + desired_idx] ^ rot_block;
  }
  return results;
}

// ================================================================
// Preprocessing: Arithmetic Triples
// ================================================================

// Helper: run one direction of OT for arithmetic triple generation.
// The sender (who holds a_local) provides OT messages encoding a_local * 2^k.
// The receiver (who holds b_local) uses b_local's bits as choices.
// Returns the cross-product contribution for each triple.
std::vector<uint64_t> SSExecutor::RunArithOtDirection(
    bool is_sender, int64_t n, int64_t num_ots,
    const std::vector<uint64_t>& a_local,
    const std::vector<uint64_t>& b_local) {
  std::vector<uint64_t> cross(n, 0);

  std::vector<OtMessages> msgs;
  if (is_sender) {
    msgs.resize(num_ots);
    for (int64_t i = 0; i < n; ++i) {
      for (int64_t k = 0; k < kBits; ++k) {
        uint64_t r = crypto::FastRandU64();
        msgs[(i * kBits) + k].m0 = r;
        msgs[(i * kBits) + k].m1 = r + (a_local[i] << k);
        cross[i] -= r;
      }
    }
  }

  std::vector<bool> choices;
  if (!is_sender) {
    choices.resize(num_ots);
    for (int64_t i = 0; i < n; ++i) {
      for (int64_t k = 0; k < kBits; ++k) {
        choices[(i * kBits) + k] = static_cast<bool>((b_local[i] >> k) & 1);
      }
    }
  }

  auto recv = RunChosenOT(lctx_, is_sender, num_ots, msgs, choices);

  if (!is_sender) {
    for (int64_t i = 0; i < n; ++i) {
      for (int64_t k = 0; k < kBits; ++k) {
        cross[i] += recv[(i * kBits) + k];
      }
    }
  }
  return cross;
}

void SSExecutor::PreprocessArithTriples(int64_t n) {
  YACL_ENFORCE(n > 0);

  const int64_t num_ots = kBits * n;

  std::vector<uint64_t> a_local(n);
  std::vector<uint64_t> b_local(n);
  for (int64_t i = 0; i < n; ++i) {
    a_local[i] = crypto::FastRandU64();
    b_local[i] = crypto::FastRandU64();
  }

  // Direction A: rank-0 = OT sender (has a_0), rank-1 = OT receiver (has b_1).
  auto cross_a = RunArithOtDirection(rank_ == 0, n, num_ots, a_local, b_local);

  // Direction B: rank-1 = OT sender (has a_1), rank-0 = OT receiver (has b_0).
  auto cross_b = RunArithOtDirection(rank_ == 1, n, num_ots, a_local, b_local);

  for (int64_t i = 0; i < n; ++i) {
    uint64_t c = (a_local[i] * b_local[i]) + cross_a[i] + cross_b[i];
    arith_triples_.push_back(ATriple{a_local[i], b_local[i], c});
  }
}

// ================================================================
// Preprocessing: Boolean Triples
// ================================================================

// Helper: run one direction of OT for boolean triple generation.
std::vector<uint64_t> SSExecutor::RunBoolOtDirection(
    bool is_sender, int64_t n, int64_t num_ots,
    const std::vector<uint64_t>& a_local,
    const std::vector<uint64_t>& b_local) {
  std::vector<uint64_t> cross(n, 0);

  std::vector<SSExecutor::OtMessages> msgs;
  if (is_sender) {
    msgs.resize(num_ots);
    for (int64_t i = 0; i < n; ++i) {
      for (int64_t k = 0; k < kBits; ++k) {
        uint64_t r_bit = crypto::FastRandU64() & 1;
        uint64_t a_bit = (a_local[i] >> k) & 1;
        msgs[(i * kBits) + k].m0 = r_bit;
        msgs[(i * kBits) + k].m1 = r_bit ^ a_bit;
        cross[i] ^= (r_bit << k);
      }
    }
  }

  std::vector<bool> choices;
  if (!is_sender) {
    choices.resize(num_ots);
    for (int64_t i = 0; i < n; ++i) {
      for (int64_t k = 0; k < kBits; ++k) {
        choices[(i * kBits) + k] = static_cast<bool>((b_local[i] >> k) & 1);
      }
    }
  }

  auto recv = RunChosenOT(lctx_, is_sender, num_ots, msgs, choices);

  if (!is_sender) {
    for (int64_t i = 0; i < n; ++i) {
      for (int64_t k = 0; k < kBits; ++k) {
        cross[i] ^= ((recv[(i * kBits) + k] & 1) << k);
      }
    }
  }
  return cross;
}

void SSExecutor::PreprocessBoolTriples(int64_t n) {
  YACL_ENFORCE(n > 0);

  const int64_t num_ots = kBits * n;

  std::vector<uint64_t> a_local(n);
  std::vector<uint64_t> b_local(n);
  for (int64_t i = 0; i < n; ++i) {
    a_local[i] = crypto::FastRandU64();
    b_local[i] = crypto::FastRandU64();
  }

  // Direction A: rank-0 = OT sender, rank-1 = OT receiver.
  auto cross_a = RunBoolOtDirection(rank_ == 0, n, num_ots, a_local, b_local);

  // Direction B: rank-1 = OT sender, rank-0 = OT receiver.
  auto cross_b = RunBoolOtDirection(rank_ == 1, n, num_ots, a_local, b_local);

  for (int64_t i = 0; i < n; ++i) {
    uint64_t c = (a_local[i] & b_local[i]) ^ cross_a[i] ^ cross_b[i];
    bool_triples_.push_back(BTriple{a_local[i], b_local[i], c});
  }
}

// ================================================================
// Arithmetic Operations
// ================================================================

AShare SSExecutor::ShareA(uint64_t secret) {
  auto tag = NextTag("sha");
  if (rank_ == 0) {
    uint64_t r = crypto::FastRandU64();
    SendU64(tag, r);
    return AShare{secret - r};
  }
  return AShare{RecvU64(tag)};
}

uint64_t SSExecutor::RevealA(AShare share) {
  auto tag = NextTag("rva");
  SendU64(tag, share.v);
  uint64_t peer = RecvU64(tag);
  return share.v + peer;
}

AShare SSExecutor::AddA(AShare x, AShare y) { return AShare{x.v + y.v}; }

AShare SSExecutor::SubA(AShare x, AShare y) { return AShare{x.v - y.v}; }

AShare SSExecutor::NegA(AShare x) {
  return AShare{static_cast<uint64_t>(-static_cast<int64_t>(x.v))};
}

AShare SSExecutor::MulConstA(AShare x, uint64_t c) { return AShare{x.v * c}; }

AShare SSExecutor::MulA(AShare x, AShare y) {
  ATriple t = ConsumeATriple();

  uint64_t e_local = x.v - t.a;
  uint64_t f_local = y.v - t.b;

  auto tag = NextTag("mla");
  std::vector<uint64_t> send_ef = {e_local, f_local};
  SendU64Span(tag, std::span(send_ef));
  auto recv_ef = RecvU64Vec(tag, 2);

  uint64_t e = e_local + recv_ef[0];
  uint64_t f = f_local + recv_ef[1];

  uint64_t z = t.c + (e * t.b) + (f * t.a);
  if (rank_ == 0) {
    z += e * f;
  }
  return AShare{z};
}

// ================================================================
// Boolean Operations
// ================================================================

BShare SSExecutor::ShareB(uint64_t secret) {
  auto tag = NextTag("shb");
  if (rank_ == 0) {
    uint64_t r = crypto::FastRandU64();
    SendU64(tag, r);
    return BShare{secret ^ r};
  }
  return BShare{RecvU64(tag)};
}

uint64_t SSExecutor::RevealB(BShare share) {
  auto tag = NextTag("rvb");
  SendU64(tag, share.v);
  uint64_t peer = RecvU64(tag);
  return share.v ^ peer;
}

BShare SSExecutor::XorB(BShare x, BShare y) { return BShare{x.v ^ y.v}; }

BShare SSExecutor::NotB(BShare x) {
  if (rank_ == 0) {
    return BShare{~x.v};
  }
  return x;
}

BShare SSExecutor::AndB(BShare x, BShare y) {
  BTriple t = ConsumeBTriple();

  uint64_t e_local = x.v ^ t.a;
  uint64_t f_local = y.v ^ t.b;

  auto tag = NextTag("anb");
  std::vector<uint64_t> send_ef = {e_local, f_local};
  SendU64Span(tag, std::span(send_ef));
  auto recv_ef = RecvU64Vec(tag, 2);

  uint64_t e = e_local ^ recv_ef[0];
  uint64_t f = f_local ^ recv_ef[1];

  uint64_t z = t.c ^ (e & t.b) ^ (f & t.a);
  if (rank_ == 0) {
    z ^= (e & f);
  }
  return BShare{z};
}

// ================================================================
// A2B: Arithmetic to Boolean Conversion
// ================================================================
//
// Evaluates a 64-bit ripple-carry adder in boolean MPC.
// Input: arithmetic share x_i, where x_0 + x_1 = secret mod 2^64.
// Output: boolean share z_i, where z_0 ^ z_1 = secret.
//
// Consumes 64 boolean triples (1 batch generate + 63 carry chain).
//
BShare SSExecutor::A2B(AShare x) {
  YACL_ENFORCE(BoolTripleCount() >= kBits,
               "A2B requires {} boolean triples; only {} available", kBits,
               BoolTripleCount());

  // BShares of each party's arithmetic share bits.
  // rank-0: A=x.v, B=0. rank-1: A=0, B=x.v.
  BShare a_packed = (rank_ == 0) ? BShare{x.v} : BShare{0};
  BShare b_packed = (rank_ == 1) ? BShare{x.v} : BShare{0};

  // Batch compute generate[k] = A[k] AND B[k] for all 64 bits at once.
  BShare gen_packed = AndB(a_packed, b_packed);  // 1 triple

  // Propagate is local XOR: propagate[k] = A[k] XOR B[k].
  BShare prop_packed = XorB(a_packed, b_packed);

  // Carry chain and sum bits in a single pass.
  uint64_t result = 0;

  // sum[0] = prop[0] (no carry-in for bit 0).
  result |= (prop_packed.v & 1);

  // carry[0] = generate[0].
  BShare carry{gen_packed.v & 1};

  for (int64_t k = 1; k < kBits; ++k) {
    // sum[k] = prop[k] XOR carry[k-1] (local operation on single-bit shares).
    uint64_t sum_k = ((prop_packed.v >> k) & 1) ^ (carry.v & 1);
    result |= (sum_k << k);

    // carry[k] = gen[k] XOR (prop[k] AND carry[k-1]).
    BShare prop_k{(prop_packed.v >> k) & 1};
    BShare gen_k{(gen_packed.v >> k) & 1};
    BShare prop_and_carry = AndB(prop_k, carry);  // 1 triple
    carry = XorB(gen_k, prop_and_carry);
  }

  return BShare{result};
}

// ================================================================
// B2A: Boolean to Arithmetic Conversion
// ================================================================
//
// OT-based per-bit conversion (ABY construction).
// Input: boolean share x_i, where x_0 ^ x_1 = secret.
// Output: arithmetic share a_i, where a_0 + a_1 = secret mod 2^64.
//
// Uses 64 fresh OT instances (no triple consumption).
//
AShare SSExecutor::B2A(BShare x) {
  // rank-1 is OT sender (knows b_{1,k}), rank-0 is OT receiver
  // (choice=b_{0,k}).
  bool is_sender = (rank_ == 1);

  std::vector<OtMessages> msgs;
  uint64_t sender_share = 0;

  if (is_sender) {
    msgs.resize(kBits);
    for (int64_t k = 0; k < kBits; ++k) {
      uint64_t b1_k = (x.v >> k) & 1;
      uint64_t r_k = crypto::FastRandU64();
      uint64_t weight = 1ULL << k;
      // (1 - 2*b1_k) * weight: if b1_k=0 then +weight; if b1_k=1 then -weight.
      uint64_t correction =
          (b1_k != 0U) ? static_cast<uint64_t>(-weight) : weight;
      msgs[k].m0 = r_k;
      msgs[k].m1 = r_k + correction;
      sender_share += (b1_k * weight) - r_k;
    }
  }

  std::vector<bool> choices;
  if (!is_sender) {
    choices.resize(kBits);
    for (int64_t k = 0; k < kBits; ++k) {
      choices[k] = static_cast<bool>((x.v >> k) & 1);
    }
  }

  auto recv_vals = RunChosenOT(lctx_, is_sender, kBits, msgs, choices);

  if (is_sender) {
    return AShare{sender_share};
  }

  uint64_t receiver_share = 0;
  for (int64_t k = 0; k < kBits; ++k) {
    receiver_share += recv_vals[k];
  }
  return AShare{receiver_share};
}

}  // namespace yacl::engine
