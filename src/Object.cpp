#include "Object.hpp"

#include "Exceptions.hpp"

namespace tpcpp {

// helper to collect call operators from all lambdas
template <class... Ts> struct overloadUnary : Ts... {
  using Ts::operator()...;
};

// c++17 CTAD
template <class... Ts> overloadUnary(Ts...) -> overloadUnary<Ts...>;

// Object: predicates //////////////////////////////////////////////////////////
bool isNumber(const Object &obj) { return obj.index() < 3; }

bool isCallable(const Object &obj) { return obj.index() == 3; }

// Object: type conversion /////////////////////////////////////////////////////
bool toBool(const Object &obj) {
  auto handlerBool = [](bool o) -> bool { return o; };
  auto handlerLong = [](long o) -> bool { return o != 0; };
  auto handlerDouble = [](double o) -> bool { return o != 0; };
  auto handlerCallable = [](CallablePtr o) -> bool {
    (void)o;
    return true;
  };

  return std::visit(
      overloadUnary{handlerBool, handlerLong, handlerDouble, handlerCallable},
      obj);
}

long toLong(const Object &obj) {
  switch (obj.index()) {
  case 0:
    return static_cast<long>(std::get<bool>(obj));
  case 1:
    return std::get<long>(obj);
  case 2:
    return static_cast<long>(std::get<double>(obj));
  default:
    // ptr address -> long
    return reinterpret_cast<long>(toCallable(obj).get());
  }
}

double toDouble(const Object &obj) {
  switch (obj.index()) {
  case 0:
    return static_cast<double>(std::get<bool>(obj));
  case 1:
    return static_cast<double>(std::get<long>(obj));
  case 2:
    return std::get<double>(obj);
  default:
    return static_cast<double>(reinterpret_cast<long>(toCallable(obj).get()));
  }
}

CallablePtr toCallable(const Object &obj) {
  if (!isCallable(obj))
    throw RuntimeError("Object is not callable.");
  return std::get<CallablePtr>(obj);
}

std::string toString(const Object &obj) {
  switch (obj.index()) {
  case 0:
    return std::get<bool>(obj) ? "true" : "false";
  case 1:
    return std::to_string(std::get<long>(obj));
  case 2:
    return std::to_string(std::get<double>(obj));
  default:
    return toCallable(obj)->toString();
  }
}

// Object: operators ///////////////////////////////////////////////////////////

// unary operators
bool operator!(const Object &rhs) { return !toBool(rhs); }

Object operator-(const Object &rhs) {
  switch (rhs.index()) {
  case 0:
    return !std::get<bool>(rhs);
  case 1:
    return -std::get<long>(rhs);
  case 2:
    return -std::get<double>(rhs);
  default:
    throw RuntimeError("Unary operator '-' cannot be applied to a callable.");
  }
}

// binary operators, comparisons
bool operator==(const Object &lhs, const Object &rhs) {
  if ((isCallable(lhs) && isNumber(rhs)) || (isNumber(lhs) && isCallable(rhs)))
    return false;

  if (lhs.index() != rhs.index())
    return false;
  switch (lhs.index()) {
  case 0:
    return std::get<bool>(lhs) == std::get<bool>(rhs);
  case 1:
    return std::get<long>(lhs) == std::get<long>(rhs);
  case 2:
    return std::get<double>(lhs) == std::get<double>(rhs);
  default:
    return toCallable(lhs).get() == toCallable(rhs).get();
  }
}

bool operator!=(const Object &lhs, const Object &rhs) { return !(lhs == rhs); }

bool operator>(const Object &lhs, const Object &rhs) {
  return toDouble(lhs) > toDouble(rhs);
}

bool operator>=(const Object &lhs, const Object &rhs) {
  return lhs > rhs || lhs == rhs;
}

bool operator<(const Object &lhs, const Object &rhs) {
  return toDouble(lhs) < toDouble(rhs);
}

bool operator<=(const Object &lhs, const Object &rhs) {
  return lhs < rhs || lhs == rhs;
}

} // namespace tpcpp
