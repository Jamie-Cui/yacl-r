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

#include "yacl/engine/snark/serialization.h"

#include <cstring>

#include "yacl/base/exception.h"

namespace yacl::engine::snark {

namespace {

// Helper: write a uint64 to buffer at offset.
void WriteU64(uint8_t* buf, int64_t offset, uint64_t val) {
  std::memcpy(buf + offset, &val, sizeof(uint64_t));
}

// Helper: read a uint64 from buffer at offset.
uint64_t ReadU64(const uint8_t* buf, int64_t offset) {
  uint64_t val = 0;
  std::memcpy(&val, buf + offset, sizeof(uint64_t));
  return val;
}

}  // namespace

Buffer SerializeProof(const Proof& proof,
                      const std::shared_ptr<crypto::PairingGroup>& pairing) {
  auto g1 = pairing->GetGroup1();
  auto g2 = pairing->GetGroup2();

  auto a_buf = g1->SerializePoint(proof.a);
  auto b_buf = g2->SerializePoint(proof.b);
  auto c_buf = g1->SerializePoint(proof.c);

  // Format: [a_len:8][a_data][b_len:8][b_data][c_len:8][c_data]
  int64_t total = 3 * static_cast<int64_t>(sizeof(uint64_t)) + a_buf.size() +
                  b_buf.size() + c_buf.size();
  Buffer result(total);
  auto* ptr = result.data<uint8_t>();
  int64_t offset = 0;

  WriteU64(ptr, offset, static_cast<uint64_t>(a_buf.size()));
  offset += sizeof(uint64_t);
  std::memcpy(ptr + offset, a_buf.data(), a_buf.size());
  offset += a_buf.size();

  WriteU64(ptr, offset, static_cast<uint64_t>(b_buf.size()));
  offset += sizeof(uint64_t);
  std::memcpy(ptr + offset, b_buf.data(), b_buf.size());
  offset += b_buf.size();

  WriteU64(ptr, offset, static_cast<uint64_t>(c_buf.size()));
  offset += sizeof(uint64_t);
  std::memcpy(ptr + offset, c_buf.data(), c_buf.size());

  return result;
}

Proof DeserializeProof(ByteContainerView buf,
                       const std::shared_ptr<crypto::PairingGroup>& pairing) {
  auto g1 = pairing->GetGroup1();
  auto g2 = pairing->GetGroup2();

  const auto* ptr = reinterpret_cast<const uint8_t*>(buf.data());
  int64_t remaining = static_cast<int64_t>(buf.size());
  int64_t offset = 0;

  YACL_ENFORCE(remaining >= static_cast<int64_t>(sizeof(uint64_t)),
               "Buffer too small for proof deserialization");
  uint64_t a_len = ReadU64(ptr, offset);
  offset += sizeof(uint64_t);
  remaining -= sizeof(uint64_t);

  YACL_ENFORCE(remaining >= static_cast<int64_t>(a_len),
               "Buffer too small for proof.a");
  auto a_point = g1->DeserializePoint(
      ByteContainerView(ptr + offset, static_cast<size_t>(a_len)));
  offset += static_cast<int64_t>(a_len);
  remaining -= static_cast<int64_t>(a_len);

  YACL_ENFORCE(remaining >= static_cast<int64_t>(sizeof(uint64_t)),
               "Buffer too small for b_len");
  uint64_t b_len = ReadU64(ptr, offset);
  offset += sizeof(uint64_t);
  remaining -= sizeof(uint64_t);

  YACL_ENFORCE(remaining >= static_cast<int64_t>(b_len),
               "Buffer too small for proof.b");
  auto b_point = g2->DeserializePoint(
      ByteContainerView(ptr + offset, static_cast<size_t>(b_len)));
  offset += static_cast<int64_t>(b_len);
  remaining -= static_cast<int64_t>(b_len);

  YACL_ENFORCE(remaining >= static_cast<int64_t>(sizeof(uint64_t)),
               "Buffer too small for c_len");
  uint64_t c_len = ReadU64(ptr, offset);
  offset += sizeof(uint64_t);
  remaining -= sizeof(uint64_t);

  YACL_ENFORCE(remaining >= static_cast<int64_t>(c_len),
               "Buffer too small for proof.c");
  auto c_point = g1->DeserializePoint(
      ByteContainerView(ptr + offset, static_cast<size_t>(c_len)));

  return Proof{std::move(a_point), std::move(b_point), std::move(c_point)};
}

Buffer SerializeVerificationKey(const VerificationKey& vk) {
  auto g1 = vk.pairing->GetGroup1();
  auto g2 = vk.pairing->GetGroup2();
  auto gt = vk.pairing->GetGroupT();

  auto alpha_buf = g1->SerializePoint(vk.alpha_g1);
  auto beta_buf = g2->SerializePoint(vk.beta_g2);
  auto gamma_buf = g2->SerializePoint(vk.gamma_g2);
  auto delta_buf = g2->SerializePoint(vk.delta_g2);

  YACL_ENFORCE(vk.alpha_beta_gt.has_value());
  auto abt_buf = gt->Serialize(*vk.alpha_beta_gt);

  // Serialize IC points
  std::vector<Buffer> ic_bufs;
  ic_bufs.reserve(vk.ic.size());
  int64_t ic_total = 0;
  for (const auto& pt : vk.ic) {
    ic_bufs.push_back(g1->SerializePoint(pt));
    ic_total += ic_bufs.back().size() + static_cast<int64_t>(sizeof(uint64_t));
  }

  // Format: [num_fields:8]
  //         [alpha_len:8][alpha][beta_len:8][beta][gamma_len:8][gamma]
  //         [delta_len:8][delta][abt_len:8][abt]
  //         [ic_count:8] { [ic_len:8][ic_data] }*
  int64_t total =
      sizeof(uint64_t) +                                       // num_fields
      5 * static_cast<int64_t>(sizeof(uint64_t)) +             // 5 lengths
      alpha_buf.size() + beta_buf.size() + gamma_buf.size() +  //
      delta_buf.size() + abt_buf.size() +                      //
      sizeof(uint64_t) +                                       // ic_count
      ic_total;

  Buffer result(total);
  auto* ptr = result.data<uint8_t>();
  int64_t offset = 0;

  auto write_blob = [&](const Buffer& blob) {
    WriteU64(ptr, offset, static_cast<uint64_t>(blob.size()));
    offset += sizeof(uint64_t);
    std::memcpy(ptr + offset, blob.data(), blob.size());
    offset += blob.size();
  };

  WriteU64(ptr, offset, 5);  // number of key fields
  offset += sizeof(uint64_t);

  write_blob(alpha_buf);
  write_blob(beta_buf);
  write_blob(gamma_buf);
  write_blob(delta_buf);
  write_blob(abt_buf);

  WriteU64(ptr, offset, static_cast<uint64_t>(vk.ic.size()));
  offset += sizeof(uint64_t);
  for (const auto& ic_buf : ic_bufs) {
    write_blob(ic_buf);
  }

  return result;
}

VerificationKey DeserializeVerificationKey(
    ByteContainerView buf,
    const std::shared_ptr<crypto::PairingGroup>& pairing) {
  auto g1 = pairing->GetGroup1();
  auto g2 = pairing->GetGroup2();
  auto gt = pairing->GetGroupT();

  const auto* ptr = reinterpret_cast<const uint8_t*>(buf.data());
  int64_t offset = 0;
  int64_t buf_size = static_cast<int64_t>(buf.size());

  auto read_blob = [&](int64_t& off) -> ByteContainerView {
    YACL_ENFORCE(off + static_cast<int64_t>(sizeof(uint64_t)) <= buf_size);
    uint64_t len = ReadU64(ptr, off);
    off += sizeof(uint64_t);
    YACL_ENFORCE(off + static_cast<int64_t>(len) <= buf_size);
    ByteContainerView view(ptr + off, static_cast<size_t>(len));
    off += static_cast<int64_t>(len);
    return view;
  };

  YACL_ENFORCE(offset + static_cast<int64_t>(sizeof(uint64_t)) <= buf_size);
  uint64_t num_fields = ReadU64(ptr, offset);
  offset += sizeof(uint64_t);
  YACL_ENFORCE(num_fields == 5, "Expected 5 fields, got {}", num_fields);

  VerificationKey vk;
  vk.pairing = pairing;
  vk.alpha_g1 = g1->DeserializePoint(read_blob(offset));
  vk.beta_g2 = g2->DeserializePoint(read_blob(offset));
  vk.gamma_g2 = g2->DeserializePoint(read_blob(offset));
  vk.delta_g2 = g2->DeserializePoint(read_blob(offset));
  vk.alpha_beta_gt = gt->Deserialize(read_blob(offset));

  YACL_ENFORCE(offset + static_cast<int64_t>(sizeof(uint64_t)) <= buf_size);
  uint64_t ic_count = ReadU64(ptr, offset);
  offset += sizeof(uint64_t);
  YACL_ENFORCE(ic_count <= 10000, "Unreasonable IC count: {}", ic_count);

  vk.ic.resize(ic_count);
  for (uint64_t i = 0; i < ic_count; ++i) {
    vk.ic[i] = g1->DeserializePoint(read_blob(offset));
  }

  return vk;
}

}  // namespace yacl::engine::snark
