// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_TYPE_TRAITS_HPP
#define BEMAN_ANY_VIEW_DETAIL_TYPE_TRAITS_HPP

#include <beman/any_view/any_view_options.hpp>
#include <beman/any_view/detail/concepts.hpp>

#include <iterator>

namespace beman::any_view::detail {

template <class T>
struct rvalue_ref {
    using type = T;
};

template <class T>
struct rvalue_ref<T&> {
    using type = T&&;
};

template <class T>
using rvalue_ref_t = typename rvalue_ref<T>::type;

template <any_view_options OptsV>
[[nodiscard]] consteval auto get_iterator_concept() {
    using enum any_view_options;

    if constexpr (flag_is_set<OptsV, contiguous>) {
        return std::contiguous_iterator_tag{};
    } else if constexpr (flag_is_set<OptsV, random_access>) {
        return std::random_access_iterator_tag{};
    } else if constexpr (flag_is_set<OptsV, bidirectional>) {
        return std::bidirectional_iterator_tag{};
    } else if constexpr (flag_is_set<OptsV, forward>) {
        return std::forward_iterator_tag{};
    } else if constexpr (flag_is_set<OptsV, input>) {
        return std::input_iterator_tag{};
    }
}

template <any_view_options OptsV>
using iterator_concept_t = decltype(get_iterator_concept<OptsV>());

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_TYPE_TRAITS_HPP
