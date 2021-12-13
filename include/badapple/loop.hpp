#ifndef _LOOP_HPP_
#define _LOOP_HPP_

#include <cstddef>
#include <initializer_list>
#include <utility>

namespace term {
  namespace details {
    template<class F, size_t... Is>
    void unroll_impl(F&& f, std::index_sequence<Is...>) {
      std::initializer_list<int> {(void(f(Is)), 0)...};
    }
  }
  template <size_t I, class F>
  void unroll(F&& f) {
    details::unroll_impl(std::forward<F>(f), std::make_index_sequence<I> {});
  }
}
#endif