#include "Object.hpp"

#include "parsing/Expr.hpp"

namespace tpcpp {

class Interpreter;

/// @brief Evaluate each `tpcpp::Expr` to a `tpcpp::Object`.
struct EvaluatorExpr {
  EvaluatorExpr(Interpreter &interpreter);

  Object operator()(Expr expr);

  Object operator()(Box<Assign> expr);
  Object operator()(Box<Binary> expr);
  // Object operator()(Box<Call> expr);
  Object operator()(Box<Grouping> expr);
  Object operator()(Box<Literal> expr);
  Object operator()(Box<Logical> expr);
  Object operator()(Box<Unary> expr);
  Object operator()(Box<Variable> expr);

  // fallback
  template <typename T> Object operator()(T) { return Object{}; }

private:
  Interpreter &m_interpreter;
};

} // namespace tpcpp
