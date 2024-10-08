#include "Interpreter.hpp"

#include <csignal>
#include <iostream>

namespace tpcpp {

// local function declarations
static void handlerCtrlC(int sig);

// public methods //////////////////////////////////////////////////////////////
Interpreter::Interpreter() : m_hadError(false), m_hadRuntimeError(false) {}

/// @brief Start the inteprter REPL loop.
void Interpreter::run() {
  std::signal(SIGINT, handlerCtrlC); // install handler

  std::string line;
  while (true) {
    printf("> ");
    line.clear();

    std::getline(std::cin, line);
    if (std::cin.eof())
      break;
    // evaluate(line);

    // reset error in interactive loop
    if (m_hadError)
      m_hadError = false;
    if (m_hadRuntimeError)
      m_hadRuntimeError = false;
  }
}

/// @brief Indicate a parsing-time error to the inteprter.
///
/// @param[in] message The error message.
void Interpreter::error(const std::string &message) {
  (void)message;
  return;
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
