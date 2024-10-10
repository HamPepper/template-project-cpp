#include <string>

#pragma push_macro("NDEBUG")
#undef NDEBUG // to shut up unused variable warning in release build
#include <cassert>
#pragma pop_macro("NDEBUG")

#include "Exceptions.hpp"
#include "Interpreter.hpp"
#include "parsing/Scanner.hpp"

namespace tpcpp {

void testScannerPlus() {
  std::string source = "1 + 2";

  Interpreter interpreter{};
  Scanner scanner{interpreter, source};

  auto tokens = scanner.scanTokens();

  assert(tokens.size() == 4);

  assert(tokens[0].type == TokenType::INTEGER);
  assert(tokens[0].lexeme == "1");
  assert(tokens[0].literal == 1);

  assert(tokens[1].type == TokenType::PLUS);
  assert(tokens[1].lexeme == "+");

  assert(tokens[2].type == TokenType::INTEGER);
  assert(tokens[2].lexeme == "2");
  assert(tokens[2].literal == 2);

  assert(tokens[3].type == TokenType::END_OF_FILE);
}

void testScannerPower() {
  std::string source = "3.0^4";

  Interpreter interpreter{};
  Scanner scanner{interpreter, source};

  auto tokens = scanner.scanTokens();

  assert(tokens.size() == 4);

  assert(tokens[0].type == TokenType::FLOAT);
  assert(tokens[0].lexeme == "3.0");
  assert(tokens[0].literal == 3.0);

  assert(tokens[1].type == TokenType::CARET);
  assert(tokens[1].lexeme == "^");

  assert(tokens[2].type == TokenType::INTEGER);
  assert(tokens[2].lexeme == "4");
  assert(tokens[2].literal == 4);

  assert(tokens[3].type == TokenType::END_OF_FILE);
}

void testScannerId() {
  std::string source = "id";

  Interpreter interpreter{};
  Scanner scanner{interpreter, source};

  auto tokens = scanner.scanTokens();

  assert(tokens.size() == 2);

  assert(tokens[0].type == TokenType::IDENTIFIER);
  assert(tokens[0].lexeme == "id");

  assert(tokens[1].type == TokenType::END_OF_FILE);
}

void testScannerUndefinedSymbol() {
  std::string source = "$";

  Interpreter interpreter{};
  Scanner scanner{interpreter, source};

  try {
    (void)scanner.scanTokens();
  } catch (const RuntimeError &e) {
    assert(e.message == "Unexpected character.");
  }
}

} // namespace tpcpp

int main() {
  tpcpp::testScannerPlus();
  tpcpp::testScannerPower();
  tpcpp::testScannerId();
  tpcpp::testScannerUndefinedSymbol();

  return 0;
}
