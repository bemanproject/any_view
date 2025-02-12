// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_CONCEPTS_HPP
#define BEMAN_ANY_VIEW_CONCEPTS_HPP

#include <beman/any_view/detail/concepts.hpp>
#include <beman/any_view/detail/type_traits.hpp>

#include <ranges>

namespace beman::any_view {
namespace detail {

template <class IteratorT, class AnyIteratorT>
concept iterator_compatible_with =
    contiguous_iterator_compatible_with<IteratorT, iter_concept_t<AnyIteratorT>> and
    contiguous_reference_convertible_to<std::iter_reference_t<IteratorT>,
                                        std::iter_reference_t<AnyIteratorT>,
                                        iter_concept_t<AnyIteratorT>> and
    std::convertible_to<std::iter_rvalue_reference_t<IteratorT>, std::iter_rvalue_reference_t<AnyIteratorT>> and
    std::convertible_to<std::iter_difference_t<IteratorT>, std::iter_difference_t<AnyIteratorT>>;

template <class RangeT, class AnyViewT>
concept sized_range_compatible_with = not std::ranges::sized_range<AnyViewT> or std::ranges::sized_range<RangeT>;

template <class RangeT, class AnyViewT>
concept borrowed_range_compatible_with =
    not std::ranges::borrowed_range<AnyViewT> or std::ranges::borrowed_range<RangeT>;

template <class ViewT, class AnyViewT>
concept copyable_view_compatible_with = not std::copyable<AnyViewT> or std::copyable<ViewT>;

template <class ViewT, class AnyViewT>
concept view_compatible_with =
    std::ranges::view<ViewT> and
    iterator_compatible_with<std::ranges::iterator_t<ViewT>, std::ranges::iterator_t<AnyViewT>> and
    sized_range_compatible_with<ViewT, AnyViewT> and borrowed_range_compatible_with<ViewT, AnyViewT> and
    copyable_view_compatible_with<ViewT, AnyViewT>;

} // namespace detail

template <class RangeT, class AnyViewT>
concept ext_viewable_range_compatible_with =
    std::ranges::viewable_range<RangeT> and detail::view_compatible_with<std::views::all_t<RangeT>, AnyViewT>;

} // namespace beman::any_view

#endif // BEMAN_ANY_VIEW_CONCEPTS_HPP
