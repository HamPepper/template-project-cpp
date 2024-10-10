#pragma once

#include <string>
#include <unordered_map>

#include "parsing/Token.hpp"
#include "parsing/TokenType.hpp"

namespace tpcpp {

class Interpreter;

// clang-format off
const std::unordered_map<std::string, TokenType> RESERVED_KEYWORDS{
  {"and",      TokenType::AND},
  {"false",    TokenType::FALSE},
  {"not",      TokenType::BANG},
  {"or",       TokenType::OR},
  {"true",     TokenType::TRUE},
};
// clang-format on

/// @brief The tpcpp scanner/lexer.
class Scanner {
public:
  Scanner(Interpreter &interpreter, std::string source);

  ListOfTokens scanTokens();

private:
  Interpreter &m_interpreter;

  std::string m_source;
  ListOfTokens m_tokens;

  size_t m_start;   // current lexeme start, an index of m_source
  size_t m_current; // cursor, an index of m_source

  // scanning
  void scanToken();
  void addToken(TokenType type, std::string literal = "");

  // look aheads
  char advance();
  char peek();
  char peekNext();

  bool match(char expected);

  // specific-scanners
  void number();
  void identifier();

  // helpers
  bool isAtEnd();
  bool isDigit(char c);
  bool isAlpha(char c, bool allowSpecial = false);
  bool isAlphaNumeric(char c);
};

} // namespace tpcpp
