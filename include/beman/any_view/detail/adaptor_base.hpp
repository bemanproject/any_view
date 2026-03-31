// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_ADAPTOR_BASE_HPP
#define BEMAN_ANY_VIEW_DETAIL_ADAPTOR_BASE_HPP

#include <concepts>

namespace beman::any_view::detail {

struct adaptor_base {};

template <class T>
concept adaptor = std::derived_from<T, adaptor_base>;

template <class T>
concept not_adaptor = not adaptor<T>;

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_ADAPTOR_BASE_HPP
