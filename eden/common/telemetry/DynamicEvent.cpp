/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "eden/common/telemetry/DynamicEvent.h"

#include <folly/Conv.h>
#include <folly/Unicode.h>
#include <folly/logging/xlog.h>
#include "eden/common/utils/Throw.h"

namespace {
void validateUtf8(folly::StringPiece sp) {
  auto* p = reinterpret_cast<const unsigned char*>(sp.begin());
  auto* const end = reinterpret_cast<const unsigned char*>(sp.end());
  while (p < end) {
    (void)folly::utf8ToCodePoint(p, end, false);
  }
}
} // namespace

namespace facebook::eden {

void DynamicEvent::addInt(std::string name, int64_t value) {
  auto [iter, inserted] = ints_.emplace(std::move(name), value);
  if (!inserted) {
    throw_<std::logic_error>(
        "Attempted to insert duplicate int: ", iter->first);
  }
}

void DynamicEvent::addString(std::string name, std::string value) {
  validateUtf8(value);
  auto [iter, inserted] = strings_.emplace(std::move(name), std::move(value));
  if (!inserted) {
    throw_<std::logic_error>(
        "Attempted to insert duplicate string: ", iter->first);
  }
}

void DynamicEvent::addDouble(std::string name, double value) {
  XCHECK(std::isfinite(value))
      << "Attempted to insert double-precision value that cannot be represented in JSON: "
      << name;
  auto [iter, inserted] = doubles_.emplace(std::move(name), value);
  if (!inserted) {
    throw_<std::logic_error>(
        "Attempted to insert duplicate double: ", iter->first);
  }
}

} // namespace facebook::eden
