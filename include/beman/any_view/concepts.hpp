// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_CONCEPTS_HPP
#define BEMAN_ANY_VIEW_CONCEPTS_HPP

#include <beman/any_view/detail/concepts.hpp>
#include <beman/any_view/reserve_hint.hpp>

namespace beman::any_view {
namespace detail {

template <class IteratorT, class RefT, class RValueRefT, class DiffT, any_view_options OptsV>
concept any_compatible_iterator =
    any_compatible_contiguous_iterator<IteratorT, OptsV> and
    any_compatible_contiguous_reference<std::iter_reference_t<IteratorT>, RefT, OptsV> and
    convertible_to_borrowed<std::iter_rvalue_reference_t<IteratorT>, RValueRefT> and
    std::convertible_to<std::iter_difference_t<IteratorT>, DiffT>;

template <class RangeT, any_view_options OptsV>
concept any_compatible_approximately_sized_range =
    not flag_is_set<OptsV, any_view_options::approximately_sized> or approximately_sized_range<RangeT>;

template <class RangeT, any_view_options OptsV>
concept any_compatible_sized_range =
    any_compatible_approximately_sized_range<RangeT, OptsV> and
    (not flag_is_set<OptsV, any_view_options::sized> or std::ranges::sized_range<RangeT>);

template <class RangeT, any_view_options OptsV>
concept any_compatible_borrowed_range =
    not flag_is_set<OptsV, any_view_options::borrowed> or
    (not std::ranges::enable_view<std::remove_cvref_t<RangeT>> and std::is_lvalue_reference_v<RangeT>) or
    std::ranges::enable_borrowed_range<std::remove_cvref_t<RangeT>>;

template <class T, class U>
concept different_from = not std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;

} // namespace detail

template <class RangeT, class RefT, class RValueRefT, class DiffT, any_view_options OptsV>
concept ext_any_compatible_range =
    std::ranges::range<RangeT> and
    detail::any_compatible_iterator<std::ranges::iterator_t<RangeT>, RefT, RValueRefT, DiffT, OptsV> and
    detail::any_compatible_sized_range<RangeT, OptsV> and detail::any_compatible_borrowed_range<RangeT, OptsV>;

} // namespace beman::any_view

#endif // BEMAN_ANY_VIEW_CONCEPTS_HPP
