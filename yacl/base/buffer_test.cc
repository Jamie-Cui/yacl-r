// Copyright 2023 Ant Group Co., Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "yacl/base/buffer.h"

#include <cstdint>
#include <vector>

#include "gtest/gtest.h"

namespace yacl::test {


TEST(BufferTest, ReserveWorks) {
  Buffer buf("abc\0", 4);
  ASSERT_EQ(buf.size(), 4);

  buf.reserve(10);
  EXPECT_EQ(buf.size(), 4);
  EXPECT_EQ(buf.capacity(), 10);
  EXPECT_STREQ(buf.data<char>(), "abc");

  EXPECT_ANY_THROW(buf.reserve(1));
  EXPECT_NO_THROW(buf.resize(1));
  EXPECT_EQ(buf.size(), 1);
  EXPECT_EQ(buf.capacity(), 10);
}

}  // namespace yacl::test
