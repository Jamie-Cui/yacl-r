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

#include <algorithm>
#include <charconv>
#include <cstdint>

namespace yacl {

namespace {
constexpr char kHexChars[] = "0123456789abcdef";
}  // namespace

std::string BytesToHexString(std::string_view bytes) {
  std::string result;
  result.reserve(bytes.size() * 2);
  for (unsigned char c : bytes) {
    result.push_back(kHexChars[c >> 4]);
    result.push_back(kHexChars[c & 0xf]);
  }
  return result;
}

std::string BytesToHexString(std::span<const uint8_t> bytes) {
  return BytesToHexString(std::string_view(
      reinterpret_cast<const char*>(bytes.data()), bytes.size()));
}

bool HexStringToBytes(std::string_view hex, std::string* out) {
  if (hex.size() % 2 != 0) {
    return false;
  }
  out->clear();
  out->reserve(hex.size() / 2);
  for (size_t i = 0; i < hex.size(); i += 2) {
    uint8_t byte = 0;
    auto r = std::from_chars(hex.data() + i, hex.data() + i + 2, byte, 16);
    if (r.ec != std::errc{} || r.ptr != hex.data() + i + 2) {
      return false;
    }
    out->push_back(static_cast<char>(byte));
  }
  return true;
}

std::vector<std::string> StrSplit(std::string_view sv, char delim) {
  std::vector<std::string> result;
  size_t start = 0;
  for (size_t i = 0; i <= sv.size(); ++i) {
    if (i == sv.size() || sv[i] == delim) {
      result.emplace_back(sv.substr(start, i - start));
      start = i + 1;
    }
  }
  return result;
}

std::vector<std::string> StrSplit(std::string_view sv, std::string_view sep) {
  std::vector<std::string> result;
  if (sep.empty()) {
    result.emplace_back(sv);
    return result;
  }
  size_t start = 0;
  size_t pos;
  while ((pos = sv.find(sep, start)) != std::string_view::npos) {
    result.emplace_back(sv.substr(start, pos - start));
    start = pos + sep.size();
  }
  result.emplace_back(sv.substr(start));
  return result;
}

std::string StrJoin(const std::vector<std::string>& parts,
                    std::string_view sep) {
  std::string result;
  for (size_t i = 0; i < parts.size(); ++i) {
    if (i > 0) {
      result.append(sep);
    }
    result.append(parts[i]);
  }
  return result;
}

std::string StrJoin(const std::vector<std::string_view>& parts,
                    std::string_view sep) {
  std::string result;
  for (size_t i = 0; i < parts.size(); ++i) {
    if (i > 0) {
      result.append(sep);
    }
    result.append(parts[i]);
  }
  return result;
}

std::string StrJoin(std::span<const int64_t> parts, std::string_view sep) {
  std::string result;
  for (size_t i = 0; i < parts.size(); ++i) {
    if (i > 0) {
      result.append(sep);
    }
    result.append(std::to_string(parts[i]));
  }
  return result;
}

bool SimpleAtoi(std::string_view sv, int64_t* out) {
  auto r = std::from_chars(sv.data(), sv.data() + sv.size(), *out);
  return r.ec == std::errc{} && r.ptr == sv.data() + sv.size();
}

bool SimpleAtoi(std::string_view sv, int32_t* out) {
  auto r = std::from_chars(sv.data(), sv.data() + sv.size(), *out);
  return r.ec == std::errc{} && r.ptr == sv.data() + sv.size();
}

bool SimpleAtoi(std::string_view sv, uint64_t* out) {
  auto r = std::from_chars(sv.data(), sv.data() + sv.size(), *out);
  return r.ec == std::errc{} && r.ptr == sv.data() + sv.size();
}

bool SimpleAtoi(std::string_view sv, uint32_t* out) {
  auto r = std::from_chars(sv.data(), sv.data() + sv.size(), *out);
  return r.ec == std::errc{} && r.ptr == sv.data() + sv.size();
}

bool SimpleHexAtoi(std::string_view sv, uint64_t* out) {
  auto r = std::from_chars(sv.data(), sv.data() + sv.size(), *out, 16);
  return r.ec == std::errc{} && r.ptr == sv.data() + sv.size();
}

void AsciiStrToLower(std::string* s) {
  std::transform(s->begin(), s->end(), s->begin(),
                 [](unsigned char c) { return std::tolower(c); });
}

std::string AsciiStrToLower(std::string_view sv) {
  std::string result(sv);
  AsciiStrToLower(&result);
  return result;
}

void AsciiStrToUpper(std::string* s) {
  std::transform(s->begin(), s->end(), s->begin(),
                 [](unsigned char c) { return std::toupper(c); });
}

std::string AsciiStrToUpper(std::string_view sv) {
  std::string result(sv);
  AsciiStrToUpper(&result);
  return result;
}

std::string_view StripAsciiWhitespace(std::string_view sv) {
  size_t start = 0;
  while (start < sv.size() &&
         std::isspace(static_cast<unsigned char>(sv[start]))) {
    ++start;
  }
  size_t end = sv.size();
  while (end > start &&
         std::isspace(static_cast<unsigned char>(sv[end - 1]))) {
    --end;
  }
  return sv.substr(start, end - start);
}

bool SimpleAtod(std::string_view sv, double* out) {
  auto r = std::from_chars(sv.data(), sv.data() + sv.size(), *out);
  return r.ec == std::errc{} && r.ptr == sv.data() + sv.size();
}

}  // namespace yacl
