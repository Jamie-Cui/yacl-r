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

#include "yacl/link/transport/asio_link.h"

#include <cstring>
#include <memory>
#include <utility>

#include "spdlog/spdlog.h"

#include "yacl/base/exception.h"

namespace yacl::link::transport {

namespace {

constexpr uint8_t kTransMono = 0;
constexpr uint8_t kTransChunked = 1;
constexpr uint32_t kErrorCodeOk = 0;
constexpr uint32_t kErrorCodeInvalid = 1;

std::pair<std::string, uint16_t> ParseHostPort(const std::string& addr) {
  auto pos = addr.find_last_of(':');
  if (pos == std::string::npos) {
    YACL_THROW_IO_ERROR("Invalid address format: {}", addr);
  }
  std::string host = addr.substr(0, pos);
  uint16_t port = static_cast<uint16_t>(std::stoi(addr.substr(pos + 1)));
  if (host.front() == '[' && host.back() == ']') {
    host = host.substr(1, host.size() - 2);
  }
  return {host, port};
}

std::vector<uint8_t> SerializePushMessage(const PushMessage& msg) {
  msgpack::sbuffer sbuf;
  msgpack::pack(sbuf, msg);
  std::vector<uint8_t> result(sbuf.size());
  std::memcpy(result.data(), sbuf.data(), sbuf.size());
  return result;
}

PushMessage DeserializePushMessage(const uint8_t* data, size_t len) {
  msgpack::unpacked result;
  msgpack::unpack(result, reinterpret_cast<const char*>(data), len);
  return result.get().as<PushMessage>();
}

std::vector<uint8_t> SerializePushAckMessage(const PushAckMessage& msg) {
  msgpack::sbuffer sbuf;
  msgpack::pack(sbuf, msg);
  std::vector<uint8_t> result(sbuf.size());
  std::memcpy(result.data(), sbuf.data(), sbuf.size());
  return result;
}

PushAckMessage DeserializePushAckMessage(const uint8_t* data, size_t len) {
  msgpack::unpacked result;
  msgpack::unpack(result, reinterpret_cast<const char*>(data), len);
  return result.get().as<PushAckMessage>();
}

}  // namespace

std::array<uint8_t, MessageHeader::kHeaderLen> MessageHeader::Serialize()
    const {
  std::array<uint8_t, kHeaderLen> buf{};
  buf[0] = static_cast<uint8_t>((magic >> 24) & 0xFF);
  buf[1] = static_cast<uint8_t>((magic >> 16) & 0xFF);
  buf[2] = static_cast<uint8_t>((magic >> 8) & 0xFF);
  buf[3] = static_cast<uint8_t>(magic & 0xFF);
  buf[4] = static_cast<uint8_t>((body_len >> 24) & 0xFF);
  buf[5] = static_cast<uint8_t>((body_len >> 16) & 0xFF);
  buf[6] = static_cast<uint8_t>((body_len >> 8) & 0xFF);
  buf[7] = static_cast<uint8_t>(body_len & 0xFF);
  buf[8] = msg_type;
  return buf;
}

MessageHeader MessageHeader::Parse(const uint8_t* data) {
  MessageHeader header;
  header.magic = (static_cast<uint32_t>(data[0]) << 24) |
                 (static_cast<uint32_t>(data[1]) << 16) |
                 (static_cast<uint32_t>(data[2]) << 8) |
                 static_cast<uint32_t>(data[3]);
  header.body_len = (static_cast<uint32_t>(data[4]) << 24) |
                    (static_cast<uint32_t>(data[5]) << 16) |
                    (static_cast<uint32_t>(data[6]) << 8) |
                    static_cast<uint32_t>(data[7]);
  header.msg_type = data[8];
  return header;
}

class AsioRequest : public Request {
 public:
  MessageHeader header;
  std::vector<uint8_t> body;
};

ReceiverLoopAsio::~ReceiverLoopAsio() { Stop(); }

void ReceiverLoopAsio::Stop() {
  if (!running_.exchange(false)) {
    return;
  }
  asio::post(io_ctx_, []() {});
  if (acceptor_) {
    asio::error_code ec;
    acceptor_->close(ec);
  }
  if (io_thread_.joinable()) {
    io_thread_.join();
  }
}

std::string ReceiverLoopAsio::Start(const std::string& host,
                                    const SSLOptions* ssl_opts) {
  if (running_.exchange(true)) {
    YACL_THROW_LOGIC_ERROR("asio server is already running");
  }

  auto [ip, port] = ParseHostPort(host);

  if (ssl_opts != nullptr) {
    ssl_ctx_ =
        std::make_unique<asio::ssl::context>(asio::ssl::context::tls_server);
    ssl_ctx_->use_certificate_chain_file(ssl_opts->cert.certificate_path);
    ssl_ctx_->use_private_key_file(ssl_opts->cert.private_key_path,
                                   asio::ssl::context::pem);
    ssl_ctx_->load_verify_file(ssl_opts->verify.ca_file_path);
    ssl_ctx_->set_verify_depth(ssl_opts->verify.verify_depth);
  }

  asio::ip::tcp::endpoint endpoint(asio::ip::make_address(ip), port);
  acceptor_ = std::make_unique<asio::ip::tcp::acceptor>(io_ctx_, endpoint);

  io_thread_ = std::thread([this]() {
    asio::io_context::work work(io_ctx_);
    io_ctx_.run();
  });

  asio::post(io_ctx_, [this]() { DoAccept(); });

  return fmt::format("{}:{}", acceptor_->local_endpoint().address().to_string(),
                     acceptor_->local_endpoint().port());
}

void ReceiverLoopAsio::DoAccept() {
  if (!running_) {
    return;
  }

  auto socket = std::make_shared<asio::ip::tcp::socket>(io_ctx_);
  acceptor_->async_accept(*socket, [this, socket](asio::error_code ec) {
    if (ec) {
      if (running_) {
        SPDLOG_WARN("Accept error: {}", ec.message());
        asio::post(io_ctx_, [this]() { DoAccept(); });
      }
      return;
    }
    {
      std::lock_guard<std::mutex> lock(sessions_mutex_);
      active_sessions_.push_back(socket);
    }
    HandleSession(socket);
    DoAccept();
  });
}

void ReceiverLoopAsio::HandleSession(
    std::shared_ptr<asio::ip::tcp::socket> socket) {
  auto read_header = [this, socket]() {
    auto header_buf =
        std::make_shared<std::array<uint8_t, MessageHeader::kHeaderLen>>();
    asio::async_read(
        *socket, asio::buffer(*header_buf),
        [this, socket, header_buf](asio::error_code ec, size_t) {
          if (ec) {
            SPDLOG_DEBUG("Read header error: {}", ec.message());
            return;
          }

          auto header = MessageHeader::Parse(header_buf->data());
          if (header.magic != 0x5941434C) {
            SPDLOG_WARN("Invalid magic number");
            return;
          }

          auto body_buf =
              std::make_shared<std::vector<uint8_t>>(header.body_len);
          asio::async_read(
              *socket, asio::buffer(*body_buf),
              [this, socket, header, body_buf](asio::error_code ec, size_t) {
                if (ec) {
                  SPDLOG_DEBUG("Read body error: {}", ec.message());
                  return;
                }

                if (header.msg_type ==
                    static_cast<uint8_t>(MessageType::kPush)) {
                  auto push_msg = DeserializePushMessage(body_buf->data(),
                                                         body_buf->size());
                  auto iter = listeners_.find(push_msg.sender_rank);
                  if (iter != listeners_.end()) {
                    auto request = std::make_unique<AsioRequest>();
                    request->header = header;
                    request->body = *body_buf;

                    PushAckMessage ack_msg;
                    try {
                      iter->second->OnRequest(*request, nullptr);
                      ack_msg.error_code = kErrorCodeOk;
                    } catch (const std::exception& e) {
                      ack_msg.error_code = kErrorCodeInvalid;
                      ack_msg.error_msg = e.what();
                    }

                    auto ack_body = SerializePushAckMessage(ack_msg);
                    MessageHeader ack_header;
                    ack_header.magic = 0x5941434C;
                    ack_header.body_len =
                        static_cast<uint32_t>(ack_body.size());
                    ack_header.msg_type =
                        static_cast<uint8_t>(MessageType::kPushAck);

                    auto ack_header_buf = ack_header.Serialize();
                    std::vector<asio::const_buffer> buffers;
                    buffers.push_back(asio::buffer(ack_header_buf));
                    buffers.push_back(asio::buffer(ack_body));

                    asio::async_write(*socket, buffers,
                                      [socket](asio::error_code ec, size_t) {
                                        if (ec) {
                                          SPDLOG_DEBUG("Write ack error: {}",
                                                       ec.message());
                                        }
                                      });
                  }
                }
              });
        });
  };

  read_header();
}

AsioLink::AsioLink(size_t self_rank, size_t peer_rank, Options options,
                   asio::io_context& io_ctx)
    : TransportLink(self_rank, peer_rank),
      options_(std::move(options)),
      io_ctx_(io_ctx) {}

AsioLink::~AsioLink() {
  std::lock_guard<std::mutex> lock(socket_mutex_);
  if (ssl_socket_) {
    asio::error_code ec;
    ssl_socket_->shutdown(ec);
  }
  if (socket_) {
    asio::error_code ec;
    socket_->close(ec);
  }
}

void AsioLink::SetPeerHost(const std::string& peer_host,
                           const SSLOptions* ssl_opts) {
  peer_host_ = peer_host;
  if (ssl_opts != nullptr) {
    ssl_ctx_ =
        std::make_unique<asio::ssl::context>(asio::ssl::context::tls_client);
    ssl_ctx_->load_verify_file(ssl_opts->verify.ca_file_path);
    ssl_ctx_->set_verify_depth(ssl_opts->verify.verify_depth);
    ssl_ctx_->use_certificate_chain_file(ssl_opts->cert.certificate_path);
    ssl_ctx_->use_private_key_file(ssl_opts->cert.private_key_path,
                                   asio::ssl::context::pem);
  }
}

void AsioLink::Connect() const {
  if (connected_.load()) {
    return;
  }

  bool expected = false;
  if (!connecting_.compare_exchange_strong(expected, true)) {
    std::unique_lock<std::mutex> lock(socket_mutex_);
    connect_cv_.wait(
        lock, [this]() { return connected_.load() || !connecting_.load(); });
    return;
  }

  auto [ip, port] = ParseHostPort(peer_host_);

  asio::ip::tcp::resolver resolver(io_ctx_);
  auto endpoints = resolver.resolve(ip, std::to_string(port));

  std::lock_guard<std::mutex> lock(socket_mutex_);

  if (ssl_ctx_) {
    ssl_socket_ = std::make_shared<asio::ssl::stream<asio::ip::tcp::socket>>(
        io_ctx_, *ssl_ctx_);
    asio::connect(ssl_socket_->lowest_layer(), endpoints);
    ssl_socket_->handshake(asio::ssl::stream_base::client);
  } else {
    socket_ = std::make_shared<asio::ip::tcp::socket>(io_ctx_);
    asio::connect(*socket_, endpoints);
  }

  connected_.store(true);
  connecting_.store(false);
  connect_cv_.notify_all();
}

void AsioLink::DoSend(const MessageHeader& header,
                      const std::vector<uint8_t>& body) const {
  Connect();

  auto header_buf = header.Serialize();
  std::vector<asio::const_buffer> buffers;
  buffers.push_back(asio::buffer(header_buf));
  buffers.push_back(asio::buffer(body));

  std::lock_guard<std::mutex> lock(socket_mutex_);
  if (ssl_socket_) {
    asio::write(*ssl_socket_, buffers);
  } else {
    asio::write(*socket_, buffers);
  }
}

std::unique_ptr<TransportLink::Request> AsioLink::PackMonoRequest(
    const std::string& key, ByteContainerView value) const {
  PushMessage msg;
  msg.sender_rank = static_cast<uint32_t>(self_rank_);
  msg.key = key;
  msg.value.assign(value.data(), value.data() + value.size());
  msg.trans_type = kTransMono;

  auto request = std::make_unique<AsioRequest>();
  request->body = SerializePushMessage(msg);
  request->header.magic = 0x5941434C;
  request->header.body_len = static_cast<uint32_t>(request->body.size());
  request->header.msg_type = static_cast<uint8_t>(MessageType::kPush);

  return request;
}

std::unique_ptr<TransportLink::Request> AsioLink::PackChunkedRequest(
    const std::string& key, ByteContainerView value, size_t offset,
    size_t total_length) const {
  PushMessage msg;
  msg.sender_rank = static_cast<uint32_t>(self_rank_);
  msg.key = key;
  msg.value.assign(value.data(), value.data() + value.size());
  msg.trans_type = kTransChunked;
  msg.chunk_offset = static_cast<uint64_t>(offset);
  msg.message_length = static_cast<uint64_t>(total_length);

  auto request = std::make_unique<AsioRequest>();
  request->body = SerializePushMessage(msg);
  request->header.magic = 0x5941434C;
  request->header.body_len = static_cast<uint32_t>(request->body.size());
  request->header.msg_type = static_cast<uint8_t>(MessageType::kPush);

  return request;
}

void AsioLink::UnpackMonoRequest(const Request& request, std::string* key,
                                 ByteContainerView* value) const {
  auto& req = static_cast<const AsioRequest&>(request);
  auto msg = DeserializePushMessage(req.body.data(), req.body.size());
  *key = msg.key;
  *value = ByteContainerView(msg.value.data(), msg.value.size());
}

void AsioLink::UnpackChunckRequest(const Request& request, std::string* key,
                                   ByteContainerView* value, size_t* offset,
                                   size_t* total_length) const {
  auto& req = static_cast<const AsioRequest&>(request);
  auto msg = DeserializePushMessage(req.body.data(), req.body.size());
  *key = msg.key;
  *value = ByteContainerView(msg.value.data(), msg.value.size());
  *offset = static_cast<size_t>(msg.chunk_offset);
  *total_length = static_cast<size_t>(msg.message_length);
}

void AsioLink::FillResponseOk(const Request& /*request*/,
                              Response* /*response*/) const {}

void AsioLink::FillResponseError(const Request& /*request*/,
                                 Response* /*response*/) const {}

bool AsioLink::IsChunkedRequest(const Request& request) const {
  auto& req = static_cast<const AsioRequest&>(request);
  auto msg = DeserializePushMessage(req.body.data(), req.body.size());
  return msg.trans_type == kTransChunked;
}

bool AsioLink::IsMonoRequest(const Request& request) const {
  auto& req = static_cast<const AsioRequest&>(request);
  auto msg = DeserializePushMessage(req.body.data(), req.body.size());
  return msg.trans_type == kTransMono;
}

void AsioLink::SendRequest(const Request& request,
                           uint32_t /*timeout_override_ms*/) const {
  auto& req = static_cast<const AsioRequest&>(request);
  DoSend(req.header, req.body);

  MessageHeader resp_header;
  std::array<uint8_t, MessageHeader::kHeaderLen> resp_header_buf;
  std::vector<uint8_t> resp_body;

  {
    std::lock_guard<std::mutex> lock(socket_mutex_);
    if (ssl_socket_) {
      asio::read(*ssl_socket_, asio::buffer(resp_header_buf));
      resp_header = MessageHeader::Parse(resp_header_buf.data());
      resp_body.resize(resp_header.body_len);
      asio::read(*ssl_socket_, asio::buffer(resp_body));
    } else {
      asio::read(*socket_, asio::buffer(resp_header_buf));
      resp_header = MessageHeader::Parse(resp_header_buf.data());
      resp_body.resize(resp_header.body_len);
      asio::read(*socket_, asio::buffer(resp_body));
    }
  }

  auto ack = DeserializePushAckMessage(resp_body.data(), resp_body.size());
  if (ack.error_code != kErrorCodeOk) {
    YACL_THROW("send failed, peer error: {}", ack.error_msg);
  }
}

}  // namespace yacl::link::transport