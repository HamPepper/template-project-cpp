#include <cassert>
#include <iostream>

#include "Exceptions.hpp"
#include "Object.hpp"
#include "mockup.hpp"

namespace tpcpp {

void testCallableNegate() {
  Object c = CallablePtr{new MockUpCallable{}};
  bool exceptionThrown = false;

  try {
    -c;
  } catch (RuntimeError &e) {
    exceptionThrown = true;
    assert(e.message ==
           std::string("Unary operator '-' cannot be applied to a callable."));
  }

  if (!exceptionThrown)
    std::cerr << "Expected exception was not thrown!" << std::endl;
  assert(exceptionThrown == true);
}

} // namespace tpcpp

int main() {
  tpcpp::testCallableNegate();

  return 0;
}
