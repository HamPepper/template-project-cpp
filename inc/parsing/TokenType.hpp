#pragma once

#include <string>

namespace tpcpp {

enum class TokenType {
  // clang-format off
  /* single character tokens */
  LEFT_PAREN, RIGHT_PAREN,
  COMMA,
  CARET, DOT, MINUS, PLUS, SLASH, STAR,
  /* one or two character tokens */
  BANG, BANG_EQUAL,
  EQUAL, EQUAL_EQUAL,
  GREATER, GREATER_EQUAL,
  LESS, LESS_EQUAL,
  /* literals */
  IDENTIFIER,
  INTEGER, FLOAT,
  /* keywords */
  AND,
  FALSE,
  OR,
  TRUE,
  /* misc */
  END_OF_FILE,
  // clang-format on
};

} // namespace tpcpp
