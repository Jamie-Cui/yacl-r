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

#include <array>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "absl/strings/escaping.h"
#include "spdlog/spdlog.h"

#include "yacl/base/byte_container_view.h"
#include "yacl/base/exception.h"
#include "yacl/io/stream/file_io.h"
#include "yacl/io/stream/interface.h"
#include "yacl/utils/spi/type_traits.h"

namespace yacl::io {

// Bristol Fashion Circuit
// see: https://nigelsmart.github.io/MPC-Circuits/
class BFCircuit {
 public:
  uint32_t ng = 0;            // number of gates
  uint32_t nw = 0;            // number of wires
  uint32_t niv;               // number of input values
  std::vector<uint32_t> niw;  // number of wires per each input values
  uint32_t nov;               // number of output values
  std::vector<uint32_t> now;  // number of wires per each output values

  // circuit oeprations
  enum class Op { XOR, AND, INV, EQ, EQW, MAND };

  // Gate definition
  class Gate {
   public:
    uint32_t niw = 0;          // numer of input wires
    uint32_t now = 0;          // number of output wires
    std::vector<uint32_t> iw;  // lists of input wires
    std::vector<uint32_t> ow;  // lists of output wires
    Op op;
  };

  void PrintSummary();

  std::vector<Gate> gates;
};

// bristol fashion circuit file reader
class CircuitReader {
 public:
  explicit CircuitReader(const std::string &path) {
    Reset();
    Init(path);
  }

  ~CircuitReader() = default;

  void ReadMeta();
  void ReadAllGates();
  void ReadAll() { ReadAllGates(); }

  std::unique_ptr<BFCircuit> StealCirc() {
    YACL_ENFORCE(circ_ != nullptr);
    return std::move(circ_);
  }

  void Reset() {
    if (in_ != nullptr) {
      in_->Close();
    }
    if (circ_ != nullptr) {
      circ_.reset();
    }
  }

  void Init(const std::string &path) {
    in_ = std::unique_ptr<io::InputStream>(new io::FileInputStream(path));
    circ_ = std::make_unique<BFCircuit>();  // get a new circ instance
  }

 private:
  std::unique_ptr<BFCircuit> circ_;

  // io-related infos
  std::unique_ptr<io::InputStream> in_;
  size_t metadata_length_ = 0;
};

class BuiltinBFCircuit {
 public:
  static std::string Add64Path() {
    return fmt::format("{}/yacl/io/circuit/data/adder64.txt",
                       std::filesystem::current_path().string());
  }

  static std::string Sub64Path() {
    return fmt::format("{}/yacl/io/circuit/data/sub64.txt",
                       std::filesystem::current_path().string());
  }

  static std::string Neg64Path() {
    return fmt::format("{}/yacl/io/circuit/data/neg64.txt",
                       std::filesystem::current_path().string());
  }

  static std::string Mul64Path() {
    return fmt::format("{}/yacl/io/circuit/data/mult64.txt",
                       std::filesystem::current_path().string());
  }

  static std::string Div64Path() {
    return fmt::format("{}/yacl/io/circuit/data/divide64.txt",
                       std::filesystem::current_path().string());
  }

  static std::string UDiv64Path() {
    return fmt::format("{}/yacl/io/circuit/data/udivide64.txt",
                       std::filesystem::current_path().string());
  }

  static std::string EqzPath() {
    return fmt::format("{}/yacl/io/circuit/data/zero_equal.txt",
                       std::filesystem::current_path().string());
  }

  // Prepare (append & tweak) the input sha256 message before fed to the sha256
  // bristol circuit.
  //
  // For more details, please check:
  // https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf
  //
  // NOTE since we are using dynamic_bitset for bristol format circuit
  // representation, the actual bit operation here is slightly different from
  // the standards.
  static std::vector<uint8_t> PrepareSha256Input(ByteContainerView input) {
    constexpr size_t kFixPadSize = 1;                 // in bytes
    constexpr size_t kMsgLenSize = sizeof(uint64_t);  // in bytes
    constexpr size_t kMsgBlockSize = 64;              // in bytes
    auto kInitSha256Bytes = GetSha256InitialHashValues();

    uint64_t input_size = input.size();  // in bits
    uint64_t zero_padding_size =
        (input_size + kFixPadSize + kMsgLenSize) % kMsgBlockSize == 0
            ? 0
            : kMsgBlockSize -
                  (input_size + kFixPadSize + kMsgLenSize) % kMsgBlockSize;
    uint64_t message_size =
        input_size + kFixPadSize + zero_padding_size + kMsgLenSize;
    uint64_t result_size = message_size + kInitSha256Bytes.size();

    YACL_ENFORCE(message_size % kMsgBlockSize == 0);

    // Declare the result byte-vector
    size_t offset = 0;
    std::vector<uint8_t> result(result_size);

    // the next 64 bits should be the byte length of input message
    uint64_t input_bitnum = input_size * 8;  // in bytes
    std::memcpy(result.data() + offset, &input_bitnum, sizeof(input_bitnum));
    offset += sizeof(uint64_t);

    // zero padding (result vector has zero initialization)
    // ... should doing nothing ...
    offset += zero_padding_size;

    // additional padding bit-'1' (as a mark)
    result[offset] = 0x80;
    offset += kFixPadSize;

    // original input message
    // auto input_reverse = ReverseBytes(absl::MakeSpan(input));  // copy here
    auto input_reverse = std::vector<uint8_t>(input.begin(), input.end());
    std::reverse(input_reverse.begin(), input_reverse.end());
    std::memcpy(result.data() + offset, input_reverse.data(), input_size);
    offset += input_size;

    // initial hash values
    std::memcpy(result.data() + offset, kInitSha256Bytes.data(),
                kInitSha256Bytes.size());
    offset += kInitSha256Bytes.size();

    return result;
  }

  // NOTE: For AES-128 the wire orders are in the reverse order as used in
  // the examples given in our earlier `Bristol Format', thus bit 0 becomes bit
  // 127 etc, for key, plaintext and message.
  //
  // see: https://nigelsmart.github.io/MPC-Circuits/
  static std::string Aes128Path() {
    return fmt::format("{}/yacl/io/circuit/data/aes_128.txt",
                       std::filesystem::current_path().string());
  }

  // NOTE: sha256 needs two inputs, a 512 bit buffer, and a 256 bit previous
  // digest value
  //
  static std::string Sha256Path() {
    return fmt::format("{}/yacl/io/circuit/data/sha256.txt",
                       std::filesystem::current_path().string());
  }

  static std::array<uint8_t, 32> GetSha256InitialHashValues() {
    std::array<uint8_t, 32> standard_init_array = {
        0x6a, 0x09, 0xe6, 0x67, 0xbb, 0x67, 0xae, 0x85, 0x3c, 0x6e, 0xf3,
        0x72, 0xa5, 0x4f, 0xf5, 0x3a, 0x51, 0x0e, 0x52, 0x7f, 0x9b, 0x05,
        0x68, 0x8c, 0x1f, 0x83, 0xd9, 0xab, 0x5b, 0xe0, 0xcd, 0x19};
    std::reverse(standard_init_array.begin(), standard_init_array.end());
    return standard_init_array;
  }
};

}  // namespace yacl::io
