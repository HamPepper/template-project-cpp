#pragma once

#include <variant>
#include <vector>

#include "Object.hpp"

#include "parsing/Box.hpp"
#include "parsing/Token.hpp"

namespace tpcpp {

// clang-format off
// forward declarations of each expression type
struct Assign;
struct Binary;
struct Call;
struct Grouping;
struct Literal;
struct Logical;
struct Unary;
struct Variable;

/// @typedef tpcpp::Expr
/// @brief A variant type that can hold any of the expression types
using Expr = std::variant<
  Box<Assign>,
  Box<Binary>,
  Box<Call>,
  Box<Grouping>,
  Box<Literal>,
  Box<Logical>,
  Box<Unary>,
  Box<Variable>
>;
// clang-format on

using ListOfExprs = std::vector<Expr>;

// predicates //////////////////////////////////////////////////////////////////
bool isExprBinary(const Expr &expr);
bool isExprVariable(const Expr &expr);

// Expr structs ////////////////////////////////////////////////////////////////
struct Assign {
  Assign(const Token &name, const Expr &value) : name(name), value(value) {};

  Token name;
  Expr value;
};

struct Binary {
  Binary(const Token &op, const Expr &left, const Expr &right)
      : op(op), left(left), right(right) {};
  // allow infix notation for binary operators
  Binary(const Expr &left, const Token &op, const Expr &right)
      : Binary(op, left, right) {};

  Token op;
  Expr left;
  Expr right;
};

struct Call {
  Call(const Expr &callee, const Token &paren, const ListOfExprs &arguments)
      : callee(callee), paren(paren), arguments(arguments) {};

  Expr callee;
  Token paren;
  ListOfExprs arguments;
};

struct Grouping {
  Grouping(const Expr &expression, const Token &paren = Token{})
      : expression(expression), paren(paren) {};

  Expr expression;
  Token paren;
};

struct Literal {
  Literal(const Object &value, const Token &token = Token{})
      : value(value), token(token) {};

  Object value;
  Token token;
};

struct Logical {
  Logical(const Token &op, const Expr &left, const Expr &right)
      : op(op), left(left), right(right) {};
  Logical(const Expr &left, const Token &op, const Expr &right)
      : Logical(op, left, right) {};

  Token op;
  Expr left;
  Expr right;
};

struct Unary {
  Unary(const Token &op, const Expr &right) : op(op), right(right) {};

  Token op;
  Expr right;
};

struct Variable {
  Variable(const Token &name) : name(name) {};

  Token name;
};

} // namespace tpcpp
