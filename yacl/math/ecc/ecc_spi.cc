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

#include "yacl/math/ecc/ecc_spi.h"

#include "yacl/math/ecc/FourQlib/FourQ_group.h"
#include "yacl/math/ecc/mcl/mcl_ec_group.h"
#include "yacl/math/ecc/openssl/openssl_group.h"
#include "yacl/math/ecc/toy/common.h"
#include "yacl/utils/strings.h"

namespace yacl {

EcGroupFactory::EcGroupFactory() {
  Register("FourQlib", 1500, FourQ::IsSupported, FourQ::Create);
  Register("libmcl", 400, MclEGFactory::IsSupported, MclEGFactory::Create);
  Register("OpenSSL", 100, ossl::OpensslGroup::IsSupported,
           ossl::OpensslGroup::Create);
  Register("Toy", 10, toy::IsSupported, toy::Create);
}

EcGroupFactory &EcGroupFactory::Instance() {
  static EcGroupFactory factory;
  return factory;
}

void EcGroupFactory::Register(const std::string &lib_name, uint64_t performance,
                              const EcCheckerT &checker,
                              const EcCreatorT &creator) {
  SpiFactoryBase<EcGroup>::Register(
      lib_name, performance,
      [checker](const std::string &curve_name, const SpiArgs &) {
        CurveMeta meta;
        try {
          meta = GetCurveMetaByName(curve_name);
        } catch (const yacl::Exception &) {
          return false;
        }
        return checker(meta);
      },
      [creator](const std::string &curve_name, const SpiArgs &) {
        return creator(GetCurveMetaByName(curve_name));
      });
}

}  // namespace yacl
