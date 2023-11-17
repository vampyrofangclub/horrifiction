//Author: https://github.com/vampyrofangclub
//Documentation and examples of usages: https://github.com/vampyrofangclub/horrifiction
//License: The MIT License (https://opensource.org/license/mit/)
template <int T>
struct horrifiction_adl_tag {
  friend constexpr auto horrifiction_injected_func(horrifiction_adl_tag<T>);
};
template <int Key, typename Value>
struct horrifiction_inject_func {
  friend constexpr auto horrifiction_injected_func(horrifiction_adl_tag<Key>) { 
    return Value{}; 
  }
};
template <typename T, int N = 0>
constexpr int horrifiction_get_id_from_type_impl() {
  if constexpr (requires { horrifiction_injected_func(horrifiction_adl_tag<N>{}); }) {
    return horrifiction_get_id_from_type_impl<T, N + 1>();
  } else {
    (void)horrifiction_inject_func<N, T>{};
    return N;
  }
}
template <typename T>
constexpr int horrifiction_get_id_from_type() {
  return horrifiction_get_id_from_type_impl<T, 0>();
}
template <int N>
constexpr bool horrifiction_get_type_from_id_impl(int typeId, void* ptr, auto fn) {
  if constexpr (requires { horrifiction_injected_func(horrifiction_adl_tag<N>{}); }) {
    if (typeId == N) {
      using T = decltype(horrifiction_injected_func(horrifiction_adl_tag<N>{}));
      if constexpr(requires { static_cast<T>(ptr); }){
        fn(static_cast<T>(ptr));
      }
      return true;
    }
    return horrifiction_get_type_from_id_impl<N + 1>(typeId, ptr, fn);
  }
  return false;
}
constexpr bool horrifiction_get_type_from_id(int typeId, void* ptr, auto fn){
  return horrifiction_get_type_from_id_impl<0>(typeId, ptr, fn);
}
