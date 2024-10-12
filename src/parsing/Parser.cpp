#include <cassert>
#include <format>
#include <unordered_map>

#include "Interpreter.hpp"

#include "parsing/Expr.hpp"
#include "parsing/Parser.hpp"

namespace tpcpp {

// public methods //////////////////////////////////////////////////////////////
Parser::Parser(Interpreter &interpreter, const ListOfTokens &tokens)
    : m_interpreter(interpreter), m_tokens(tokens), m_current(0) {}

/// @brief Parse input tokens into expressions.
ListOfExprs Parser::parse() {
  ListOfExprs expressions{};
  while (!isAtEnd()) {
    try {
      expressions.push_back(expression());
    } catch (Parser::ParseException &) {
      // syntax error recovery is the parser’s job, so ParseError exception
      // should not escape into the rest of the interpreter.
      synchronize();
    }
  }

  return expressions;
}

// private methods /////////////////////////////////////////////////////////////

// parsing
// rule at the top-most has the lowest priority

/// Implement `expression -> assignment`
Expr Parser::expression() { return assignment(); }

/// Implement `assignment     -> IDENTIFIER "=" assignment | logic_or`
Expr Parser::assignment() {
  Expr expr = logicOr();

  if (match(TokenType::EQUAL)) {
    Token equals = previous();
    Expr value = assignment();

    if (isExprVariable(expr)) {
      Token name = std::get<Box<Variable>>(expr)->name;
      return Assign{name, value};
    }

    error(equals, "Invalid assignment target.");
  }

  return expr;
}

/// Implemet `logic_or -> logic_and ( "|" logic_and )*`
Expr Parser::logicOr() {
  Expr expr = logicAnd();

  while (match(TokenType::OR)) {
    Token op = previous();
    Expr right = logicAnd();
    expr = Logical{expr, op, right};
  }

  return expr;
}

/// Implemet `logic_and -> equality ( "&" equality )*`
Expr Parser::logicAnd() {
  Expr expr = equality();

  while (match(TokenType::AND)) {
    Token op = previous();
    Expr right = equality();
    expr = Logical{expr, op, right};
  }

  return expr;
}

/// Implement `equality -> comparison ( ( "!=" | "==" ) comparison )*`
Expr Parser::equality() {
  Expr expr = comparison();

  while (match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL)) {
    Token op = previous();
    if (isExprBinary(expr)) {
      auto binary = std::get<Box<Binary>>(expr);
      if (binary->op.type == TokenType::BANG_EQUAL ||
          binary->op.type == TokenType::EQUAL_EQUAL) {
        error(op, std::format("Chained equality of the form 'a {} b {} c' is "
                              "illegal. Use 'a {} b & b {} c' instead.",
                              binary->op.lexeme, op.lexeme, binary->op.lexeme,
                              op.lexeme));
      }
    }

    Expr right = comparison();
    expr = Binary{expr, op, right};
  }

  return expr;
}

/// Implement `comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )*`
Expr Parser::comparison() {
  Expr expr = term();

  while (match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS,
               TokenType::LESS_EQUAL)) {
    Token op = previous();
    if (isExprBinary(expr)) {
      auto binary = std::get<Box<Binary>>(expr);
      if (binary->op.type == TokenType::GREATER ||
          binary->op.type == TokenType::GREATER_EQUAL ||
          binary->op.type == TokenType::LESS ||
          binary->op.type == TokenType::LESS_EQUAL) {
        error(op, std::format("Chained comparison of the form 'a {} b {} c' is "
                              "illegal. Use 'a {} b & b {} c' instead.",
                              binary->op.lexeme, op.lexeme, binary->op.lexeme,
                              op.lexeme));
      }
    }

    Expr right = term();
    expr = Binary{expr, op, right};
  }

  return expr;
}

/// Implemet `term -> factor ( ( "+" | "-" ) factor )*`
Expr Parser::term() {
  Expr expr = factor();

  while (match(TokenType::PLUS, TokenType::MINUS)) {
    Token op = previous();
    Expr right = factor();
    expr = Binary(expr, op, right);
  }

  return expr;
}

/// Implement `factor -> exponent ( ( "*" | "/" ) exponent )*`
Expr Parser::factor() {
  Expr expr = exponent();

  while (match(TokenType::STAR, TokenType::SLASH)) {
    Token op = previous();
    Expr right = exponent();
    expr = Binary{expr, op, right};
  }

  return expr;
}

/// Implemet `exponent -> unary ( "^" unary )?`
Expr Parser::exponent() {
  Expr expr = unary();

  while (match(TokenType::CARET)) {
    Token op = previous();
    Expr right = unary();
    expr = Binary(expr, op, right);
  }

  return expr;
}

/// Implement `unary -> ( "!" | "-" ) unary | call`
Expr Parser::unary() {
  if (match(TokenType::BANG, TokenType::MINUS)) {
    Token op = previous();
    Expr right = unary();
    return Unary{op, right};
  }

  return call();
}

/// Implement `call -> primary ( "(" arguments? ")" )*`
Expr Parser::call() {
  Expr expr = primary();

  while (true) {
    if (match(TokenType::LEFT_PAREN))
      expr = finishCall(expr);
    break;
  }

  return expr;
}

/// Implement `arguments -> expression ( "," expression )*`
Expr Parser::finishCall(const Expr &callee) {
  ListOfExprs arguments{};

  if (!check(TokenType::RIGHT_PAREN)) {
    do {
      arguments.emplace_back(expression());
    } while (match(TokenType::COMMA));
  }

  Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

  return Call{callee, paren, arguments};
}

/// Implement:
/// ```
/// primary -> INTEGER | FLOAT | "true" | "false" |
///            | "(" expression ")"
///            | IDENTIFIER
/// ```
Expr Parser::primary() {
  // boring keywords
  if (match(TokenType::FALSE))
    return Literal{false};
  if (match(TokenType::TRUE))
    return Literal{true};

  // actual literal types
  if (match(TokenType::INTEGER, TokenType::FLOAT))
    return Literal{previous().literal, previous()};

  // ( expr )
  if (match(TokenType::LEFT_PAREN)) {
    Token paren = previous();
    Expr expr = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
    return Grouping{expr, paren};
  }

  if (match(TokenType::IDENTIFIER)) {
    return Variable(previous());
  }

  throw error(peek(), "Expect expression.");
}

// look arounds
Token Parser::advance() {
  if (!isAtEnd())
    m_current++;
  return previous();
}

Token Parser::peek() { return m_tokens[m_current]; }

Token Parser::previous() { return m_tokens[m_current - 1]; }

bool Parser::check(TokenType type) {
  if (isAtEnd())
    return false;
  return peek().type == type;
}

// helpers
bool Parser::isAtEnd() { return peek().type == TokenType::END_OF_FILE; }

// error handling
Token Parser::consume(TokenType type, const std::string &message) {
  if (check(type))
    return advance();
  throw error(peek(), message);
}

Parser::ParseException Parser::error(const Token &token,
                                     const std::string &message) {
  (void)token;
  m_interpreter.error(message);
  return ParseException{};
}

// discard tokens until the end, for error handling
void Parser::synchronize() {
  while (!isAtEnd())
    advance();
}

} // namespace tpcpp
