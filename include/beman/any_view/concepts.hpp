// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_CONCEPTS_HPP
#define BEMAN_ANY_VIEW_CONCEPTS_HPP

#include "detail/concepts.hpp"
#include "detail/type_traits.hpp"

#include <ranges>

namespace beman::any_view {
namespace detail {

template <class IteratorT, class AnyIteratorT>
concept any_compatible_iterator =
    any_compatible_contiguous_iterator<IteratorT, iter_concept_t<AnyIteratorT>> and
    any_compatible_contiguous_reference<std::iter_reference_t<IteratorT>,
                                        std::iter_reference_t<AnyIteratorT>,
                                        iter_concept_t<AnyIteratorT>> and
    std::convertible_to<std::iter_rvalue_reference_t<IteratorT>, std::iter_rvalue_reference_t<AnyIteratorT>> and
    std::convertible_to<std::iter_difference_t<IteratorT>, std::iter_difference_t<AnyIteratorT>>;

template <class RangeT, class AnyViewT>
concept any_compatible_sized_range = not std::ranges::sized_range<AnyViewT> or std::ranges::sized_range<RangeT>;

template <class RangeT, class AnyViewT>
concept any_compatible_borrowed_range =
    not std::ranges::borrowed_range<AnyViewT> or std::ranges::borrowed_range<RangeT>;

template <class ViewT, class AnyViewT>
concept any_compatible_copyable_view = not std::copyable<AnyViewT> or std::copyable<ViewT>;

template <class ViewT, class AnyViewT>
concept any_compatible_view =
    std::ranges::view<ViewT> and
    any_compatible_iterator<std::ranges::iterator_t<ViewT>, std::ranges::iterator_t<AnyViewT>> and
    any_compatible_sized_range<ViewT, AnyViewT> and any_compatible_borrowed_range<ViewT, AnyViewT> and
    any_compatible_copyable_view<ViewT, AnyViewT>;

} // namespace detail

template <class RangeT, class AnyViewT>
concept ext_any_compatible_viewable_range =
    std::ranges::viewable_range<RangeT> and detail::any_compatible_view<std::views::all_t<RangeT>, AnyViewT>;

} // namespace beman::any_view

#endif // BEMAN_ANY_VIEW_CONCEPTS_HPP
