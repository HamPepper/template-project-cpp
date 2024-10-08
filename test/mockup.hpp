#include "Object.hpp"

namespace tpcpp {

struct MockUpCallable : public Callable {
  Object call(const ListOfObjects &) { return true; };
  const std::string toString() const { return "this is a mockup callable"; };
  size_t arity() const { return 0; };
};

} // namespace tpcpp
