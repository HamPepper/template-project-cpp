#include <cmath>
#include <limits>

#include "Object.hpp"

#include "Exceptions.hpp"
#include "visitors/helpers.hpp"

#pragma warning(disable : 4702)

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
  auto hdlNumber = []<typename T>(T o) -> long
    requires(std::is_arithmetic_v<T>)
  { return static_cast<long>(o); };
  auto hdlCallable = [](CallablePtr o) -> long {
    return static_cast<long>(reinterpret_cast<uintptr_t>(o.get()));
  };
  return std::visit(OpCollector{hdlNumber, hdlCallable}, obj);
}

double toDouble(const Object &obj) {
  auto hdlNumber = [](auto o) -> double { return o; };
  auto hdlCallable = [](CallablePtr o) -> double {
    return static_cast<double>(reinterpret_cast<uintptr_t>(o.get()));
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
  auto hdlBool = [](bool o) -> Object { return -static_cast<long>(o); };
  auto hdlNumber = [](auto o) -> Object { return -o; };
  auto hdlCallable = [](CallablePtr) -> Object {
    throw RuntimeError{"Unary operator '-' cannot be applied to a callable."};
  };
  return std::visit(OpCollector{hdlBool, hdlNumber, hdlCallable}, rhs);
}

// binary operators, comparisons
bool operator==(const Object &lhs, const Object &rhs) {
  auto hdlGen = []<typename T1, typename T2>(T1 l, T2 r) -> bool {
    if constexpr (std::is_same_v<T1, CallablePtr> ||
                  std::is_same_v<T2, CallablePtr>)
      return false;
#pragma warning(push)
#pragma warning(disable : 4805)
    return l == r;
#pragma warning(pop)
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

// binary operators, arithmetics
Object operator+(const Object &lhs, const Object &rhs) {
  auto hdlGen = []<typename T1, typename T2>(T1 l, T2 r) -> Object {
    if constexpr (std::is_same_v<T1, CallablePtr> ||
                  std::is_same_v<T2, CallablePtr>)
      throw RuntimeError{
          "Binary operator '+' cannot be applied to a callable."};
    return l + r;
  };
  return std::visit(hdlGen, lhs, rhs);
}

Object operator-(const Object &lhs, const Object &rhs) {
  auto hdlGen = []<typename T1, typename T2>(T1 l, T2 r) -> Object {
    if constexpr (std::is_same_v<T1, CallablePtr> ||
                  std::is_same_v<T2, CallablePtr>)
      throw RuntimeError{
          "Binary operator '-' cannot be applied to a callable."};
    return l - r;
  };
  return std::visit(hdlGen, lhs, rhs);
}

Object operator*(const Object &lhs, const Object &rhs) {
  auto hdlGen = []<typename T1, typename T2>(T1 l, T2 r) -> Object {
    if constexpr (std::is_same_v<T1, CallablePtr> ||
                  std::is_same_v<T2, CallablePtr>)
      throw RuntimeError{
          "Binary operator '*' cannot be applied to a callable."};
    return l * r;
  };
  return std::visit(hdlGen, lhs, rhs);
}

Object operator/(const Object &lhs, const Object &rhs) {
  auto hdlNumber = []<typename T1, typename T2>(T1 l, T2 r) -> Object
    requires(std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>)
  {
    if (r == 0)
      return std::numeric_limits<double>::quiet_NaN();
#pragma warning(push)
#pragma warning(disable : 4804)
    return l / r;
#pragma warning(pop)
  };

  auto hdlGen = [](auto, auto) -> Object {
    throw RuntimeError{"Binary operator '/' cannot be applied to a callable."};
  };
  return std::visit(OpCollector{hdlNumber, hdlGen}, lhs, rhs);
}

// special treatment for power: we want to get back an integral number when both
// inputs are integral
static long powerInt(long x, long p) {
  if (p == 0)
    return 1;
  if (p == 1)
    return x;

  long tmp = powerInt(x, p / 2);
  if (p % 2 == 0)
    return tmp * tmp;
  return x * tmp * tmp;
};

Object power(const Object &lhs, const Object &rhs) {
  auto hdlInt = []<typename T1, typename T2>(T1 l, T2 r) -> Object
    requires(std::is_integral_v<T1> && std::is_integral_v<T2>)
  { return powerInt(l, r); };

  auto hdlNumber = []<typename T1, typename T2>(T1 l, T2 r) -> Object
    requires(std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2> &&
             (!std::is_integral_v<T1> || !std::is_integral_v<T2>))
  { return std::pow(l, r); };

  auto hdlGen = [](auto, auto) -> Object {
    throw RuntimeError{"'power' function cannot be applied to a callable."};
  };
  return std::visit(OpCollector{hdlInt, hdlNumber, hdlGen}, lhs, rhs);
}

} // namespace tpcpp
