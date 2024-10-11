#pragma once

#include <exception>
#include <string>
#include <type_traits>

#include "parsing/Expr.hpp"
#include "parsing/Token.hpp"
#include "parsing/TokenType.hpp"

namespace tpcpp {

template <typename T>
concept IsTokenType = std::is_same_v<T, TokenType>;

class Interpreter;

/// @brief A recursive-descendent parser.
///
/// Here's the grammar. Top-most rule has the lowest priority.
///
/// ```
/// program        -> expression
/// expression     -> assignment
/// assignment     -> IDENTIFIER "=" assignment | logic_or
/// logic_or       -> logic_and ( "|" logic_and )*
/// logic_and      -> equality ( "&" equality )*
/// equality       -> comparison ( ( "!=" | "==" ) comparison )*
/// comparison     -> term ( ( ">" | ">=" | "<" | "<=" ) term )*
/// term           -> factor ( ( "+" | "-" ) factor )*
/// factor         -> exponent ( ( "*" | "/" ) exponent )*
/// exponent       -> unary ( "^" unary )*
/// unary          -> ( "!" | "-" )? unary | call
/// call           -> primary ( "(" arguments? ")" )*
///   arguments    -> expression ( "," expression )*
/// primary        -> INTEGER | FLOAT | "true" | "false"
///                   | "(" expression ")"
///                   | IDENTIFIER
/// ```
///
/// @note
/// Left-associative rules do not contain a reference to themselves.
///
/// @note
/// In recursive-descent, right-associative rules do reference themselves.
///
/// @note
/// In tpcpp, assignment is an expression, instead of a statement.
/// assignment is also right-associative.
class Parser {
public:
  Parser(Interpreter &interpreter, const ListOfTokens &tokens);

  ListOfExprs parse();

private:
  class ParseException : public std::exception {};

  Interpreter &m_interpreter;
  const ListOfTokens &m_tokens;
  unsigned long m_current;

  // parsing
  Expr expression();
  Expr assignment();
  Expr logicOr();
  Expr logicAnd();
  Expr equality();
  Expr comparison();
  Expr term();
  Expr factor();
  Expr exponent();
  Expr unary();
  Expr call();
  Expr finishCall(const Expr &callee);
  Expr primary();

  // look arounds
  Token advance();
  Token peek();
  Token previous();
  bool check(TokenType type);

  template <IsTokenType T> bool match(T type) {
    if (check(type)) {
      advance();
      return true;
    }
    return false;
  }

  template <IsTokenType... Ts> bool match(Ts... types) {
    return (match(types) || ...);
  }

  // helpers
  bool isAtEnd();

  // error handling
  Token consume(TokenType type, const std::string &message);
  ParseException error(const Token &token, const std::string &message);

  void synchronize();
};

} // namespace tpcpp
