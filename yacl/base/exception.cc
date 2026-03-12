// Copyright 2024 Ant Group Co., Ltd.
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

#include "yacl/base/exception.h"

namespace yacl {

std::string GetStacktraceString() {
  ::yacl::stacktrace_t stacks;
  const int dep =
      ::backtrace(stacks.data(), internal::kMaxStackTraceDep);
  std::string res;
  char** symbols = ::backtrace_symbols(stacks.data(), dep);
  for (int i = 0; i < dep; ++i) {
    const char* symbol = (symbols != nullptr) ? symbols[i] : "(unknown)";
    res.append(fmt::format("#{} {}\n", i, symbol));
  }
  // NOLINTNEXTLINE(cppcoreguidelines-no-malloc)
  free(symbols);
  return res;
}

}  // namespace yacl
