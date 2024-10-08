#include "Object.hpp"

#include "Exceptions.hpp"
#include "visitors/helpers.hpp"

namespace tpcpp {

// Object: predicates //////////////////////////////////////////////////////////
bool isInt(const Object &obj) {
  auto hdlInt = [](auto) -> bool { return true; };
  auto hdlDouble = [](double) -> bool { return false; };
  auto hdlCallable = [](CallablePtr) -> bool { return false; };
  return std::visit(OpCollector{hdlInt, hdlDouble, hdlCallable}, obj);
}

bool isNumber(const Object &obj) {
  auto hdlNumber = [](auto) -> bool { return true; };
  auto hdlCallable = [](CallablePtr) -> bool { return false; };
  return std::visit(OpCollector{hdlNumber, hdlCallable}, obj);
}

bool isCallable(const Object &obj) { return !isNumber(obj); }

// Object: type conversion /////////////////////////////////////////////////////
bool toBool(const Object &obj) {
  auto hdlNumber = [](auto o) -> bool { return o != 0; };
  auto hdlCallable = [](CallablePtr) -> bool { return true; };
  return std::visit(OpCollector{hdlNumber, hdlCallable}, obj);
}

long toLong(const Object &obj) {
  auto hdlNumber = [](auto o) -> long { return o; };
  auto hdlCallable = [](CallablePtr o) -> long {
    return reinterpret_cast<long>(o.get());
  };
  return std::visit(OpCollector{hdlNumber, hdlCallable}, obj);
}

double toDouble(const Object &obj) {
  auto hdlNumber = [](auto o) -> double { return o; };
  auto hdlCallable = [](CallablePtr o) -> double {
    return static_cast<double>(reinterpret_cast<long>(o.get()));
  };
  return std::visit(OpCollector{hdlNumber, hdlCallable}, obj);
}

CallablePtr toCallable(const Object &obj) {
  if (!isCallable(obj))
    throw RuntimeError{"Object is not callable."};
  return std::get<CallablePtr>(obj);
}

std::string toString(const Object &obj) {
  auto hdlBool = [](bool o) -> std::string { return o ? "true" : "false"; };
  auto hdlNumber = [](auto o) -> std::string { return std::to_string(o); };
  auto hdlCallable = [](CallablePtr o) -> std::string { return o->toString(); };
  return std::visit(OpCollector{hdlBool, hdlNumber, hdlCallable}, obj);
}

// Object: operators ///////////////////////////////////////////////////////////

// unary operators
bool operator!(const Object &rhs) { return !toBool(rhs); }

Object operator-(const Object &rhs) {
  auto hdlNumber = [](auto o) -> Object { return -o; };
  auto hdlCallable = [](CallablePtr) -> Object {
    throw RuntimeError{"Unary operator '-' cannot be applied to a callable."};
  };
  return std::visit(OpCollector{hdlNumber, hdlCallable}, rhs);
}

// binary operators, comparisons
bool operator==(const Object &lhs, const Object &rhs) {
  auto hdlGen = []<typename T1, typename T2>(T1 l, T2 r) -> bool {
    if constexpr (std::is_same_v<T1, CallablePtr> ||
                  std::is_same_v<T2, CallablePtr>)
      return false;
    else
      return l == r;
  };
  auto hdlCC = [](CallablePtr l, CallablePtr r) -> bool {
    return l.get() == r.get();
  };
  return std::visit(OpCollector{hdlGen, hdlCC}, lhs, rhs);
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
