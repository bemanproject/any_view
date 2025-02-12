// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_TYPE_TRAITS_HPP
#define BEMAN_ANY_VIEW_DETAIL_TYPE_TRAITS_HPP

#include <beman/any_view/any_view_options.hpp>

#include <iterator>

namespace beman::any_view::detail {

template <class T>
struct as_rvalue : std::type_identity<T> {};

template <class T>
struct as_rvalue<T&> : std::type_identity<T&&> {};

template <class T>
using as_rvalue_t = as_rvalue<T>::type;

template <any_view_options OptionsV>
[[nodiscard]] consteval auto get_iter_concept() {
    using enum any_view_options;
    constexpr auto iterator_concept = OptionsV & contiguous;

    if constexpr (iterator_concept == contiguous) {
        return std::contiguous_iterator_tag{};
    } else if constexpr (iterator_concept == random_access) {
        return std::random_access_iterator_tag{};
    } else if constexpr (iterator_concept == bidirectional) {
        return std::bidirectional_iterator_tag{};
    } else if constexpr (iterator_concept == forward) {
        return std::forward_iterator_tag{};
    } else {
        return std::input_iterator_tag{};
    }
}

template <std::input_iterator IteratorT>
[[nodiscard]] consteval auto get_iter_concept() {
    if constexpr (std::contiguous_iterator<IteratorT>) {
        return std::contiguous_iterator_tag{};
    } else if constexpr (std::random_access_iterator<IteratorT>) {
        return std::random_access_iterator_tag{};
    } else if constexpr (std::bidirectional_iterator<IteratorT>) {
        return std::bidirectional_iterator_tag{};
    } else if constexpr (std::forward_iterator<IteratorT>) {
        return std::forward_iterator_tag{};
    } else {
        return std::input_iterator_tag{};
    }
}

template <class IteratorT>
using iter_concept_t = decltype(get_iter_concept<IteratorT>());

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_TYPE_TRAITS_HPP
