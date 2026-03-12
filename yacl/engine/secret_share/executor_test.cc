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
#include <future>
#include <memory>
#include <vector>

#include "gtest/gtest.h"

#include "yacl/crypto/rand/rand.h"
#include "yacl/link/test_util.h"

namespace yacl::engine {
namespace {

// Helper: run a 2-party protocol, returning results from both parties.
template <typename Func>
std::pair<uint64_t, uint64_t> Run2Party(Func&& func) {
  auto contexts = link::test::SetupWorld(2);
  std::future<uint64_t> f0 =
      std::async(std::launch::async, func, 0, contexts[0]);
  std::future<uint64_t> f1 =
      std::async(std::launch::async, func, 1, contexts[1]);
  return {f0.get(), f1.get()};
}

// ---------------------------------------------------------------
// Arithmetic Share / Reveal
// ---------------------------------------------------------------

TEST(SSExecutorTest, ShareAndRevealA) {
  uint64_t secret = crypto::FastRandU64();

  auto [r0, r1] =
      Run2Party([secret](size_t rank, std::shared_ptr<link::Context> lctx) {
        SSExecutor eng(rank, lctx);
        AShare s = eng.ShareA(rank == 0 ? secret : 0);
        return eng.RevealA(s);
      });

  EXPECT_EQ(r0, secret);
  EXPECT_EQ(r1, secret);
}

// ---------------------------------------------------------------
// Arithmetic Local Operations
// ---------------------------------------------------------------

TEST(SSExecutorTest, AddA) {
  uint64_t a = crypto::FastRandU64();
  uint64_t b = crypto::FastRandU64();

  auto [r0, r1] =
      Run2Party([a, b](size_t rank, std::shared_ptr<link::Context> lctx) {
        SSExecutor eng(rank, lctx);
        AShare sa = eng.ShareA(rank == 0 ? a : 0);
        AShare sb = eng.ShareA(rank == 0 ? b : 0);
        AShare sc = eng.AddA(sa, sb);
        return eng.RevealA(sc);
      });

  EXPECT_EQ(r0, a + b);
  EXPECT_EQ(r1, a + b);
}

TEST(SSExecutorTest, SubA) {
  uint64_t a = crypto::FastRandU64();
  uint64_t b = crypto::FastRandU64();

  auto [r0, r1] =
      Run2Party([a, b](size_t rank, std::shared_ptr<link::Context> lctx) {
        SSExecutor eng(rank, lctx);
        AShare sa = eng.ShareA(rank == 0 ? a : 0);
        AShare sb = eng.ShareA(rank == 0 ? b : 0);
        AShare sc = eng.SubA(sa, sb);
        return eng.RevealA(sc);
      });

  EXPECT_EQ(r0, a - b);
  EXPECT_EQ(r1, a - b);
}

TEST(SSExecutorTest, NegA) {
  uint64_t a = crypto::FastRandU64();

  auto [r0, r1] =
      Run2Party([a](size_t rank, std::shared_ptr<link::Context> lctx) {
        SSExecutor eng(rank, lctx);
        AShare sa = eng.ShareA(rank == 0 ? a : 0);
        AShare sc = eng.NegA(sa);
        return eng.RevealA(sc);
      });

  EXPECT_EQ(r0, static_cast<uint64_t>(-static_cast<int64_t>(a)));
  EXPECT_EQ(r1, static_cast<uint64_t>(-static_cast<int64_t>(a)));
}

TEST(SSExecutorTest, MulConstA) {
  uint64_t a = crypto::FastRandU64();
  uint64_t c = crypto::FastRandU64();

  auto [r0, r1] =
      Run2Party([a, c](size_t rank, std::shared_ptr<link::Context> lctx) {
        SSExecutor eng(rank, lctx);
        AShare sa = eng.ShareA(rank == 0 ? a : 0);
        AShare sc = eng.MulConstA(sa, c);
        return eng.RevealA(sc);
      });

  EXPECT_EQ(r0, a * c);
  EXPECT_EQ(r1, a * c);
}

// ---------------------------------------------------------------
// Arithmetic Multiplication (Online)
// ---------------------------------------------------------------

TEST(SSExecutorTest, MulA) {
  uint64_t a = crypto::FastRandU64();
  uint64_t b = crypto::FastRandU64();

  auto [r0, r1] =
      Run2Party([a, b](size_t rank, std::shared_ptr<link::Context> lctx) {
        SSExecutor eng(rank, lctx);
        eng.PreprocessArithTriples(1);
        AShare sa = eng.ShareA(rank == 0 ? a : 0);
        AShare sb = eng.ShareA(rank == 0 ? b : 0);
        AShare sc = eng.MulA(sa, sb);
        return eng.RevealA(sc);
      });

  EXPECT_EQ(r0, a * b);
  EXPECT_EQ(r1, a * b);
}

// ---------------------------------------------------------------
// Boolean Share / Reveal
// ---------------------------------------------------------------

TEST(SSExecutorTest, ShareAndRevealB) {
  uint64_t secret = crypto::FastRandU64();

  auto [r0, r1] =
      Run2Party([secret](size_t rank, std::shared_ptr<link::Context> lctx) {
        SSExecutor eng(rank, lctx);
        BShare s = eng.ShareB(rank == 0 ? secret : 0);
        return eng.RevealB(s);
      });

  EXPECT_EQ(r0, secret);
  EXPECT_EQ(r1, secret);
}

// ---------------------------------------------------------------
// Boolean Local Operations
// ---------------------------------------------------------------

TEST(SSExecutorTest, XorB) {
  uint64_t a = crypto::FastRandU64();
  uint64_t b = crypto::FastRandU64();

  auto [r0, r1] =
      Run2Party([a, b](size_t rank, std::shared_ptr<link::Context> lctx) {
        SSExecutor eng(rank, lctx);
        BShare sa = eng.ShareB(rank == 0 ? a : 0);
        BShare sb = eng.ShareB(rank == 0 ? b : 0);
        BShare sc = eng.XorB(sa, sb);
        return eng.RevealB(sc);
      });

  EXPECT_EQ(r0, a ^ b);
  EXPECT_EQ(r1, a ^ b);
}

TEST(SSExecutorTest, NotB) {
  uint64_t a = crypto::FastRandU64();

  auto [r0, r1] =
      Run2Party([a](size_t rank, std::shared_ptr<link::Context> lctx) {
        SSExecutor eng(rank, lctx);
        BShare sa = eng.ShareB(rank == 0 ? a : 0);
        BShare sc = eng.NotB(sa);
        return eng.RevealB(sc);
      });

  EXPECT_EQ(r0, ~a);
  EXPECT_EQ(r1, ~a);
}

// ---------------------------------------------------------------
// Boolean AND (Online)
// ---------------------------------------------------------------

TEST(SSExecutorTest, AndB) {
  uint64_t a = crypto::FastRandU64();
  uint64_t b = crypto::FastRandU64();

  auto [r0, r1] =
      Run2Party([a, b](size_t rank, std::shared_ptr<link::Context> lctx) {
        SSExecutor eng(rank, lctx);
        eng.PreprocessBoolTriples(1);
        BShare sa = eng.ShareB(rank == 0 ? a : 0);
        BShare sb = eng.ShareB(rank == 0 ? b : 0);
        BShare sc = eng.AndB(sa, sb);
        return eng.RevealB(sc);
      });

  EXPECT_EQ(r0, a & b);
  EXPECT_EQ(r1, a & b);
}

// ---------------------------------------------------------------
// A2B Conversion
// ---------------------------------------------------------------

TEST(SSExecutorTest, A2B) {
  uint64_t secret = crypto::FastRandU64();

  auto [r0, r1] =
      Run2Party([secret](size_t rank, std::shared_ptr<link::Context> lctx) {
        SSExecutor eng(rank, lctx);
        eng.PreprocessBoolTriples(64);  // A2B needs 64 bool triples
        AShare sa = eng.ShareA(rank == 0 ? secret : 0);
        BShare sb = eng.A2B(sa);
        return eng.RevealB(sb);
      });

  EXPECT_EQ(r0, secret);
  EXPECT_EQ(r1, secret);
}

// ---------------------------------------------------------------
// B2A Conversion
// ---------------------------------------------------------------

TEST(SSExecutorTest, B2A) {
  uint64_t secret = crypto::FastRandU64();

  auto [r0, r1] =
      Run2Party([secret](size_t rank, std::shared_ptr<link::Context> lctx) {
        SSExecutor eng(rank, lctx);
        BShare sb = eng.ShareB(rank == 0 ? secret : 0);
        AShare sa = eng.B2A(sb);
        return eng.RevealA(sa);
      });

  EXPECT_EQ(r0, secret);
  EXPECT_EQ(r1, secret);
}

// ---------------------------------------------------------------
// Roundtrip Conversions
// ---------------------------------------------------------------

TEST(SSExecutorTest, RoundtripA2B2A) {
  uint64_t secret = crypto::FastRandU64();

  auto [r0, r1] =
      Run2Party([secret](size_t rank, std::shared_ptr<link::Context> lctx) {
        SSExecutor eng(rank, lctx);
        eng.PreprocessBoolTriples(64);
        AShare sa = eng.ShareA(rank == 0 ? secret : 0);
        BShare sb = eng.A2B(sa);
        AShare sa2 = eng.B2A(sb);
        return eng.RevealA(sa2);
      });

  EXPECT_EQ(r0, secret);
  EXPECT_EQ(r1, secret);
}

TEST(SSExecutorTest, RoundtripB2A2B) {
  uint64_t secret = crypto::FastRandU64();

  auto [r0, r1] =
      Run2Party([secret](size_t rank, std::shared_ptr<link::Context> lctx) {
        SSExecutor eng(rank, lctx);
        eng.PreprocessBoolTriples(64);  // needed for A2B inside roundtrip
        BShare sb = eng.ShareB(rank == 0 ? secret : 0);
        AShare sa = eng.B2A(sb);
        BShare sb2 = eng.A2B(sa);
        return eng.RevealB(sb2);
      });

  EXPECT_EQ(r0, secret);
  EXPECT_EQ(r1, secret);
}

// ---------------------------------------------------------------
// Triple Count Inspection
// ---------------------------------------------------------------

TEST(SSExecutorTest, TripleCountTracking) {
  auto contexts = link::test::SetupWorld(2);

  auto check = [](size_t rank, std::shared_ptr<link::Context> lctx) {
    SSExecutor eng(rank, lctx);

    EXPECT_EQ(eng.ArithTripleCount(), 0);
    EXPECT_EQ(eng.BoolTripleCount(), 0);

    eng.PreprocessArithTriples(5);
    EXPECT_EQ(eng.ArithTripleCount(), 5);

    eng.PreprocessBoolTriples(10);
    EXPECT_EQ(eng.BoolTripleCount(), 10);

    // Consuming a triple decrements the count.
    AShare sa = eng.ShareA(rank == 0 ? 42ULL : 0ULL);
    AShare sb = eng.ShareA(rank == 0 ? 7ULL : 0ULL);
    eng.MulA(sa, sb);
    EXPECT_EQ(eng.ArithTripleCount(), 4);
  };

  std::future<void> f0 = std::async(std::launch::async, check, 0, contexts[0]);
  std::future<void> f1 = std::async(std::launch::async, check, 1, contexts[1]);
  f0.get();
  f1.get();
}

}  // namespace
}  // namespace yacl::engine
