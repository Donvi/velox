/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <random>
#include "velox/functions/Macros.h"

namespace facebook::velox::functions::sparksql {

template <typename T>
struct RandFunction {
  static constexpr bool is_deterministic = false;

  template <typename TInput>
  FOLLY_ALWAYS_INLINE void initialize(
      const std::vector<TypePtr>& /*inputTypes*/,
      const core::QueryConfig& config,
      const TInput* seedInput) {
    const auto partitionId = config.sparkPartitionId();
    int64_t seed = seedInput ? (int64_t)*seedInput : 0;
    generator_.seed(seed + partitionId);
  }

  FOLLY_ALWAYS_INLINE void call(double& result) {
    result = folly::Random::randDouble01();
  }

  template <typename TInput>
  FOLLY_ALWAYS_INLINE void callNullable(double& result, TInput /*seedInput*/) {
    result = folly::Random::randDouble01(generator_);
  }

 protected:
  std::mt19937 generator_;
};

/*
 * This is inheritaed from the rand as we should align them with the same random generator
 * to save maintain effort.
 */
template <typename T>
struct RandnFunction: RandFunction<T> {

  FOLLY_ALWAYS_INLINE void call(double& result) {
    result = standard_nd(folly::ThreadLocalPRNG());
  };

  template <typename TInput>
  FOLLY_ALWAYS_INLINE void callNullable(double& result, TInput /*seedInput*/) {
    result = standard_nd(generator_);
  };

 private:
  std::normal_distribution<double> standard_nd{0.0, 1.0};
};

} // namespace facebook::velox::functions::sparksql
