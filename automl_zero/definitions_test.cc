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

#include "definitions.h"

#include <type_traits>
#include <unordered_set>
#include <vector>

#include "testing/base/public/benchmark.h"
#include "testing/base/public/gmock.h"
#include "gtest/gtest.h"

namespace brain {
namespace evolution {
namespace amlz {

using ::std::unordered_set;  // NOLINT
using ::std::vector;  // NOLINT
using ::testing::ElementsAre;

TEST(PositiveOrDieTest, WorksCorrectlyForIntegerT) {
  constexpr IntegerT one = 1;
  constexpr IntegerT ten = 10;
  constexpr IntegerT zero = 0;
  EXPECT_EQ(PositiveOrDie(one), one);
  EXPECT_EQ(PositiveOrDie(ten), ten);
  EXPECT_DEATH({PositiveOrDie(zero);}, "Found non-positive.");
  EXPECT_DEATH({PositiveOrDie(-one);}, "Found non-positive.");
  EXPECT_DEATH({PositiveOrDie(-ten);}, "Found non-positive.");
}

TEST(PositiveOrDieTest, WorksCorrectlyForDouble) {
  constexpr double one_point_two = 1.2;
  constexpr double ten_point_three = 10.3;
  constexpr double zero = 0.0;
  EXPECT_FLOAT_EQ(PositiveOrDie(one_point_two), one_point_two);
  EXPECT_FLOAT_EQ(PositiveOrDie(ten_point_three), ten_point_three);
  EXPECT_DEATH({PositiveOrDie(zero);}, "Found non-positive.");
  EXPECT_DEATH({PositiveOrDie(-one_point_two);}, "Found non-positive.");
  EXPECT_DEATH({PositiveOrDie(-ten_point_three);}, "Found non-positive.");
}

TEST(NotNullOrDieTest, WorksCorrectly) {
  IntegerT value = 0;
  IntegerT* notnull_ptr = &value;
  IntegerT* null_ptr = nullptr;
  EXPECT_EQ(NotNullOrDie(notnull_ptr), notnull_ptr);
  EXPECT_DEATH({NotNullOrDie(null_ptr);}, "Found null.");
}

TEST(NonEmptyOrDieTest, WorksCorrectly_MutableCase) {
  vector<IntegerT> empty;
  vector<IntegerT> non_empty = {0, 1, 2};
  EXPECT_THAT(NonEmptyOrDie(non_empty), ElementsAre(0, 1, 2));
  EXPECT_DEATH({NonEmptyOrDie(empty);}, "Found empty.");
}

TEST(NonEmptyOrDieTest, WorksCorrectly_ConstCase) {
  const vector<IntegerT> empty;
  const vector<IntegerT> non_empty = {0, 1, 2};
  EXPECT_THAT(NonEmptyOrDie(non_empty), ElementsAre(0, 1, 2));
  EXPECT_DEATH({NonEmptyOrDie(empty);}, "Found empty.");
}

TEST(NonEmptyOrDieTest, WorksCorrectly_PointerCase) {
  vector<IntegerT> empty;
  vector<IntegerT> non_empty = {0, 1, 2};
  EXPECT_EQ(NonEmptyOrDie(&non_empty), &non_empty);
  EXPECT_DEATH({NonEmptyOrDie(&empty);}, "Found empty.");
}

TEST(SizeLessThanOrDieTest, WorksCorrectly_MutableCase) {
  vector<IntegerT> small = {0, 1};
  vector<IntegerT> large = {0, 1, 2, 3, 4};
  EXPECT_THAT(SizeLessThanOrDie(small, 3), ElementsAre(0, 1));
  EXPECT_DEATH({SizeLessThanOrDie(large, 3);}, "Too large.");
}

TEST(SizeLessThanOrDieTest, WorksCorrectly_ConstCase) {
  vector<IntegerT> small = {0, 1};
  vector<IntegerT> large = {0, 1, 2, 3, 4};
  EXPECT_THAT(SizeLessThanOrDie(small, 3), ElementsAre(0, 1));
  EXPECT_DEATH({SizeLessThanOrDie(large, 3);}, "Too large.");
}

TEST(SizeLessThanOrDieTest, WorksCorrectly_PointerCase) {
  vector<IntegerT> small = {0, 1};
  vector<IntegerT> large = {0, 1, 2, 3, 4};
  EXPECT_EQ(SizeLessThanOrDie(&small, 3), &small);
  EXPECT_DEATH({SizeLessThanOrDie(&large, 3);}, "Too large.");
}

TEST(CustomHashMixTest, DoesNotGenerateShortCycles) {
  const IntegerT num_iters = 100;
  const RandomSeedT seed = 20;
  RandomSeedT current = seed;
  unordered_set<RandomSeedT> values;
  for (IntegerT iters = 0; iters < num_iters; ++iters) {
    current = CustomHashMix(current, seed);
    values.insert(current);
  }
  EXPECT_EQ(values.size(), num_iters);
}

TEST(SafeCast, ConvertsFromSignedToUnsignedInteger) {
  const IntegerT src = 42;
  const size_t dest = SafeCast<IntegerT, size_t>(src);
  constexpr size_t expected = 42;
  EXPECT_EQ(dest, expected);
}

TEST(SafeCast, DiesFromNegativeSignedToUnsignedInteger) {
  const IntegerT src = -10;
  EXPECT_DEATH(
      (SafeCast<IntegerT, size_t>(src)),
      "Check failed: util_intops::SafeAdd");
}

TEST(SafeCast, DiesFromSignedToUnsignedIntegerWithOverflow) {
  const size_t src = std::numeric_limits<size_t>::max();
  EXPECT_DEATH(
      (SafeCast<size_t, IntegerT>(src)),
      "Check failed: util_intops::SafeAdd");
}

TEST(SafeCast, ConvertsFromHighToLowPrecisionInteger) {
  const int64_t src = 42;
  const int8_t dest = SafeCast<int64_t, int8_t>(src);
  constexpr int8_t expected = 42;
  EXPECT_EQ(dest, expected);
}

TEST(SafeCast, DiesFromHighToLowPrecisionIntegerWithOverflow) {
  const int64_t src = std::numeric_limits<int64_t>::max();
  EXPECT_DEATH(
      (SafeCast<int64_t, int8_t>(src)),
      "Check failed: util_intops::SafeAdd");
}

TEST(SafeCast, ConvertsFromHighToLowPrecisionInteger_Negative) {
  const int64_t src = -42;
  const int8_t dest = SafeCast<int64_t, int8_t>(src);
  constexpr int8_t expected = -42;
  EXPECT_EQ(dest, expected);
}

TEST(SafeCast, DiesFromHighToLowPrecisionIntegerWithOverflow_Negative) {
  const int64_t src = std::numeric_limits<int64_t>::min();
  EXPECT_DEATH(
      (SafeCast<int64_t, int8_t>(src)),
      "Check failed: util_intops::SafeAdd");
}

}  // namespace amlz
}  // namespace evolution
}  // namespace brain
