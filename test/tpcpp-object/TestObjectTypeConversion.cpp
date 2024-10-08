#include <cassert>

#include "Object.hpp"
#include "mockup.hpp"

namespace tpcpp {

void testFromDoubleToBool() {
  Object o = 3.14;
  assert(toBool(o) == true);
}

void testFromDoubleToLong() {
  Object o = 13.3;
  assert(toLong(o) == 13L);
}

void testFromBoolToDouble() {
  Object o = false;
  assert(toDouble(o) == 0.0);
}

void testFromBoolToString() {
  Object o = true;
  assert(toString(o) == "true");
}

void testFromCallableToString() {
  Object c = CallablePtr{new MockUpCallable{}};
  assert(toString(c) == "this is a mockup callable");
}

} // namespace tpcpp

int main() {
  tpcpp::testFromDoubleToBool();
  tpcpp::testFromDoubleToLong();
  tpcpp::testFromBoolToDouble();
  tpcpp::testFromBoolToString();
  tpcpp::testFromCallableToString();

  return 0;
}
