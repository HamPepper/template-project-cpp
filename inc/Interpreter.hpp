#pragma once

#include <string>

namespace tpcpp {

class Interpreter {
public:
  Interpreter();
  Interpreter(const Interpreter &) = delete; // no copy allowed
  ~Interpreter() = default;

  void run();
  // Object evaluate(const std::string& expr);

  void error(const std::string &message);
  // void runtimeError(const RuntimeError& error);

private:
  bool m_hadError;
  bool m_hadRuntimeError;

  void registerFunctions();
};

} // namespace tpcpp
