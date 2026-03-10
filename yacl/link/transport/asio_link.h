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
#include <atomic>
#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

#include "asio.hpp"
#include "asio/ssl.hpp"
#include "msgpack.hpp"

#include "yacl/base/buffer.h"
#include "yacl/base/byte_container_view.h"
#include "yacl/base/exception.h"
#include "yacl/link/ssl_options.h"
#include "yacl/link/transport/channel.h"

namespace yacl::link::transport {

struct MessageHeader {
  uint32_t magic{0x5941434C};
  uint32_t body_len{0};
  uint8_t msg_type{0};

  static constexpr size_t kHeaderLen = 9;
  std::array<uint8_t, kHeaderLen> Serialize() const;
  static MessageHeader Parse(const uint8_t* data);
};

enum class MessageType : uint8_t {
  kPush = 1,
  kPushAck = 2,
};

struct PushMessage {
  uint32_t sender_rank{0};
  std::string key;
  std::vector<uint8_t> value;
  uint8_t trans_type{0};
  uint64_t chunk_offset{0};
  uint64_t message_length{0};
  MSGPACK_DEFINE(sender_rank, key, value, trans_type, chunk_offset,
                 message_length)
};

struct PushAckMessage {
  uint32_t error_code{0};
  std::string error_msg;
  MSGPACK_DEFINE(error_code, error_msg)
};

class AsioLink;

class ReceiverLoopAsio final : public IReceiverLoop {
 public:
  ~ReceiverLoopAsio() override;
  void Stop() override;

  std::string Start(const std::string& host,
                    const SSLOptions* ssl_opts = nullptr);

 private:
  void DoAccept();
  void HandleSession(std::shared_ptr<asio::ip::tcp::socket> socket);

  asio::io_context io_ctx_;
  std::unique_ptr<asio::ip::tcp::acceptor> acceptor_;
  std::unique_ptr<asio::ssl::context> ssl_ctx_;
  std::thread io_thread_;
  std::atomic<bool> running_{false};
  std::mutex sessions_mutex_;
  std::vector<std::shared_ptr<void>> active_sessions_;
};

class AsioLink final : public TransportLink {
 public:
  struct Options {
    uint32_t timeout_ms = 10 * 1000;
    uint32_t max_payload_bytes = 512 * 1024;
  };

  static Options GetDefaultOptions() { return Options{}; }

  AsioLink(size_t self_rank, size_t peer_rank, Options options,
           asio::io_context& io_ctx);
  ~AsioLink() override;

  void SetMaxBytesPerChunk(size_t bytes) override {
    options_.max_payload_bytes = bytes;
  }
  size_t GetMaxBytesPerChunk() const override {
    return options_.max_payload_bytes;
  }

  std::unique_ptr<Request> PackMonoRequest(
      const std::string& key, ByteContainerView value) const override;
  std::unique_ptr<Request> PackChunkedRequest(
      const std::string& key, ByteContainerView value, size_t offset,
      size_t total_length) const override;
  void UnpackMonoRequest(const Request& request, std::string* key,
                         ByteContainerView* value) const override;
  void UnpackChunckRequest(const Request& request, std::string* key,
                           ByteContainerView* value, size_t* offset,
                           size_t* total_length) const override;
  void FillResponseOk(const Request& request,
                      Response* response) const override;
  void FillResponseError(const Request& request,
                         Response* response) const override;
  bool IsChunkedRequest(const Request& request) const override;
  bool IsMonoRequest(const Request& request) const override;
  void SendRequest(const Request& request,
                   uint32_t timeout_override_ms) const override;

  void SetPeerHost(const std::string& peer_host,
                   const SSLOptions* ssl_opts = nullptr);

 private:
  void Connect() const;
  void DoSend(const MessageHeader& header,
              const std::vector<uint8_t>& body) const;

  Options options_;
  asio::io_context& io_ctx_;
  std::string peer_host_;
  mutable std::shared_ptr<asio::ip::tcp::socket> socket_;
  mutable std::shared_ptr<asio::ssl::stream<asio::ip::tcp::socket>> ssl_socket_;
  std::unique_ptr<asio::ssl::context> ssl_ctx_;
  mutable std::mutex socket_mutex_;
  mutable std::condition_variable connect_cv_;
  mutable std::atomic<bool> connected_{false};
  mutable std::atomic<bool> connecting_{false};
};

}  // namespace yacl::link::transport