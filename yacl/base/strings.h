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

#pragma once

#include <cctype>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace yacl {

// ---------------------------------------------------------------------------
// Hex encoding / decoding (replacements for absl/strings/escaping.h)
// ---------------------------------------------------------------------------

// Encode raw bytes (as a string_view) to lowercase hex string.
// Equivalent to yacl::BytesToHexString(string_view).
std::string BytesToHexString(std::string_view bytes);

// Encode a span of bytes to lowercase hex string.
std::string BytesToHexString(std::span<const uint8_t> bytes);

// Decode a hex string into binary bytes stored in *out.
// Returns true on success, false if input is not valid hex or odd length.
// Equivalent to yacl::HexStringToBytes(hex, out).
bool HexStringToBytes(std::string_view hex, std::string* out);

// ---------------------------------------------------------------------------
// String splitting / joining (replacements for absl/strings/str_split.h,
//                              absl/strings/str_join.h)
// ---------------------------------------------------------------------------

// Split sv on every occurrence of char delimiter.
// Equivalent to yacl::StrSplit(sv, delim) with char delimiter.
std::vector<std::string> StrSplit(std::string_view sv, char delim);

// Split sv on every occurrence of string delimiter sep.
std::vector<std::string> StrSplit(std::string_view sv, std::string_view sep);

// Join a vector of strings with separator sep.
// Equivalent to yacl::StrJoin(parts, sep) for string ranges.
std::string StrJoin(const std::vector<std::string>& parts,
                    std::string_view sep);

// Join a vector of string_views with separator sep.
std::string StrJoin(const std::vector<std::string_view>& parts,
                    std::string_view sep);

// Join a span of int64_t values with separator sep.
// Equivalent to yacl::StrJoin(span_of_ints, sep).
std::string StrJoin(std::span<const int64_t> parts, std::string_view sep);

// ---------------------------------------------------------------------------
// Number parsing (replacements for absl/strings/numbers.h)
// ---------------------------------------------------------------------------

// Parse a decimal integer from sv into *out.
// Returns true on success, false if sv is not a valid decimal integer.
// Equivalent to yacl::SimpleAtoi(sv, out).
bool SimpleAtoi(std::string_view sv, int64_t* out);
bool SimpleAtoi(std::string_view sv, int32_t* out);
bool SimpleAtoi(std::string_view sv, uint64_t* out);
bool SimpleAtoi(std::string_view sv, uint32_t* out);

// Parse a hex integer from sv (without 0x prefix) into *out.
// Equivalent to yacl::SimpleHexAtoi(sv, out).
bool SimpleHexAtoi(std::string_view sv, uint64_t* out);

// Parse a double from sv into *out.
// Equivalent to yacl::SimpleAtod(sv, out).
bool SimpleAtod(std::string_view sv, double* out);

// ---------------------------------------------------------------------------
// String matching (replacements for absl/strings/match.h)
// ---------------------------------------------------------------------------

// Returns true if haystack contains needle.
// Equivalent to yacl::StrContains.
inline bool StrContains(std::string_view haystack, std::string_view needle) {
  return haystack.find(needle) != std::string_view::npos;
}

// Returns true if s starts with prefix.
// Equivalent to yacl::StartsWith.
inline bool StartsWith(std::string_view s, std::string_view prefix) {
  return s.starts_with(prefix);
}

// Returns true if s ends with suffix.
// Equivalent to yacl::EndsWith.
inline bool EndsWith(std::string_view s, std::string_view suffix) {
  return s.ends_with(suffix);
}

// ---------------------------------------------------------------------------
// ASCII character utilities (replacements for absl/strings/ascii.h)
// ---------------------------------------------------------------------------

inline bool AsciiIsDigit(char c) {
  return std::isdigit(static_cast<unsigned char>(c)) != 0;
}
inline bool AsciiIsAlpha(char c) {
  return std::isalpha(static_cast<unsigned char>(c)) != 0;
}
inline bool AsciiIsWhitespace(char c) {
  return std::isspace(static_cast<unsigned char>(c)) != 0;
}
inline bool AsciiIsAlNum(char c) {
  return std::isalnum(static_cast<unsigned char>(c)) != 0;
}
inline char AsciiToLower(char c) {
  return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
}
inline char AsciiToUpper(char c) {
  return static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
}

// Convert all ASCII characters in *s to lowercase in place.
// Equivalent to yacl::AsciiStrToLower(s) (in-place overload).
void AsciiStrToLower(std::string* s);

// Return a lowercase copy of sv.
// Equivalent to yacl::AsciiStrToLower(sv) (copy overload).
std::string AsciiStrToLower(std::string_view sv);

// Convert all ASCII characters in *s to uppercase in place.
void AsciiStrToUpper(std::string* s);

// Return an uppercase copy of sv.
std::string AsciiStrToUpper(std::string_view sv);

// Strip leading and trailing ASCII whitespace from sv. Returns a view into sv.
std::string_view StripAsciiWhitespace(std::string_view sv);

}  // namespace yacl
