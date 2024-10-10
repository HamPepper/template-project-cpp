#include <cmath>
#include <iostream>
#include <limits>

#pragma push_macro("NDEBUG")
#undef NDEBUG // to shut up unused variable warning in release build
#include <cassert>
#pragma pop_macro("NDEBUG")

#include "Exceptions.hpp"
#include "Object.hpp"
#include "mockup.hpp"

namespace tpcpp {

void testBoolAddDouble() {
  Object b = true;
  Object d = 3.14;
  assert(std::abs(std::get<double>(b + d) - 4.14) <= 1e-10); // due to precision
}

void testBoolAddLong() {
  Object b = true;
  Object l = 233;
  assert(std::get<long>(b + l) == 234);
}

void testDoubleAddCallable() {
  Object d = 3.14;
  Object c = CallablePtr{new MockUpCallable{}};
  bool exceptionThrown = false;

  try {
    d + c;
  } catch (const RuntimeError &e) {
    exceptionThrown = true;
    assert(e.message ==
           std::string("Binary operator '+' cannot be applied to a callable."));
  }

  assert(exceptionThrown == true);
}

void testCallableAddCallable() {
  Object c1 = CallablePtr{new MockUpCallable{}};
  Object c2 = CallablePtr{new MockUpCallable{}};
  bool exceptionThrown = false;

  try {
    c1 + c2;
  } catch (const RuntimeError &e) {
    exceptionThrown = true;
    assert(e.message ==
           std::string("Binary operator '+' cannot be applied to a callable."));
  }

  assert(exceptionThrown == true);
}

void testBoolSubLong() {
  Object b = true;
  Object l = 233;
  auto result = std::get<long>(b - l);
  (void)result;
  assert(result == -232);
}

void testDoubleSubCallable() {
  Object d = 3.14;
  Object c = CallablePtr{new MockUpCallable{}};
  bool exceptionThrown = false;

  try {
    d - c;
  } catch (const RuntimeError &e) {
    exceptionThrown = true;
    assert(e.message ==
           std::string("Binary operator '-' cannot be applied to a callable."));
  }

  assert(exceptionThrown == true);
}

void testBoolMulLong() {
  Object b = true;
  Object l = 233;
  assert(std::get<long>(b * l) == 233);
}

void testDoubleMulCallable() {
  Object d = 3.14;
  Object c = CallablePtr{new MockUpCallable{}};
  bool exceptionThrown = false;

  try {
    d *c;
  } catch (const RuntimeError &e) {
    exceptionThrown = true;
    assert(e.message ==
           std::string("Binary operator '*' cannot be applied to a callable."));
  }

  assert(exceptionThrown == true);
}

void testBoolDivLong() {
  Object b = true;
  Object l = 233;
  assert(std::get<long>(b / l) == 0);
}

void testLongDivByZero() {
  Object l = 233;
  assert(std::isnan(std::get<double>(l / 0)));
}

void testDoubleDivCallable() {
  Object d = 3.14;
  Object c = CallablePtr{new MockUpCallable{}};
  bool exceptionThrown = false;

  try {
    d / c;
  } catch (const RuntimeError &e) {
    exceptionThrown = true;
    assert(e.message ==
           std::string("Binary operator '/' cannot be applied to a callable."));
  }

  assert(exceptionThrown == true);
}

void testLongPowerBool() {
  Object b = true;
  Object l = 233;
  assert(std::get<long>(power(l, b)) == 233);
}

void testLongPowerLong() {
  Object l1 = 3;
  Object l2 = 4;
  assert(std::get<long>(power(l1, l2)) == 81L);
}

void testDoublePowerLong() {
  Object d = 3.0;
  Object l = 4;
  assert(std::get<double>(power(d, l)) == 81.0);
}

void testDoublePowerCallable() {
  Object d = 3.14;
  Object c = CallablePtr{new MockUpCallable{}};
  bool exceptionThrown = false;

  try {
    power(d, c);
  } catch (const RuntimeError &e) {
    exceptionThrown = true;
    assert(e.message ==
           std::string("'power' function cannot be applied to a callable."));
  }

  assert(exceptionThrown == true);
}

} // namespace tpcpp

int main() {
  tpcpp::testBoolAddDouble();
  tpcpp::testBoolAddLong();
  tpcpp::testDoubleAddCallable();
  tpcpp::testCallableAddCallable();

  tpcpp::testBoolSubLong();
  tpcpp::testDoubleSubCallable();

  tpcpp::testBoolMulLong();
  tpcpp::testDoubleMulCallable();

  tpcpp::testBoolDivLong();
  tpcpp::testLongDivByZero();
  tpcpp::testDoubleDivCallable();

  tpcpp::testLongPowerBool();
  tpcpp::testLongPowerLong();
  tpcpp::testDoublePowerLong();
  tpcpp::testDoublePowerCallable();

  return 0;
}
