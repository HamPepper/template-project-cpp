#include <cassert>

#include "Object.hpp"
#include "mockup.hpp"

namespace tpcpp {

void testBoolIsInt() {
  Object o = true;
  assert(isInt(o) == true);
}

void testLongIsInt() {
  Object o = 13L;
  assert(isInt(o) == true);
}

void testDoubleIsInt() {
  Object o = 3.14;
  assert(isInt(o) == false);
}

void testDoubleIsNumber() {
  Object o = 3.14;
  assert(isNumber(o) == true);
}

void testCallableIsNumber() {
  Object c = CallablePtr{new MockUpCallable{}};
  assert(isNumber(c) == false);
}

} // namespace tpcpp

int main() {
  tpcpp::testBoolIsInt();
  tpcpp::testLongIsInt();
  tpcpp::testDoubleIsInt();
  tpcpp::testDoubleIsNumber();
  tpcpp::testCallableIsNumber();

  return 0;
}
