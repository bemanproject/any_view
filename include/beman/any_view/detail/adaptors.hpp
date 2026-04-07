// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_ADAPTORS_HPP
#define BEMAN_ANY_VIEW_DETAIL_ADAPTORS_HPP

#include <beman/any_view/any_view_options.hpp>
#include <beman/any_view/detail/no_unique_address.hpp>

#include <concepts>
#include <iterator>
#include <ranges>

namespace beman::any_view::detail {

struct adaptor_base {};

template <class T>
concept adaptor = std::derived_from<T, adaptor_base>;

template <class T>
concept not_adaptor = not adaptor<T>;

template <std::input_or_output_iterator IteratorT, std::sentinel_for<IteratorT> SentinelT>
struct iterator_adaptor : adaptor_base {
    BEMAN_ANY_VIEW_NO_UNIQUE_ADDRESS IteratorT iterator;
    BEMAN_ANY_VIEW_NO_UNIQUE_ADDRESS SentinelT sentinel;
};

template <std::ranges::view ViewT>
using iterator_adaptor_for = iterator_adaptor<std::ranges::iterator_t<ViewT>, std::ranges::sentinel_t<ViewT>>;

template <std::ranges::view ViewT, any_view_options OptsV>
struct view_adaptor : adaptor_base {
    using adaptor_type = iterator_adaptor_for<ViewT>;

    static constexpr any_view_options options = OptsV;

    BEMAN_ANY_VIEW_NO_UNIQUE_ADDRESS ViewT view;
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_ADAPTORS_HPP
