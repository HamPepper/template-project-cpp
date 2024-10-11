#include <unordered_map>

#include "Interpreter.hpp"

#include "parsing/Scanner.hpp"

namespace tpcpp {

// clang-format off
const std::unordered_map<std::string, TokenType> gReservedKeywords {
  {"false",    TokenType::FALSE},
  {"true",     TokenType::TRUE},
};
// clang-format on

// public methods //////////////////////////////////////////////////////////////
Scanner::Scanner(Interpreter &interpreter, std::string source)
    : m_interpreter(interpreter), m_source(source), m_tokens(), m_start(0),
      m_current(0) {}

/// @brief Scan the source code stored in `m_source`.
///
/// @return a vector of Tokens.
ListOfTokens Scanner::scanTokens() {
  while (!isAtEnd()) {
    // the beginning of the next lexeme
    m_start = m_current;
    scanToken();
  }

  m_tokens.emplace_back(TokenType::END_OF_FILE, "");
  return m_tokens;
}

// private methods /////////////////////////////////////////////////////////////

// scanning
void Scanner::scanToken() {
  char c = advance();
  switch (c) {
  case '(':
    addToken(TokenType::LEFT_PAREN);
    break;
  case ')':
    addToken(TokenType::RIGHT_PAREN);
    break;
  case ',':
    addToken(TokenType::COMMA);
    break;
  case '.':
    addToken(TokenType::DOT);
    break;
  case '+':
    addToken(TokenType::PLUS);
    break;
  case '-':
    addToken(TokenType::MINUS);
    break;
  case '*':
    addToken(TokenType::STAR);
    break;
  case '/':
    addToken(TokenType::SLASH);
    break;
  case '&':
    addToken(TokenType::AND);
    break;
  case '|':
    addToken(TokenType::OR);
    break;
  case '^':
    addToken(TokenType::CARET);
    break;

  case '!':
    addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
    break;
  case '=':
    addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
    break;
  case '<':
    addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
    break;
  case '>':
    addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
    break;

  case ' ':
  case '\t':
  case '\r':
  case '\n':
    break; // skip whitespaces and EOL indicators

  default:
    if (isDigit(c))
      number();
    else if (isAlpha(c))
      identifier();
    else
      m_interpreter.error("Unexpected character.");
    break;
  }
}

void Scanner::addToken(TokenType type, std::string literal) {
  size_t count = m_current - m_start;
  auto lexeme = m_source.substr(m_start, count);

  switch (type) {
  case TokenType::INTEGER:
    m_tokens.emplace_back(type, lexeme, std::stol(literal));
    break;
  case TokenType::FLOAT:
    m_tokens.emplace_back(type, lexeme, std::stod(literal));
    break;
  default:
    m_tokens.emplace_back(type, lexeme);
    break;
  }
}

// look aheads
// we look maximally 2 characters ahead
// get m_source[m_current], then increment m_current
char Scanner::advance() { return m_source[m_current++]; }

char Scanner::peek() {
  if (isAtEnd())
    return '\0';
  return m_source[m_current];
}

char Scanner::peekNext() {
  if (m_current + 1 >= m_source.length())
    return '\0';
  return m_source[m_current + 1];
}

// a conditional 'advance'
bool Scanner::match(char expected) {
  if (isAtEnd())
    return false;
  if (m_source[m_current] != expected)
    return false;

  m_current++;
  return true;
}

// specific-scanners
void Scanner::number() {
  bool isFloat = false;
  while (isDigit(peek()))
    advance();

  if (peek() == '.' && isDigit(peekNext())) {
    // consume '.' only in this case
    advance();
    isFloat = true;
    while (isDigit(peek()))
      advance();
  }

  size_t count = m_current - m_start;
  auto literal = m_source.substr(m_start, count);
  addToken(isFloat ? TokenType::FLOAT : TokenType::INTEGER, literal);
}

void Scanner::identifier() {
  while (isAlphaNumeric(peek()))
    advance();

  size_t count = m_current - m_start;
  auto identifier = m_source.substr(m_start, count);

  auto it = gReservedKeywords.find(identifier);
  addToken(it == gReservedKeywords.end() ? TokenType::IDENTIFIER : it->second);
}

// helpers
bool Scanner::isAtEnd() { return m_current == m_source.length(); }

bool Scanner::isDigit(char c) { return c >= '0' && c <= '9'; }

bool Scanner::isAlpha(char c) {
  // clang-format off
  return (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') ||
          c == '_'
         ;
  // clang-format off
}

bool Scanner::isAlphaNumeric(char c) {
  return isAlpha(c) || isDigit(c);
}

}
