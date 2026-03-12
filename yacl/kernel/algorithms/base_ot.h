// Copyright 2022 Ant Group Co., Ltd.
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

#include <memory>
#include <vector>

#include <span>

#include "yacl/base/dynamic_bitset.h"
#include "yacl/base/int128.h"
#include "yacl/base/secparam.h"
#include "yacl/kernel/type/ot_store_utils.h"
#include "yacl/link/link.h"

/* submodules */
#if defined(__linux__) && defined(__x86_64)
#include "yacl/kernel/algorithms/x86_asm_ot_interface.h"
#else
#include "yacl/kernel/algorithms/portable_ot_interface.h"
#endif

/* security parameter declaration */
// this module is only a wrapper, no need for security parameter definition

namespace yacl::crypto {

using Block = uint128_t;

void BaseOtRecv(const std::shared_ptr<link::Context>& ctx,
                const dynamic_bitset<uint128_t>& choices,
                std::span<Block> recv_blocks);

void BaseOtSend(const std::shared_ptr<link::Context>& ctx,
                std::span<std::array<Block, 2>> send_blocks);

// ==================== //
//   Support OT Store   //
// ==================== //

inline OtRecvStore BaseOtRecv(const std::shared_ptr<link::Context>& ctx,
                              const dynamic_bitset<uint128_t>& choices,
                              uint32_t num_ot) {
  std::vector<Block> blocks(num_ot);
  BaseOtRecv(ctx, choices, std::span(blocks));
  return MakeOtRecvStore(choices, std::move(blocks));
}

inline OtSendStore BaseOtSend(const std::shared_ptr<link::Context>& ctx,
                              uint32_t num_ot) {
  std::vector<std::array<Block, 2>> blocks(num_ot);
  BaseOtSend(ctx, std::span(blocks));
  return MakeOtSendStore(std::move(blocks));
}

}  // namespace yacl::crypto
