namespace tpcpp {

/// @brief Helper class to collect call operators from all lambdas.
///
/// Use this class with `std::visit`, in the following way:
///
/// ```
/// auto v1 = [](bool) -> int { return 1; };
/// auto v2 = [](double) -> int { return 1; };
///
/// std::visit(OpCollector{v1, v2}, variant);
/// ```
///
/// @note
/// `OpCollector` also works with n-ary operators. For example:
/// ```
/// auto v = [](bool, double) -> int { return 1; };
/// std::visit(OpCollector{v}, variant);
/// ```
template <class... Ts> struct OpCollector : Ts... {
  using Ts::operator()...;
};

// to make ccls happy
template <class... Ts> OpCollector(Ts...) -> OpCollector<Ts...>;

} // namespace tpcpp
