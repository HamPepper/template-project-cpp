#include <cassert>

#include "Object.hpp"

namespace tpcpp {

void testToBoolFromDouble() {
  Object o = 3.14;
  assert(toBool(o) == true);
}

} // namespace tpcpp

int main() { tpcpp::testToBoolFromDouble(); }
