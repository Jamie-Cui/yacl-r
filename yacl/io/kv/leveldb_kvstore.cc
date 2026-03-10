// Copyright 2021 Ant Group Co., Ltd.
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

#include "yacl/io/kv/leveldb_kvstore.h"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <random>

#include "spdlog/spdlog.h"

#include "yacl/base/exception.h"

namespace yacl::io {

namespace {

namespace fs = std::filesystem;

std::string FindAvaliableTempFileName() {
  fs::path temp_dir = fs::current_path();  // save tmp file to current path
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, 999999);
  fs::path temp_file;
  do {
    temp_file = temp_dir / ("temp_" + std::to_string(distrib(gen)) + ".txt");
  } while (fs::exists(temp_file));

  std::ofstream ofs(temp_file);
  if (!ofs.is_open()) {
    // almost impossible
    throw std::runtime_error("Failed to create temp file");
  }
  std::remove(temp_file.c_str());
  return temp_file.filename();
}
}  // namespace

LeveldbKVStore::LeveldbKVStore(bool is_temp, const std::string &file_path)
    : is_temp_(is_temp) {
  leveldb::Options options;
  options.create_if_missing = true;

  std::string db_path = file_path;
  if (db_path.empty()) {
    db_path = FindAvaliableTempFileName();
  }

  leveldb::DB *db_ptr = nullptr;
  leveldb::Status db_status = leveldb::DB::Open(options, db_path, &db_ptr);
  YACL_ENFORCE(db_status.ok(), "leveldb open failed, msg: {}",
               db_status.ToString());
  db_.reset(db_ptr);
  path_ = db_path;
  is_open_ = true;
}

LeveldbKVStore::~LeveldbKVStore() {
  if (is_open_) {
    // this is a temporary db.
    // delete db before remove it from disk.
    if (db_) {
      db_ = nullptr;
    }
    if (is_temp_) {
      try {
        std::remove(path_.c_str());
        // butil::FilePath file_path(path_);
        // butil::DeleteFile(file_path, true);
      } catch (const std::exception &e) {
        // Nothing we can do here.
        SPDLOG_INFO("Delete tmp file:{} exception {}", path_,
                    std::string(e.what()));
      }
    }
    is_open_ = false;
  }
}

void LeveldbKVStore::Put(absl::string_view key, ByteContainerView value) {
  leveldb::Slice key_slice(key.data(), key.length());
  leveldb::Slice data_slice((const char *)value.data(), value.size());

  leveldb::Status db_status =
      db_->Put(leveldb::WriteOptions(), key_slice, data_slice);

  if (!db_status.ok()) {
    YACL_THROW("Put key:{} error, {}", key, db_status.ToString());
  }
}

bool LeveldbKVStore::Get(absl::string_view key, std::string *value) const {
  leveldb::Status db_status = db_->Get(
      leveldb::ReadOptions(),
      static_cast<std::basic_string<char, std::char_traits<char>>>(key), value);

  if (!db_status.ok()) {
    if (db_status.IsNotFound()) {
      SPDLOG_INFO("key not found");
      return false;
    }
    SPDLOG_ERROR("Get key: {}, error: {}", key, db_status.ToString());
    return false;
  }
  return true;
}

size_t LeveldbKVStore::Count() const {
  size_t count = 0;

  leveldb::Iterator *it = db_->NewIterator(leveldb::ReadOptions());
  for (it->SeekToFirst(); it->Valid(); it->Next()) {
    count++;
  }

  YACL_ENFORCE(it->status().ok());

  delete it;

  return count;
}

}  // namespace yacl::io
