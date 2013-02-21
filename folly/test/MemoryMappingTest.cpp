/*
 * Copyright 2013 Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include "folly/MemoryMapping.h"

namespace folly {

TEST(MemoryMapping, Basic) {
  File f = File::temporary();
  {
    WritableMemoryMapping m(f.fd(), 0, sizeof(double));
    double volatile* d = m.asWritableRange<double>().data();
    *d = 37 * M_PI;
  }
  {
    MemoryMapping m(f.fd(), 0, 3);
    EXPECT_EQ(0, m.asRange<int>().size()); //not big enough
  }
  {
    MemoryMapping m(f.fd(), 0, sizeof(double));
    const double volatile* d = m.asRange<double>().data();
    EXPECT_EQ(*d, 37 * M_PI);
  }
}

TEST(MemoryMapping, DoublyMapped) {
  File f = File::temporary();
  // two mappings of the same memory, different addresses.
  WritableMemoryMapping mw(f.fd(), 0, sizeof(double));
  MemoryMapping mr(f.fd(), 0, sizeof(double));

  double volatile* dw = mw.asWritableRange<double>().data();
  const double volatile* dr = mr.asRange<double>().data();

  // Show that it's truly the same value, even though the pointers differ
  EXPECT_NE(dw, dr);
  *dw = 42 * M_PI;
  EXPECT_EQ(*dr, 42 * M_PI);
  *dw = 43 * M_PI;
  EXPECT_EQ(*dr, 43 * M_PI);
}

} // namespace folly