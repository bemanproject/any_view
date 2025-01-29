// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_TYPE_TRAITS_HPP
#define BEMAN_ANY_VIEW_DETAIL_TYPE_TRAITS_HPP

#include <type_traits>

namespace beman::any_view::detail {

template <class T>
struct as_rvalue : std::type_identity<T> {};

template <class T>
struct as_rvalue<T&> : std::type_identity<T&&> {};

template <class T>
using as_rvalue_t = as_rvalue<T>::type;

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_TYPE_TRAITS_HPP
