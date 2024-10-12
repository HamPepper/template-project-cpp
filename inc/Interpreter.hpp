#pragma once

#include <string>
#include <unordered_map>

#include "Exceptions.hpp"
#include "Object.hpp"

namespace tpcpp {

/// @brief The tpcpp interpreter, which is a simple calculator.
class Interpreter {
public:
  Interpreter();
  Interpreter(const Interpreter &) = delete; // no copy allowed
  ~Interpreter() = default;

  void run();
  Object evaluate(const std::string &expr);

  void error(const std::string &message);
  void runtimeError(const RuntimeError &error);

  void setVariable(const std::string &name, const Object &value);
  Object getVariable(const std::string &name);

private:
  bool m_hadError;
  bool m_hadRuntimeError;
  std::unordered_map<std::string, Object> m_globals;

  void registerFunctions();
};

} // namespace tpcpp
