#pragma once

#include <string>
#include <vector>

#include "Object.hpp"

#include "parsing/TokenType.hpp"

namespace tpcpp {

/// @brief a single unit of source code, as produced by the Scanner.
struct Token {
  Token(TokenType type, std::string lexeme, Object literal = 0)
      : type(type), lexeme(lexeme), literal(literal) {};
  Token(std::string lexeme) : Token(TokenType::IDENTIFIER, lexeme, 0) {};

  TokenType type;
  std::string lexeme;
  Object literal;
};

using ListOfTokens = std::vector<Token>;

} // namespace tpcpp
