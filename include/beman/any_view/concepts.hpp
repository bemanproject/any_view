// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_CONCEPTS_HPP
#define BEMAN_ANY_VIEW_CONCEPTS_HPP

#include <beman/any_view/any_view_options.hpp>
#include <beman/any_view/detail/concepts.hpp>
#include <beman/any_view/reserve_hint.hpp>

#include <ranges>

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
    not flag_is_set<OptsV, any_view_options::borrowed> or std::ranges::borrowed_range<RangeT>;

template <class ViewT, any_view_options OptsV>
concept any_compatible_copyable_view = not flag_is_set<OptsV, any_view_options::copyable> or std::copyable<ViewT>;

template <class ViewT, class RefT, class RValueRefT, class DiffT, any_view_options OptsV>
concept any_compatible_view =
    any_compatible_iterator<std::ranges::iterator_t<ViewT>, RefT, RValueRefT, DiffT, OptsV> and
    any_compatible_sized_range<ViewT, OptsV> and any_compatible_borrowed_range<ViewT, OptsV>;

template <class RangeT>
struct make_view {
    using type = std::views::all_t<RangeT>;
};

template <class RangeT>
    requires std::ranges::enable_view<std::remove_cvref_t<RangeT>>
struct make_view<RangeT> {
    using type = std::remove_cvref_t<RangeT>;
};

template <class RangeT>
using make_view_t = typename make_view<RangeT>::type;

} // namespace detail

template <class RangeT, class RefT, class RValueRefT, class DiffT, any_view_options OptsV>
concept ext_any_compatible_range =
    std::ranges::range<RangeT> and
    detail::any_compatible_view<detail::make_view_t<RangeT>, RefT, RValueRefT, DiffT, OptsV> and
    (std::ranges::enable_view<std::remove_cvref_t<RangeT>> or
     detail::any_compatible_copyable_view<detail::make_view_t<RangeT>, OptsV>);

} // namespace beman::any_view

#endif // BEMAN_ANY_VIEW_CONCEPTS_HPP
