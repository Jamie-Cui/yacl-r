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

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <future>
#include <memory>
#include <string>
#include <thread>

#include "fmt/format.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "yacl/base/byte_container_view.h"
#include "yacl/base/exception.h"
#include "yacl/link/transport/channel.h"

namespace yacl::link::transport::test {

static std::string RandStr(size_t length) {
  auto randchar = []() -> char {
    const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);
    return charset[rand() % max_index];
  };
  std::string str(length, 0);
  std::generate_n(str.begin(), length, randchar);
  return str;
}

class AsioLinkTest : public ::testing::Test {
 protected:
  void SetUp() override {
    std::srand(std::time(nullptr));
    const size_t send_rank = 0;
    const size_t recv_rank = 1;
    auto options = AsioLink::GetDefaultOptions();

    io_ctx_sender_ = std::make_shared<asio::io_context>();
    io_ctx_receiver_ = std::make_shared<asio::io_context>();

    sender_delegate_ = std::make_shared<AsioLink>(send_rank, recv_rank, options,
                                                  *io_ctx_sender_);
    receiver_delegate_ = std::make_shared<AsioLink>(recv_rank, send_rank,
                                                    options, *io_ctx_receiver_);

    sender_ =
        std::make_shared<Channel>(sender_delegate_, false, RetryOptions());
    receiver_ =
        std::make_shared<Channel>(receiver_delegate_, false, RetryOptions());

    receiver_loop_ = std::make_unique<ReceiverLoopAsio>();
    receiver_loop_->AddListener(0, receiver_);
    receiver_host_ = receiver_loop_->Start("127.0.0.1:0");

    sender_loop_ = std::make_unique<ReceiverLoopAsio>();
    sender_loop_->AddListener(1, sender_);
    sender_host_ = sender_loop_->Start("127.0.0.1:0");

    sender_delegate_->SetPeerHost(receiver_host_);
    receiver_delegate_->SetPeerHost(sender_host_);
  }

  void TearDown() override {
    auto wait = [](std::shared_ptr<Channel>& l) {
      if (l) {
        l->WaitLinkTaskFinish();
      }
    };
    auto f_s = std::async(wait, std::ref(sender_));
    auto f_r = std::async(wait, std::ref(receiver_));
    f_s.get();
    f_r.get();
  }

  std::shared_ptr<asio::io_context> io_ctx_sender_;
  std::shared_ptr<asio::io_context> io_ctx_receiver_;
  std::shared_ptr<Channel> sender_;
  std::shared_ptr<Channel> receiver_;
  std::string receiver_host_;
  std::unique_ptr<ReceiverLoopAsio> receiver_loop_;
  std::string sender_host_;
  std::unique_ptr<ReceiverLoopAsio> sender_loop_;
  std::shared_ptr<AsioLink> sender_delegate_;
  std::shared_ptr<AsioLink> receiver_delegate_;
};

TEST_F(AsioLinkTest, Normal_Empty) {
  const std::string key = "key";
  const std::string sent;
  sender_->SendAsync(key, ByteContainerView{sent});
  auto received = receiver_->Recv(key);

  EXPECT_EQ(sent, std::string_view(received));
}

TEST_F(AsioLinkTest, Timeout) {
  receiver_->SetRecvTimeout(500U);
  const std::string key = "key";
  std::string received;
  EXPECT_THROW(receiver_->Recv(key), IoError);
}

TEST_F(AsioLinkTest, Normal_Len100) {
  const std::string key = "key";
  const std::string sent = RandStr(100U);
  sender_->SendAsync(key, ByteContainerView{sent});
  auto received = receiver_->Recv(key);

  EXPECT_EQ(sent, std::string_view(received));
}

class AsioLinkWithLimitTest
    : public AsioLinkTest,
      public ::testing::WithParamInterface<std::tuple<size_t, size_t, size_t>> {
};

TEST_P(AsioLinkWithLimitTest, SendAsync) {
  const size_t size_limit_per_call = std::get<0>(GetParam());
  const size_t size_to_send = std::get<1>(GetParam());
  const size_t parallel_size = std::get<2>(GetParam());
  sender_->SetChunkParallelSendSize(parallel_size);
  receiver_->SetChunkParallelSendSize(parallel_size);

  sender_->GetLink()->SetMaxBytesPerChunk(size_limit_per_call);

  const std::string key = "key";
  const std::string sent = RandStr(size_to_send);
  sender_->SendAsync(key, ByteContainerView{sent});
  auto received = receiver_->Recv(key);

  EXPECT_EQ(sent, std::string_view(received));
}

TEST_P(AsioLinkWithLimitTest, Send) {
  const size_t size_limit_per_call = std::get<0>(GetParam());
  const size_t size_to_send = std::get<1>(GetParam());
  const size_t parallel_size = std::get<2>(GetParam());
  sender_->SetChunkParallelSendSize(parallel_size);
  receiver_->SetChunkParallelSendSize(parallel_size);

  sender_->GetLink()->SetMaxBytesPerChunk(size_limit_per_call);

  const std::string key = "key";
  const std::string sent = RandStr(size_to_send);
  sender_->Send(key, sent);
  auto received = receiver_->Recv(key);

  EXPECT_EQ(sent, std::string_view(received));
}

INSTANTIATE_TEST_SUITE_P(
    Normal_Instances, AsioLinkWithLimitTest,
    testing::Combine(testing::Values(9, 17),
                     testing::Values(1, 2, 9, 10, 11, 20, 19, 21, 1001),
                     testing::Values(1, 8)),
    [](const testing::TestParamInfo<AsioLinkWithLimitTest::ParamType>& info) {
      std::string name =
          fmt::format("Limit_{}_Len_{}_parallel_{}", std::get<0>(info.param),
                      std::get<1>(info.param), std::get<2>(info.param));
      return name;
    });

}  // namespace yacl::link::transport::test