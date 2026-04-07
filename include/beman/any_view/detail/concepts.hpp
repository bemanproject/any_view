// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_CONCEPTS_HPP
#define BEMAN_ANY_VIEW_DETAIL_CONCEPTS_HPP

#include <beman/any_view/any_view_options.hpp>
#include <beman/any_view/detail/reference_converts_from_temporary.hpp>

#include <concepts>
#include <iterator>
#include <type_traits>

namespace beman::any_view::detail {

template <any_view_options OptsV, any_view_options FlagV>
concept flag_is_set = (OptsV & FlagV) == FlagV;

template <class IterT, any_view_options OptsV>
concept any_compatible_input_iterator =
    std::input_or_output_iterator<IterT> and
    (not flag_is_set<OptsV, any_view_options::input> or std::input_iterator<IterT>);

template <class IterT, any_view_options OptsV>
concept any_compatible_forward_iterator =
    any_compatible_input_iterator<IterT, OptsV> and
    (not flag_is_set<OptsV, any_view_options::forward> or std::forward_iterator<IterT>);

template <class IterT, any_view_options OptsV>
concept any_compatible_bidirectional_iterator =
    any_compatible_forward_iterator<IterT, OptsV> and
    (not flag_is_set<OptsV, any_view_options::bidirectional> or std::bidirectional_iterator<IterT>);

template <class IterT, any_view_options OptsV>
concept any_compatible_random_access_iterator =
    any_compatible_bidirectional_iterator<IterT, OptsV> and
    (not flag_is_set<OptsV, any_view_options::random_access> or std::random_access_iterator<IterT>);

template <class IterT, any_view_options OptsV>
concept any_compatible_contiguous_iterator =
    any_compatible_random_access_iterator<IterT, OptsV> and
    (not flag_is_set<OptsV, any_view_options::contiguous> or std::contiguous_iterator<IterT>);

template <class TempT, class RefT>
concept convertible_to_borrowed =
    std::convertible_to<TempT, RefT> and not reference_converts_from_temporary_v<RefT, TempT>;

template <class PointerT, class AnyPointerT>
concept uses_nonqualification_pointer_conversion =
    std::is_pointer_v<PointerT> and std::is_pointer_v<AnyPointerT> and
    not std::convertible_to<std::remove_pointer_t<PointerT> (*)[], std::remove_pointer_t<AnyPointerT> (*)[]>;

template <class RefT, class AnyRefT, any_view_options OptsV = any_view_options::contiguous>
concept any_compatible_contiguous_reference =
    convertible_to_borrowed<RefT, AnyRefT> and
    (not flag_is_set<OptsV, any_view_options::contiguous> or
     not uses_nonqualification_pointer_conversion<std::add_pointer_t<RefT>, std::add_pointer_t<AnyRefT>>);

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_CONCEPTS_HPP
