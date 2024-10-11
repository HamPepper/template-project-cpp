#include "parsing/Expr.hpp"
#include "parsing/Box.hpp"
#include "visitors/helpers.hpp"

namespace tpcpp {

// predicates //////////////////////////////////////////////////////////////////
bool isExprBinary(const Expr &expr) {
  auto hdlBinary = [](Box<Binary>) -> bool { return true; };
  auto hdlGen = [](auto) -> bool { return false; };
  return std::visit(OpCollector{hdlBinary, hdlGen}, expr);
}

bool isExprVariable(const Expr &expr) {
  auto hdlVariable = [](Box<Variable>) -> bool { return true; };
  auto hdlGen = [](auto) -> bool { return false; };
  return std::visit(OpCollector{hdlVariable, hdlGen}, expr);
}

} // namespace tpcpp
