#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#pragma push_macro("NDEBUG")
#undef NDEBUG // to shut up unused variable warning in release build
#include <cassert>
#pragma pop_macro("NDEBUG")

#include "Exceptions.hpp"
#include "Interpreter.hpp"

#include "parsing/Parser.hpp"
#include "parsing/Scanner.hpp"

#include "visitors/PrinterExpr.hpp"

namespace tpcpp {

void testParserValidInputs() {
  // clang-format off
  std::vector<std::string> inputs {
    "1 + 2 * 3",
    "(1 + 2) * 3",
    "func(1, 2, 3)",
    "3^4 * 2 + 1",
    "a & b | c & d",
    "func1(func2(), 2)"
  };

  std::vector<std::string> expected {
    "(+ 1 (* 2 3))",
    "(* (group (+ 1 2)) 3)",
    "(call func (1 2 3))",
    "(+ (* (^ 3 4) 2) 1)",
    "(| (& a b) (& c d))",
    "(call func1 ((call func2 ()) 2))"
  };
  // clang-format on

  Interpreter interpreter{};
  for (int idx = 0; const auto &src : inputs) {
    Scanner scanner{interpreter, src};
    auto tokens = scanner.scanTokens();

    Parser parser{interpreter, tokens};
    auto expr = parser.parse()[0];

    auto str = PrinterExpr{}(expr);
    assert(str == expected[idx]);

    idx++;
  }
}

void testParserInvalidInputs() {
  // clang-format off
  std::vector<std::string> inputs {
    "1+",
    "%",
    "func(1, 2"
  };

  std::vector<std::string> expectedError {
    "Expect expression.\n",
    "Unexpected character.\n",
    "Expect ')' after arguments.\n"
  };
  // clang-format on

  Interpreter interpreter{};
  for (int idx = 0; const auto &src : inputs) {
    std::streambuf *originalBuffer = std::cout.rdbuf();
    std::ostringstream stringStream;
    std::cout.rdbuf(stringStream.rdbuf());

    Scanner scanner{interpreter, src};
    auto tokens = scanner.scanTokens();

    Parser parser{interpreter, tokens};
    (void)parser.parse();

    // redirect std out to a string
    auto str = stringStream.str();
    assert(str == expectedError[idx]);

    std::cout.rdbuf(originalBuffer);
    idx++;
  }
}

} // namespace tpcpp

int main() {
  tpcpp::testParserValidInputs();
  tpcpp::testParserInvalidInputs();

  return 0;
}
