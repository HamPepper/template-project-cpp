#include <format>
#include <iostream>

#include "parsing/Expr.hpp"
#include "visitors/PrinterExpr.hpp"
#include "visitors/helpers.hpp"

namespace tpcpp {

/// @brief Convert a `tpcpp::Expr` to a string.
///
/// @param[in] expr The `tpcpp::Expr` to convert.
/// @return The string representation of the `tpcpp::Expr`.
std::string PrinterExpr::operator()(Expr expr) {
  auto hdlAssign = [this](Box<Assign> e) -> std::string {
    return std::format("(assign {} {})", e->name.lexeme, operator()(e->value));
  };
  auto hdlBinary = [this](Box<Binary> e) -> std::string {
    return std::format("({} {} {})",
                       e->op.lexeme, operator()(e->left), operator()(e->right));
  };
  auto hdlCall = [this](Box<Call> e) -> std::string {
    std::string args;
    for (auto it = e->arguments.begin(); it != e->arguments.end(); it++) {
      args += operator()(*it) +
              (std::next(it) == e->arguments.end() ? "" : " ");
    }
    return std::format("(call {} ({}))", operator()(e->callee), args);
  };
  auto hdlGrouping = [this](Box<Grouping> e) -> std::string {
    return std::format("(group {})", operator()(e->expression));
  };
  auto hdlLiteral = [](Box<Literal> e) -> std::string {
    return toString(e->value);
  };
  auto hdlLogical = [this](Box<Logical> e) -> std::string {
    return std::format("({} {} {})",
                       e->op.lexeme, operator()(e->left), operator()(e->right));
  };
  auto hdlUnary = [this](Box<Unary> e) -> std::string {
    return std::format("({} {})", e->op.lexeme, operator()(e->right));
  };
  auto hdlVariable = [](Box<Variable> e) -> std::string {
    return e->name.lexeme;
  };

  return std::visit(OpCollector{hdlAssign, hdlBinary, hdlCall, hdlGrouping,
                                hdlLiteral, hdlLogical, hdlUnary, hdlVariable},
                    expr);
}

/// @brief Print a `tpcpp::Expr` to stdout.
///
/// @param[in] expr The `tpcpp::Expr` to print.
void PrinterExpr::print(Expr expr) {
  auto str = operator()(expr);
  std::cout << str << std::endl;
}

} // namespace tpcpp
