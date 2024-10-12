#include "Interpreter.hpp"

#include <csignal>
#include <iostream>

#include "parsing/Parser.hpp"
#include "parsing/Scanner.hpp"

// #define DEBUG_PARSER
#ifdef DEBUG_PARSER
#include "visitors/PrinterExpr.hpp"
#endif

namespace tpcpp {

// local function declarations
static void handlerCtrlC(int sig);

// public methods //////////////////////////////////////////////////////////////
Interpreter::Interpreter() : m_hadError(false), m_hadRuntimeError(false) {
  std::signal(SIGINT, handlerCtrlC); // install handler
}

/// @brief Start the inteprter REPL loop.
void Interpreter::run() {
  std::string line;
  while (true) {
    printf("> ");
    line.clear();

    std::getline(std::cin, line);
    if (std::cin.eof())
      break;
    auto result = evaluate(line);

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

  return false;
}

/// @brief Indicate a parsing-time error to the inteprter.
///
/// @param[in] message The error message.
void Interpreter::error(const std::string &message) {
  std::cout << message << std::endl;
  m_hadError = true;
}

// private methods /////////////////////////////////////////////////////////////
void Interpreter::registerFunctions() {
  // register native functions
  return;
}

// static helpers //////////////////////////////////////////////////////////////
static void handlerCtrlC(int sig) {
  (void)sig;
  std::cout << "Ctrl-C caught, terminating..." << std::endl;
  exit(1);
}

} // namespace tpcpp
