#include "visitors/EvaluatorExpr.hpp"

#include <format>
#include <variant>

#include "Interpreter.hpp"

namespace tpcpp {

// public methods //////////////////////////////////////////////////////////////
EvaluatorExpr::EvaluatorExpr(Interpreter &interpreter)
    : m_interpreter(interpreter) {};

/// @brief The main interface evaluating all `tpcpp::Expr` to a `tpcpp::Object`.
Object EvaluatorExpr::operator()(Expr expr) { return std::visit(*this, expr); }

Object EvaluatorExpr::operator()(Box<Assign> expr) {
  auto value = (*this)(expr->value);
  m_interpreter.setVariable(expr->name.lexeme, value);
  return value;
}

Object EvaluatorExpr::operator()(Box<Binary> expr) {
  Object left = (*this)(expr->left);
  Object right = (*this)(expr->right);

  // do check first
  switch (expr->op.type) {
  case TokenType::PLUS:
  case TokenType::MINUS:
  case TokenType::STAR:
  case TokenType::SLASH:
  case TokenType::CARET:
  case TokenType::GREATER:
  case TokenType::GREATER_EQUAL:
  case TokenType::LESS:
  case TokenType::LESS_EQUAL:
    if (!isNumber(left) && !isNumber(right))
      throw RuntimeError{
          std::format("Operands must be numbers for '{}'.", expr->op.lexeme)};
    break;

  default:
    break;
  }

  // now, the actual operation
  switch (expr->op.type) {
  case TokenType::PLUS:
    return left + right;
  case TokenType::MINUS:
    return left - right;
  case TokenType::STAR:
    return left * right;
  case TokenType::SLASH:
    return left / right;
  case TokenType::CARET:
    return power(left, right);

  case TokenType::GREATER:
    return left > right;
  case TokenType::GREATER_EQUAL:
    return left >= right;
  case TokenType::LESS:
    return left < right;
  case TokenType::LESS_EQUAL:
    return left <= right;
  case TokenType::EQUAL_EQUAL:
    return left == right;
  case TokenType::BANG_EQUAL:
    return left != right;

  default:
    throw RuntimeError{
        std::format("Unknown binary operator '{}'.", expr->op.lexeme)};
  }
}

Object EvaluatorExpr::operator()(Box<Call> expr) {
  auto callee = toCallable((*this)(expr->callee));

  std::vector<Object> arguments{};
  for (auto &&arg : expr->arguments)
    arguments.push_back((*this)(arg));

  if (arguments.size() != callee->arity())
    throw RuntimeError{std::format("Expected {} arguments but got {}.",
                                   callee->arity(), arguments.size())};

  return callee->call(arguments);
}

Object EvaluatorExpr::operator()(Box<Grouping> expr) {
  return (*this)(expr->expression);
}

Object EvaluatorExpr::operator()(Box<Literal> expr) { return expr->value; }

Object EvaluatorExpr::operator()(Box<Logical> expr) {
  bool left = toBool((*this)(expr->left));

  // short-curcuiting
  if (expr->op.type == TokenType::OR) {
    if (left)
      return left;
  } else {
    if (!left)
      return left;
  }

  return toBool((*this)(expr->right));
}

Object EvaluatorExpr::operator()(Box<Unary> expr) {
  Object right = (*this)(expr->right);

  switch (expr->op.type) {
  case TokenType::MINUS:
    if (!isNumber(right))
      throw RuntimeError{
          std::format("Operand must be number for '{}'.", expr->op.lexeme)};
    return -right;
  case TokenType::BANG:
    return Object{!right};
  default:
    throw RuntimeError{
        std::format("Unknown unary operator '{}'.", expr->op.lexeme)};
  }
}

Object EvaluatorExpr::operator()(Box<Variable> expr) {
  return m_interpreter.getVariable(expr->name.lexeme);
}

} // namespace tpcpp
