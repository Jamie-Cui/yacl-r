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

#include "yacl/base/strings.h"

#include <clocale>
#include <string>

#include "gtest/gtest.h"

namespace yacl::test {
namespace {

class ScopedLocale final {
 public:
  explicit ScopedLocale(int category) : category_(category) {
    if (const char* current = std::setlocale(category_, nullptr); current) {
      original_ = current;
    }
  }

  ~ScopedLocale() {
    if (!original_.empty()) {
      std::setlocale(category_, original_.c_str());
    }
  }

 private:
  int category_;
  std::string original_;
};

TEST(StringsTest, AsciiHelpersAreAsciiOnly) {
  EXPECT_TRUE(AsciiIsDigit('7'));
  EXPECT_FALSE(AsciiIsDigit('x'));
  EXPECT_TRUE(AsciiIsAlpha('Q'));
  EXPECT_TRUE(AsciiIsAlpha('q'));
  EXPECT_FALSE(AsciiIsAlpha('_'));
  EXPECT_TRUE(AsciiIsWhitespace('\t'));
  EXPECT_TRUE(AsciiIsWhitespace('\n'));
  EXPECT_FALSE(AsciiIsWhitespace(static_cast<char>(0xa0)));
  EXPECT_TRUE(AsciiIsAlNum('3'));
  EXPECT_TRUE(AsciiIsAlNum('R'));
  EXPECT_FALSE(AsciiIsAlNum('-'));
  EXPECT_EQ(AsciiToLower('I'), 'i');
  EXPECT_EQ(AsciiToUpper('i'), 'I');
  EXPECT_EQ(AsciiToLower(static_cast<char>(0xc4)), static_cast<char>(0xc4));
  EXPECT_EQ(AsciiToUpper(static_cast<char>(0xdf)), static_cast<char>(0xdf));

  std::string lower = "PAILLIER";
  lower.push_back(static_cast<char>(0xc4));
  lower.push_back('I');
  lower.push_back(static_cast<char>(0xdf));
  AsciiStrToLower(&lower);

  std::string expected_lower = "paillier";
  expected_lower.push_back(static_cast<char>(0xc4));
  expected_lower.push_back('i');
  expected_lower.push_back(static_cast<char>(0xdf));
  EXPECT_EQ(lower, expected_lower);

  std::string upper = "paillier";
  upper.push_back(static_cast<char>(0xe4));
  upper.push_back('i');
  upper.push_back(static_cast<char>(0xff));
  std::string expected_upper = "PAILLIER";
  expected_upper.push_back(static_cast<char>(0xe4));
  expected_upper.push_back('I');
  expected_upper.push_back(static_cast<char>(0xff));
  EXPECT_EQ(AsciiStrToUpper(upper), expected_upper);
}

TEST(StringsTest, StripAsciiWhitespaceOnlyStripsAscii) {
  std::string value;
  value.push_back('\t');
  value.push_back(static_cast<char>(0xa0));
  value.append("payload");
  value.push_back(static_cast<char>(0xa0));
  value.push_back('\n');

  std::string expected;
  expected.push_back(static_cast<char>(0xa0));
  expected.append("payload");
  expected.push_back(static_cast<char>(0xa0));

  EXPECT_EQ(StripAsciiWhitespace(value), expected);
}

TEST(StringsTest, CaseMappingIgnoresLocaleWhenTurkishLocaleIsAvailable) {
  ScopedLocale locale_guard(LC_CTYPE);

  if (std::setlocale(LC_CTYPE, "tr_TR.UTF-8") == nullptr &&
      std::setlocale(LC_CTYPE, "tr_TR.utf8") == nullptr) {
    GTEST_SKIP() << "Turkish UTF-8 locale is not available";
  }

  EXPECT_EQ(AsciiStrToLower("PAILLIER"), "paillier");
  EXPECT_EQ(AsciiStrToUpper("paillier"), "PAILLIER");
}

}  // namespace
}  // namespace yacl::test
