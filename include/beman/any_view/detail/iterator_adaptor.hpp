// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_ITERATOR_ADAPTOR_HPP
#define BEMAN_ANY_VIEW_DETAIL_ITERATOR_ADAPTOR_HPP

#include <beman/any_view/detail/adaptor_base.hpp>
#include <beman/any_view/detail/no_unique_address.hpp>

#include <iterator>

namespace beman::any_view::detail {

template <std::input_or_output_iterator IteratorT, std::sentinel_for<IteratorT> SentinelT>
struct iterator_adaptor : adaptor_base {
    BEMAN_ANY_VIEW_NO_UNIQUE_ADDRESS IteratorT iterator;
    BEMAN_ANY_VIEW_NO_UNIQUE_ADDRESS SentinelT sentinel;
};

template <std::ranges::view ViewT>
using iterator_adaptor_for = iterator_adaptor<std::ranges::iterator_t<ViewT>, std::ranges::sentinel_t<ViewT>>;

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_ITERATOR_ADAPTOR_HPP
