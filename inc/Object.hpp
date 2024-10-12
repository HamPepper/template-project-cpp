#pragma once

#include <memory>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

/// @namespace tpcpp
/// @brief All tpcpp classes are placed in this namespace.
namespace tpcpp {

// Object //////////////////////////////////////////////////////////////////////

struct Callable;

/// @typedef tpcpp::CallablePtr
/// @brief A `std::shared_ptr` type for `tpcpp::Callable`.
using CallablePtr = std::shared_ptr<Callable>;

/// @typedef tpcpp::Object
/// @brief A `std::variant` type used in expression evaluation.
///
/// It may contain one of the following types:
/// - `bool`
/// - `long`
/// - `double`
/// - `tpcpp::CallablePtr`
///
/// @note
/// It is the only type used in expression evaluation.
using Object = std::variant<bool, long, double, CallablePtr>;

using ListOfObjects = std::vector<Object>;

// Callable ////////////////////////////////////////////////////////////////////

/// @brief An interface for callable objects.
///
/// @warning
/// This is a pure virtual class.
struct Callable {
  virtual ~Callable() = default;

  /// @brief Call the function with the given arguments.
  virtual Object call(const ListOfObjects &arguments) = 0;

  /// @brief Return the name the function in `std::string`.
  virtual const std::string name() const = 0;

  /// @brief Return a description of the function in `std::string`.
  virtual const std::string toString() const = 0;

  /// @brief Return the arity of the function.
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

// type conversion with type a template parameter
template <typename T>
concept ObjectToType =
    std::same_as<T, bool> || std::same_as<T, long> || std::same_as<T, double> ||
    std::same_as<T, CallablePtr> || std::same_as<T, std::string>;

template <ObjectToType T> constexpr auto toType(Object &obj) {
  if constexpr (std::same_as<T, bool>)
    return toBool(obj);
  else if constexpr (std::same_as<T, long>)
    return toLong(obj);
  else if constexpr (std::same_as<T, double>)
    return toDouble(obj);
  else if constexpr (std::same_as<T, CallablePtr>)
    return toCallable(obj);
  else
    return toString(obj);
}

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
