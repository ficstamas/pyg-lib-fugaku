#pragma once

#include "types.h"

#include <type_traits>

namespace pyg {

namespace utils {

// Base class for easier type check
struct HeteroDispatchMode {};

// List hetero dispatch mode as different types to avoid non-type template
// specialization.
struct SkipMode : public HeteroDispatchMode {};

struct NodeSrcMode : public HeteroDispatchMode {};

struct NodeDstMode : public HeteroDispatchMode {};

struct EdgeMode : public HeteroDispatchMode {};

template <typename... T>
struct is_c10_dict : std::false_type {};

template <typename T, typename N>
struct is_c10_dict<c10::Dict<T, N>> : std::true_type {};

// TODO: Should specialize as if-constexpr when in C++17
template <typename T, typename V, typename MODE>
class HeteroDispatchArg {};

// In SkipMode we do not filter this arg
template <typename T, typename V>
class HeteroDispatchArg<T, V, SkipMode> {
 public:
  HeteroDispatchArg(const T& val) : val_(val) {}

  // If we pass the filter, we will obtain the value of the argument.
  template <typename K>
  V value_by_edge(const K& key) {
    return val_;
  }

  bool filter_by_edge(const edge_t& edge) { return true; }

 private:
  T val_;
};

// In NodeSrcMode we check if source node is in the dict
template <typename T, typename V>
class HeteroDispatchArg<T, V, NodeSrcMode> {
 public:
  HeteroDispatchArg(const T& val) : val_(val) {
    static_assert(is_c10_dict<T>::value, "Should be a c10::dict");
  }

  template <typename K>
  V value_by_edge(const K& key) {
    return val_.at(get_src(key));
  }

  bool filter_by_edge(const edge_t& edge) {
    return val_.contains(get_src(edge));
  }

 private:
  T val_;
};

// In NodeDstMode we check if destination node is in the dict
template <typename T, typename V>
class HeteroDispatchArg<T, V, NodeDstMode> {
 public:
  HeteroDispatchArg(const T& val) : val_(val) {
    static_assert(is_c10_dict<T>::value, "Should be a c10::dict");
  }

  template <typename K>
  V value_by_edge(const K& key) {
    return val_.at(get_dst(key));
  }

  bool filter_by_edge(const edge_t& edge) {
    return val_.contains(get_dst(edge));
  }

 private:
  T val_;
};

// In EdgeMode we check if edge is in the dict
template <typename T, typename V>
class HeteroDispatchArg<T, V, EdgeMode> {
 public:
  HeteroDispatchArg(const T& val) : val_(val) {
    static_assert(is_c10_dict<T>::value, "Should be a c10::dict");
  }

  template <typename K>
  V value_by_edge(const K& key) {
    return val_.at(key);
  }

  bool filter_by_edge(const edge_t& edge) { return val_.contains(edge); }

 private:
  T val_;
};

// The following will help static type checks:
template <typename... T>
struct is_hetero_arg : std::false_type {};

// Just check inheritance, a workaround without introducing concepts
template <typename T, typename V, typename Mode>
struct is_hetero_arg<HeteroDispatchArg<T, V, Mode>> : std::true_type {
  static_assert(std::is_base_of<HeteroDispatchMode, Mode>::value,
                "Must pass a mode for dispatching");
};

// Specialize
template <typename... Args>
bool filter_args_by_edge(const edge_t& edge, Args&&... args) {}

template <>
bool filter_args_by_edge(const edge_t& edge) {
  return true;
}

template <typename T, typename... Args>
bool filter_args_by_edge(const edge_t& edge, T&& t, Args&&... args) {
  static_assert(
      is_hetero_arg<std::remove_const_t<std::remove_reference_t<T>>>::value,
      "args should be HeteroDispatchArg");
  return t.filter_by_edge(edge) && filter_args_by_edge(edge, args...);
}

template <typename... T>
struct is_std_function : std::false_type {};

template <typename T, typename... Args>
struct is_std_function<std::function<T(Args...)>> : std::true_type {};

template <typename T>
class HeteroDispatchOp {
 public:
  using result_type = typename T::result_type;
  HeteroDispatchOp(const edge_tensor_dict_t& rowptr,
                   const edge_tensor_dict_t& col,
                   T op)
      : rowptr_(rowptr), col_(col), op_(op) {
    // Check early
    static_assert(is_std_function<T>::value, "Must pass a function");
  }

  template <typename... Args>
  c10::Dict<edge_t, result_type> operator()(Args&&... args) {
    c10::Dict<edge_t, result_type> dict;
    for (const auto& kv : rowptr_) {
      auto edge = kv.key();
      auto rowptr = kv.value();
      auto col = col_.at(edge);
      bool pass = filter_args_by_edge(edge, args...);
      if (pass) {
        result_type res = op_(rowptr, col, args.value_by_edge(edge)...);
        dict.insert(edge, res);
      }
    }
    return dict;
  }

 private:
  edge_tensor_dict_t rowptr_;
  edge_tensor_dict_t col_;
  T op_;
};

}  // namespace utils

}  // namespace pyg