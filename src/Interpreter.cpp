#include "Interpreter.hpp"

#include <cmath>
#include <csignal>
#include <format>
#include <functional>
#include <iostream>

#include "Function.hpp"

#include "parsing/Parser.hpp"
#include "parsing/Scanner.hpp"

// #define DEBUG_PARSER
#ifdef DEBUG_PARSER
#include "visitors/PrinterExpr.hpp"
#endif
#include "visitors/EvaluatorExpr.hpp"

namespace tpcpp {

// local function declarations
static void handlerCtrlC(int sig);

// public methods //////////////////////////////////////////////////////////////
Interpreter::Interpreter() : m_hadError(false), m_hadRuntimeError(false) {
  std::signal(SIGINT, handlerCtrlC); // install handler
  registerFunctions();
}

/// @brief Start the interpreter REPL loop.
void Interpreter::run() {
  std::string line;
  while (true) {
    std::cout << "> ";
    line.clear();

    std::getline(std::cin, line);
    if (std::cin.eof())
      break;
    auto result = evaluate(line);
    std::cout << toString(result) << std::endl;

    // reset error in interactive loop
    if (m_hadError)
      m_hadError = false;
    if (m_hadRuntimeError)
      m_hadRuntimeError = false;
  }
}

/// @brief Evaluate a given string.
///
/// @param[in] source The source code to evaluate.
///
/// @return The result of the evaluation, which is an `tpcpp::Object`.
Object Interpreter::evaluate(const std::string &source) {
  Scanner scanner(*this, source);
  auto tokens = scanner.scanTokens();

  Parser parser(*this, tokens);
  auto exprs = parser.parse();

#ifdef DEBUG_PARSER
  PrinterExpr printer{};
  for (const auto &e : exprs)
    printer.print(e);
#endif

  Object result{};
  EvaluatorExpr evaluator{*this};
  try {
    for (const auto &e : exprs)
      result = evaluator(e);
  } catch (const RuntimeError &e) {
    runtimeError(e);
  }

  return result;
}

/// @brief Indicate a parsing-time error to the inteprter.
///
/// @param[in] message The error message.
void Interpreter::error(const std::string &message) {
  std::cout << message << std::endl;
  m_hadError = true;
}

/// @brief Indicate a run-time error to the inteprter.
///
/// @param[in] error A `tpcpp::RuntimeError` instance.
void Interpreter::runtimeError(const RuntimeError &error) {
  std::cout << error.message << std::endl;
  m_hadRuntimeError = true;
}

/// @brief Set a global variable.
///
/// @param[in] name The name of the variable.
/// @param[in] value The value of the variable.
void Interpreter::setVariable(const std::string &name, const Object &value) {
  m_globals[name] = value;
}

/// @brief Get a global variable.
///
/// @param[in] name The name of the variable.
/// @return The value of the variable.
Object Interpreter::getVariable(const std::string &name) {
  auto it = m_globals.find(name);
  if (it == m_globals.end())
    throw RuntimeError(std::format("Undefined variable '{}'", name));
  return it->second;
}

// private methods /////////////////////////////////////////////////////////////

/// @brief Register native functions to the tpcpp interpreter.
///
/// @remark
/// Currently the following functions are provided
/// - `sin`
/// - `cos`
/// - `abs`
/// - `max`
/// - `min`
void Interpreter::registerFunctions() {
  auto reg = [&](const CallablePtr &ptr) { m_globals[ptr->name()] = ptr; };

  // clang-format off
  reg(CallablePtr{new Function{"sin",
      std::function<double(double)>{[](double x) { return std::sin(x); }}}});
  reg(CallablePtr{new Function{"cos",
      std::function<double(double)>{[](double x) { return std::cos(x); }}}});
  reg(CallablePtr{new Function{"abs",
      std::function<double(double)>{[](double x) { return std::abs(x); }}}});

  reg(CallablePtr{new Function{"max",
      std::function<double(double, double)>{
          [](double x, double y) -> double {return std::max(x, y); }
      }}});
  reg(CallablePtr{new Function{"min",
      std::function<double(double, double)>{
          [](double x, double y) -> double {return std::min(x, y); }
      }}});
  // clang-format on
}

// static helpers //////////////////////////////////////////////////////////////
static void handlerCtrlC(int sig) {
  (void)sig;
  std::cout << "Ctrl-C caught, terminating..." << std::endl;
  exit(1);
}

} // namespace tpcpp
