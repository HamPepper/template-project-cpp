#pragma push_macro("NDEBUG")
#undef NDEBUG // to shut up unused variable warning in release build
#include <cassert>
#pragma pop_macro("NDEBUG")

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

  assert(exceptionThrown == true);
}

} // namespace tpcpp

int main() {
  tpcpp::testCallableNegate();

  return 0;
}
