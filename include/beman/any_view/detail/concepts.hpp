// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_CONCEPTS_HPP
#define BEMAN_ANY_VIEW_DETAIL_CONCEPTS_HPP

#include <beman/any_view/config.hpp>

#include <ranges>

namespace beman::any_view::detail {

template <class RangeT>
concept simple_range = std::ranges::range<RangeT> and std::ranges::range<const RangeT> and
                       std::same_as<std::ranges::iterator_t<RangeT>, std::ranges::iterator_t<const RangeT>> and
                       std::same_as<std::ranges::sentinel_t<RangeT>, std::ranges::sentinel_t<const RangeT>>;

template <class ViewT>
concept BEMAN_ANY_VIEW_OPTION_(range) =
#if BEMAN_ANY_VIEW_USE_MOVE_ONLY()
    not
#endif
    std::copyable<ViewT> and
    std::ranges::range<ViewT>;

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_CONCEPTS_HPP
