#pragma once

#include <memory>

namespace tpcpp {

/// @brief Boxing for all Expr classes.
///
/// The Box class provides a value-like semantics.
///
/// @note
/// Stolen from: https://www.foonathan.net/2022/05/recursive-variant-box/
template <typename T> class Box {
public:
  /// Automatic construction from a `T`, not a `T*`
  Box(T &&obj) : m_impl(new T(std::move(obj))) {};
  Box(const T &obj) : m_impl(new T(obj)) {};
  ~Box() = default;

  /// Copy constructor copies `T`
  Box(const Box &other) : Box(*other.m_impl) {}
  Box &operator=(const Box &other) {
    *m_impl = *other.m_impl;
    return *this;
  }

  /// Access propagates constness
  T &operator*() { return *m_impl; }
  const T &operator*() const { return *m_impl; }

  T *operator->() { return m_impl.get(); }
  const T *operator->() const { return m_impl.get(); }

private:
  std::unique_ptr<T> m_impl;
};

}; // namespace tpcpp
