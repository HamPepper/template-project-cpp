#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace tpcpp {

// Object //////////////////////////////////////////////////////////////////////
struct Callable;
using CallablePtr = std::shared_ptr<Callable>;

using Object = std::variant<bool, long, double, CallablePtr>;
using ListOfObjects = std::vector<Object>;

// Callable ////////////////////////////////////////////////////////////////////
struct Callable {
  ~Callable() = default;

  virtual Object call(const ListOfObjects &arguments) = 0;
  virtual const std::string toString() const = 0;
  virtual size_t arity() const = 0;
};

// Object: predicates //////////////////////////////////////////////////////////
bool isInt(const Object &obj);
bool isNumber(const Object &obj);
bool isCallable(const Object &obj);

// Object: type conversion /////////////////////////////////////////////////////
bool toBool(const Object &obj);
long toLong(const Object &obj);
double toDouble(const Object &obj);
CallablePtr toCallable(const Object &obj);

std::string toString(const Object &obj);

// Object: operators ///////////////////////////////////////////////////////////

// unary operators
bool operator!(const Object &rhs);
Object operator-(const Object &rhs);

// binary operators, comparisons
bool operator==(const Object &lhs, const Object &rhs);
bool operator!=(const Object &lhs, const Object &rhs);

bool operator>(const Object &lhs, const Object &rhs);
bool operator>=(const Object &lhs, const Object &rhs);

bool operator<(const Object &lhs, const Object &rhs);
bool operator<=(const Object &lhs, const Object &rhs);

// binary operators, arithmetics
Object operator+(const Object &lhs, const Object &rhs);
Object operator-(const Object &lhs, const Object &rhs);
Object operator*(const Object &lhs, const Object &rhs);
Object operator/(const Object &lhs, const Object &rhs);

Object power(const Object &lhs, const Object &rhs);

} // namespace tpcpp
