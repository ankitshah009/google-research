// Copyright 2020 The Google Research Authors.
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

#ifndef THIRD_PARTY_GOOGLE_RESEARCH_GOOGLE_RESEARCH_AUTOML_ZERO_UTIL_H_
#define THIRD_PARTY_GOOGLE_RESEARCH_GOOGLE_RESEARCH_AUTOML_ZERO_UTIL_H_

#include <cstdlib>

#include "definitions.h"
#include "third_party/eigen3/Eigen/Core"

namespace brain {
namespace evolution {
namespace amlz {

constexpr double kVectorEqTolerance = 0.000001;

IntegerT Pow2(const IntegerT exp);
size_t Pow2(const size_t exp);

template<FeatureIndexT F>
bool VectorEq(const Vector<F>& observed, const std::vector<double>& expected) {
  if (expected.size() != F) {
    std::cout << "Wrong size. observed size = " << F
              << ", expected size = " << expected.size() << std::endl;
    return false;
  }
  Eigen::Map<const Vector<F>> expected_eigen(expected.data());
  return (observed - expected_eigen).norm() < kVectorEqTolerance;
}

}  // namespace amlz
}  // namespace evolution
}  // namespace brain

#endif  // THIRD_PARTY_GOOGLE_RESEARCH_GOOGLE_RESEARCH_AUTOML_ZERO_UTIL_H_
