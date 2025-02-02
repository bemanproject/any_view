// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_CONCEPTS_HPP
#define BEMAN_ANY_VIEW_CONCEPTS_HPP

#include <beman/any_view/range_traits.hpp>
#include <iterator>

namespace beman::any_view {
namespace detail {

template <class IterT, class IterConceptT>
concept input_iterator_compatible_with =
    std::input_or_output_iterator<IterT> and
    (not std::derived_from<IterConceptT, std::input_iterator_tag> or std::input_iterator<IterT>);

template <class IterT, class IterConceptT>
concept forward_iterator_compatible_with =
    input_iterator_compatible_with<IterT, IterConceptT> and
    (not std::derived_from<IterConceptT, std::forward_iterator_tag> or std::forward_iterator<IterT>);

template <class IterT, class IterConceptT>
concept bidirectional_iterator_compatible_with =
    forward_iterator_compatible_with<IterT, IterConceptT> and
    (not std::derived_from<IterConceptT, std::bidirectional_iterator_tag> or std::bidirectional_iterator<IterT>);

template <class IterT, class IterConceptT>
concept random_access_iterator_compatible_with =
    bidirectional_iterator_compatible_with<IterT, IterConceptT> and
    (not std::derived_from<IterConceptT, std::random_access_iterator_tag> or std::random_access_iterator<IterT>);

template <class IterT, class IterConceptT>
concept contiguous_iterator_compatible_with =
    random_access_iterator_compatible_with<IterT, IterConceptT> and
    (not std::derived_from<IterConceptT, std::contiguous_iterator_tag> or std::contiguous_iterator<IterT>);

template <class RangeRefT, class TraitsRefT, class IterConceptT>
concept contiguous_reference_convertible_to =
    std::convertible_to<RangeRefT, TraitsRefT> and
    (not std::derived_from<IterConceptT, std::contiguous_iterator_tag> or
     std::convertible_to<std::remove_reference_t<RangeRefT> (*)[], std::remove_reference_t<TraitsRefT> (*)[]>);

template <class RangeT, class RangeTraitsT>
concept sized_range_compatible_with = not RangeTraitsT::sized or std::ranges::sized_range<RangeT>;

template <class RangeT, class RangeTraitsT>
concept borrowed_range_compatible_with = not RangeTraitsT::borrowed or std::ranges::borrowed_range<RangeT>;

template <class RangeT, class RangeTraitsT>
concept range_compatible_with =
    std::ranges::range<RangeT> and
    contiguous_iterator_compatible_with<std::ranges::iterator_t<RangeT>, iterator_concept_t<RangeTraitsT>> and
    contiguous_reference_convertible_to<std::ranges::range_reference_t<RangeT>,
                                        reference_type_t<RangeTraitsT>,
                                        iterator_concept_t<RangeTraitsT>> and
    std::convertible_to<std::ranges::range_rvalue_reference_t<RangeT>, rvalue_reference_type_t<RangeTraitsT>> and
    std::convertible_to<std::ranges::range_difference_t<RangeT>, difference_type_t<RangeTraitsT>> and
    sized_range_compatible_with<RangeT, RangeTraitsT> and borrowed_range_compatible_with<RangeT, RangeTraitsT>;

template <class ViewT, class RangeTraitsT>
concept copyable_view_compatible_with =
#if BEMAN_ANY_VIEW_USE_COPYABLE()
    not
#endif
    RangeTraitsT::BEMAN_ANY_VIEW_OPTION() or
    std::copyable<ViewT>;

template <bool SimpleV, class ViewT>
using const_if_t = std::conditional_t<SimpleV, const ViewT, ViewT>;

template <class ViewT, class RangeTraitsT>
concept view_compatible_with = copyable_view_compatible_with<ViewT, RangeTraitsT> and
                               range_compatible_with<const_if_t<RangeTraitsT::simple, ViewT>, RangeTraitsT>;

} // namespace detail

template <class RangeT, class RangeTraitsT>
concept ext_viewable_range_compatible_with =
    std::ranges::viewable_range<RangeT> and detail::view_compatible_with<std::views::all_t<RangeT>, RangeTraitsT>;

} // namespace beman::any_view

#endif // BEMAN_ANY_VIEW_CONCEPTS_HPP
