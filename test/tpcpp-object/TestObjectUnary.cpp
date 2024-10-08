#include <cassert>

#include "Exceptions.hpp"
#include "Object.hpp"
#include "mockup.hpp"

namespace tpcpp {

void testCallableNegate() {
  Object c = CallablePtr{new MockUpCallable{}};
  bool expectionThrown = false;

  try {
    -c;
  } catch (RuntimeError &e) {
    expectionThrown = true;
    assert(e.message ==
           std::string("Unary operator '-' cannot be applied to a callable."));
  }

  assert(expectionThrown == true);
}

} // namespace tpcpp

int main() {
  tpcpp::testCallableNegate();

  return 0;
}
