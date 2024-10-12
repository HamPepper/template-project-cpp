#include <string>

#include "Object.hpp"

#include "parsing/Expr.hpp"

namespace tpcpp {

/// @brief Print each tpcpp::Expr as a string.
struct PrinterExpr {
  std::string operator()(Expr expr);
  void print(Expr expr);
};

} // namespace tpcpp
