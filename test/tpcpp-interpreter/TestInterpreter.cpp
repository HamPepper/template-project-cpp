#include <cmath>
#include <string>

#pragma push_macro("NDEBUG")
#undef NDEBUG // to shut up unused variable warning in release build
#include <cassert>
#pragma pop_macro("NDEBUG")

#include "Interpreter.hpp"

namespace tpcpp {

void testInterpreterBasic() {
  std::string source = "1 + 2 * 3^4 - 5.0";

  Interpreter interpreter{};
  auto result = interpreter.evaluate(source);

  assert(std::abs(std::get<double>(result) - (1 + 2 * 81 - 5.0)) <= 1e-10);
}

void testInterpreterFuncCall() {
  std::string source = "abs(-1)";

  Interpreter interpreter{};
  auto result = interpreter.evaluate(source);

  assert(std::abs(std::get<double>(result) - 1) <= 1e-10);
}

} // namespace tpcpp

int main() {
  tpcpp::testInterpreterBasic();
  tpcpp::testInterpreterFuncCall();

  return 0;
}
