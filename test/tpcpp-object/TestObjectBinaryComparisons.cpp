#include <cassert>

#include "Object.hpp"
#include "mockup.hpp"

namespace tpcpp {

void testBoolEqDouble() {
  Object b = true;
  Object d = 1.0;
  assert(b == d);
}

void testBoolNeqDouble() {
  Object b = true;
  Object d = 3.14;
  assert(b != d);
}

void testLongNeqDouble() {
  Object l = 3L;
  Object d = 3.14;
  assert(l != d);
}

void testLongNeqCallable() {
  Object l = 3L;
  Object c = CallablePtr{new MockUpCallable{}};
  assert(l != c);
}

} // namespace tpcpp

int main() {
  tpcpp::testBoolEqDouble();
  tpcpp::testBoolNeqDouble();
  tpcpp::testLongNeqDouble();
  tpcpp::testLongNeqCallable();

  return 0;
}
