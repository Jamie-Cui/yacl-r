// Copyright 2025 Jamie Cui
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

#include "yacl/base/byte_container_view.h"
#include "yacl/base/dynamic_bitset.h"
#include "yacl/base/int128.h"
#include "yacl/io/circuit/bristol_fashion.h"

namespace yacl::engine {

// plaintext protocol that executes everything without link

class PlainExecutor {
 public:
  using BlockType = uint8_t;

  // Constructor
  explicit PlainExecutor() = default;

  // Load circuit from file (local operation)
  void LoadCircuitFile(const std::string &path);

  // Setup the input wire (local operation)
  template <typename T = uint64_t>
  void SetupInputs(absl::Span<T> inputs) {
    YACL_ENFORCE(inputs.size() == circ_->niv);

    dynamic_bitset<BlockType> input_wires;
    input_wires.resize(sizeof(T) * 8 * inputs.size());
    std::memcpy(input_wires.data(), inputs.data(), inputs.size() * sizeof(T));
    wires_.append(input_wires);
    wires_.resize(circ_->nw);
  }

  // Setup the input wire
  //
  // NOTE internally this function simply copies the memory of bytes to internal
  // dynamic_bitset
  void SetupInputBytes(ByteContainerView bytes) {
    wires_.resize(bytes.size() * 8);
    std::memcpy(wires_.data(), bytes.data(), bytes.size());
    wires_.resize(circ_->nw);
  }

  // Execute the circuit
  void Exec();

  // Finalize and get the result
  template <typename T = uint64_t>
  void Finalize(absl::Span<T> outputs) {
    YACL_ENFORCE(outputs.size() >= circ_->nov);
    size_t index = wires_.size();
    for (size_t i = 0; i < circ_->nov; ++i) {
      dynamic_bitset<BlockType> result(circ_->now[i]);
      for (size_t j = 0; j < circ_->now[i]; ++j) {
        result[j] = wires_[index - circ_->now[i] + j];
      }
      outputs[circ_->nov - i - 1] = *(T *)result.data();
      index -= circ_->now[i];
    }
  }

  std::vector<uint8_t> FinalizeBytes() {
    // Count the totoal number of output wires (a.k.a. output bits)
    size_t total_out_bitnum = 0;
    for (size_t i = 0; i < circ_->nov; ++i) {
      total_out_bitnum += circ_->now[i];
    }


    std::vector<uint8_t> out(total_out_bitnum / 8);

    size_t index = wires_.size();
    for (size_t i = 0; i < 32; ++i) {
      dynamic_bitset<BlockType> result(8);
      for (size_t j = 0; j < 8; ++j) {
        result[j] = wires_[index - 8 + j];
      }
      out[32 - i - 1] = *(uint8_t *)result.data();
      index -= 8;
    }
    std::reverse(out.begin(), out.end());
    return out;
  }

 private:
  // NOTE: please make sure you use the correct order of wires
  dynamic_bitset<BlockType> wires_;      // shares
  std::shared_ptr<io::BFCircuit> circ_;  // bristol fashion circuit
};

}  // namespace yacl::engine
