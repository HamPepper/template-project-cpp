#include <format>
#include <functional>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "Exceptions.hpp"
#include "Object.hpp"

namespace tpcpp {

template <typename T, typename... Args>
concept AllOfTheSameType = (std::same_as<T, Args> && ...);

// extract function signature information from std::function
template <typename T> struct FunctionTraits;

template <typename R, typename... Args>
  requires AllOfTheSameType<R, Args...>
struct FunctionTraits<std::function<R(Args...)>> {
  using ReturnType = R;
  using ArgsTuple = std::tuple<Args...>;
  using FirstArgType = std::tuple_element_t<0, ArgsTuple>;

  static constexpr std::size_t arity = sizeof...(Args);
};

// convert vector to tuple
template <typename T, std::size_t... I>
auto vectorToTupleImpl(const std::vector<T> &vec, std::index_sequence<I...>) {
  return std::make_tuple(vec[I]...);
}

template <typename T, std::size_t N>
auto vectorToTuple(const std::vector<T> &vec) {
  return vectorToTupleImpl(vec, std::make_index_sequence<N>{});
}

// store functions of different arity
template <typename Func> struct Function final : public Callable {
  using Traits = FunctionTraits<Func>;
  using ReturnType = typename Traits::ReturnType;
  using ArgsTuple = typename Traits::ArgsTuple;
  using FirstArgType = typename Traits::FirstArgType;

  static constexpr std::size_t Arity = Traits::arity;

  Function(const std::string &name, Func func) : m_name(name), m_func(func) {};
  virtual ~Function() = default;

  virtual Object call(const ListOfObjects &arguments) override {
    (void)arguments;
    std::vector<FirstArgType> args{};
    for (auto a : arguments)
      args.push_back(toType<FirstArgType>(a));

    auto argsTuple = vectorToTuple<FirstArgType, Arity>(args);
    return std::apply(m_func, argsTuple);
  }

  virtual const std::string name() const override { return m_name; };

  virtual const std::string toString() const override {
    return std::format("<native fn '{}'>", m_name);
  }

  virtual std::size_t arity() const override { return Arity; };

private:
  std::string m_name;
  Func m_func;
};

} // namespace tpcpp
