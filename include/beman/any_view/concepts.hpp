// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_CONCEPTS_HPP
#define BEMAN_ANY_VIEW_CONCEPTS_HPP

#include <beman/any_view/detail/concepts.hpp>
#include <beman/any_view/range_traits.hpp>

namespace beman::any_view {
namespace detail {

template <class IteratorT, class RangeTraitsT>
concept iterator_compatible_with =
    contiguous_iterator_compatible_with<IteratorT, iterator_concept_t<RangeTraitsT>> and
    contiguous_reference_convertible_to<std::iter_reference_t<IteratorT>,
                                        reference_type_t<RangeTraitsT>,
                                        iterator_concept_t<RangeTraitsT>> and
    std::convertible_to<std::iter_rvalue_reference_t<IteratorT>, rvalue_reference_type_t<RangeTraitsT>> and
    std::convertible_to<std::iter_difference_t<IteratorT>, difference_type_t<RangeTraitsT>>;

template <class RangeT, class RangeTraitsT>
concept sized_range_compatible_with = not RangeTraitsT::sized or std::ranges::sized_range<RangeT>;

template <class RangeT, class RangeTraitsT>
concept borrowed_range_compatible_with = not RangeTraitsT::borrowed or std::ranges::borrowed_range<RangeT>;

template <class ViewT, class RangeTraitsT>
concept copyable_view_compatible_with =
#if BEMAN_ANY_VIEW_USE_COPYABLE()
    not
#endif
    RangeTraitsT::BEMAN_ANY_VIEW_OPTION() or
    std::copyable<ViewT>;

template <class ViewT, class RangeTraitsT>
concept view_compatible_with =
    std::ranges::view<ViewT> and iterator_compatible_with<std::ranges::iterator_t<ViewT>, RangeTraitsT> and
    sized_range_compatible_with<ViewT, RangeTraitsT> and borrowed_range_compatible_with<ViewT, RangeTraitsT> and
    copyable_view_compatible_with<ViewT, RangeTraitsT>;

} // namespace detail

template <class RangeT, class RangeTraitsT>
concept ext_viewable_range_compatible_with =
    std::ranges::viewable_range<RangeT> and detail::view_compatible_with<std::views::all_t<RangeT>, RangeTraitsT>;

} // namespace beman::any_view

#endif // BEMAN_ANY_VIEW_CONCEPTS_HPP
