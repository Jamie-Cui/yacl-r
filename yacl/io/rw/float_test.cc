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

#include "yacl/io/rw/float.h"

#include "gtest/gtest.h"

namespace yacl::io {
namespace {

TEST(FloatTest, HeaderIsSelfContainedAndParsesValues) {
  double value = 0;
  EXPECT_TRUE(FloatFromString("1.25", &value));
  EXPECT_DOUBLE_EQ(value, 1.25);

  EXPECT_FALSE(FloatFromString("not-a-number", &value));
  EXPECT_EQ(FloatToString(0.0, 6), "0");
}

}  // namespace
}  // namespace yacl::io
