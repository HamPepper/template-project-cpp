#pragma once

#include <string>

#include "parsing/Token.hpp"
#include "parsing/TokenType.hpp"

namespace tpcpp {

class Interpreter;

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
  bool isAlpha(char c);
  bool isAlphaNumeric(char c);
};

} // namespace tpcpp
