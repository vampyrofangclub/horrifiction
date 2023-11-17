template <int T>
struct __adl_tag {
  friend constexpr auto __injected_func(__adl_tag<T>);
};
template <int Key, typename Value>
struct __inject_func {
  friend constexpr auto __injected_func(__adl_tag<Key>) { 
    return Value{}; 
  }
};
template <typename T, int N = 0>
constexpr int __get_id_from_type_impl() {
  if constexpr (requires { __injected_func(__adl_tag<N>{}); }) {
    return __get_id_from_type_impl<T, N + 1>();
  } else {
    (void)__inject_func<N, T>{};
    return N;
  }
}
template <typename T>
constexpr int __get_id_from_type() {
  return __get_id_from_type_impl<T, 0>();
}
template <int N>
constexpr bool __get_type_from_id_impl(int typeId, void* ptr, auto fn) {
  if constexpr (requires { __injected_func(__adl_tag<N>{}); }) {
    if (typeId == N) {
      using T = decltype(__injected_func(__adl_tag<N>{}));
      if constexpr(requires { static_cast<T>(ptr); }){
        fn(static_cast<T>(ptr));
      }
      return true;
    }
    return __get_type_from_id_impl<N + 1>(typeId, ptr, fn);
  }
  return false;
}
constexpr bool __get_type_from_id(int typeId, void* ptr, auto fn){
  return __get_type_from_id_impl<0>(typeId, ptr, fn);
}
