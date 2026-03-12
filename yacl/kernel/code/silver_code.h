// Copyright 2023 Ant Group Co., Ltd.
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

#include <algorithm>
#include <array>
#include <cstdint>
#include <set>
#include <vector>

#include <span>
#include "code_interface.h"
#include "spdlog/spdlog.h"

#include "yacl/base/block.h"
#include "yacl/base/exception.h"
#include "yacl/base/int128.h"
#include "yacl/kernel/code/code_interface.h"

namespace yacl::crypto {

// Implementation of Low-density Parity-check Code for SILent Vole and transfER
// over F2k, for more details, see original paper:
// https://eprint.iacr.org/2021/1150
//
// Warning: Silver Code is NOT recommended, more detail counld be found in
// paper: https://eprint.iacr.org/2023/882.pdf
//
// Implementation mostly from:
// https://github.com/osu-crypto/libOTe/blob/master/libOTe/Tools/LDPC/LdpcEncoder.h
//

class SilverCode : public LinearCodeInterface {
 public:
  explicit SilverCode(uint64_t n, uint32_t weight = 5);

  uint32_t GetDimention() const override { return m_; }

  uint32_t GetLength() const override { return n_; }

  uint32_t GetWeight() const { return weight_; }

  // uint128_t interface
  void DualEncodeInplace(std::span<uint128_t> inout) const {
    DualEncodeInplaceImpl(inout);
  }

  void DualEncode(std::span<const uint128_t> in,
                  std::span<uint128_t> out) const {
    DualEncodeImpl(in, out);
  }

  void DualEncodeInplace2(std::span<uint128_t> inout0 /* GF(2^128) */,
                          std::span<uint128_t> inout1 /* GF(2^128) */) const {
    DualEncodeInplace2Impl(inout0, inout1);
  }

  void DualEncode2(std::span<const uint128_t> in0 /* GF(2^128) */,
                   std::span<uint128_t> out0,
                   std::span<const uint128_t> in1 /* GF(2^128) */,
                   std::span<uint128_t> out1) const {
    DualEncode2Impl(in0, out0, in1, out1);
  }

  // uint64_t interface
  void DualEncodeInplace(std::span<uint64_t> inout) const {
    DualEncodeInplaceImpl(inout);
  }

  void DualEncode(std::span<const uint64_t> in,
                  std::span<uint64_t> out) const {
    DualEncodeImpl(in, out);
  }

  void DualEncodeInplace2(std::span<uint64_t> inout0 /* GF(2^64) */,
                          std::span<uint64_t> inout1 /* GF(2^64) */) const {
    DualEncodeInplace2Impl(inout0, inout1);
  }

  void DualEncodeInplace2(std::span<uint64_t> inout0 /* GF(2^64) */,
                          std::span<uint128_t> inout1 /* GF(2^128) */) const {
    DualEncodeInplace2Impl(inout0, inout1);
  }

  void DualEncode2(std::span<const uint64_t> in0 /* GF(2^64) */,
                   std::span<uint64_t> out0,
                   std::span<const uint64_t> in1 /* GF(2^64) */,
                   std::span<uint64_t> out1) const {
    DualEncode2Impl(in0, out0, in1, out1);
  }

  void DualEncode2(std::span<const uint64_t> in0 /* GF(2^64) */,
                   std::span<uint64_t> out0,
                   std::span<const uint128_t> in1 /* GF(2^128) */,
                   std::span<uint128_t> out1) const {
    DualEncode2Impl(in0, out0, in1, out1);
  }

 private:
  uint32_t n_;
  uint32_t m_;
  uint32_t weight_;
  uint32_t gap_;
  std::vector<uint32_t> L_one_idx_;

  void InitLeftMatrix(std::span<const double> R);

  // LDPC for SILent Vole and transfER
  // LPN problem  --> H = [L R]
  // where L is a d-local-linear code, R is a lower triangle matrix
  // dual version --> G = [   I
  //                       -R^{-1}*L]
  // satisfying that H * G = 0
  // thus, dual encode would be: xG = x[0:n] - x[n:2n] * R^{-1} * L
  template <typename T>
  void DualEncodeInplaceImpl(std::span<T> inout) const;

  template <typename T>
  void DualEncodeImpl(std::span<const T> in, std::span<T> out) const;

  template <typename T, typename K>
  void DualEncodeInplace2Impl(std::span<T> inout0, std::span<K> inout1) const;

  template <typename T, typename K>
  void DualEncode2Impl(std::span<const T> in0, std::span<T> out0,
                       std::span<const K> in1, std::span<K> out1) const;

  // L and R matrix Encode
  template <typename T>
  void LeftEncode(std::span<const T> in, std::span<T> out) const;

  template <typename T, typename K>
  void LeftEncode2(std::span<const T> in0, std::span<T> out0,
                   std::span<const K> in1, std::span<K> out1) const;

  template <typename T>
  void RightEncode(std::span<T> inout) const;

  template <typename T, typename K>
  void RightEncode2(std::span<T> inout0, std::span<K> inout1) const;
};

};  // namespace yacl::crypto
