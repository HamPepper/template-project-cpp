#pragma once

#include <exception>
#include <string>

namespace tpcpp {

struct RuntimeError : public std::exception {
  RuntimeError(const std::string &message) : message(message) {};
  std::string message;
};

} // namespace tpcpp
