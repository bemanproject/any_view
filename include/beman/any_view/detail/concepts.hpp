// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_CONCEPTS_HPP
#define BEMAN_ANY_VIEW_DETAIL_CONCEPTS_HPP

#include <iterator>

namespace beman::any_view::detail {

template <class IterT, class IterConceptT>
concept any_compatible_input_iterator =
    std::input_or_output_iterator<IterT> and
    (not std::derived_from<IterConceptT, std::input_iterator_tag> or std::input_iterator<IterT>);

template <class IterT, class IterConceptT>
concept any_compatible_forward_iterator =
    any_compatible_input_iterator<IterT, IterConceptT> and
    (not std::derived_from<IterConceptT, std::forward_iterator_tag> or std::forward_iterator<IterT>);

template <class IterT, class IterConceptT>
concept any_compatible_bidirectional_iterator =
    any_compatible_forward_iterator<IterT, IterConceptT> and
    (not std::derived_from<IterConceptT, std::bidirectional_iterator_tag> or std::bidirectional_iterator<IterT>);

template <class IterT, class IterConceptT>
concept any_compatible_random_access_iterator =
    any_compatible_bidirectional_iterator<IterT, IterConceptT> and
    (not std::derived_from<IterConceptT, std::random_access_iterator_tag> or std::random_access_iterator<IterT>);

template <class IterT, class IterConceptT>
concept any_compatible_contiguous_iterator =
    any_compatible_random_access_iterator<IterT, IterConceptT> and
    (not std::derived_from<IterConceptT, std::contiguous_iterator_tag> or std::contiguous_iterator<IterT>);

template <class PointerT, class AnyPointerT>
concept uses_nonqualification_pointer_conversion =
    std::is_pointer_v<PointerT> and std::is_pointer_v<AnyPointerT> and
    not std::convertible_to<std::remove_pointer_t<PointerT> (*)[], std::remove_pointer_t<AnyPointerT> (*)[]>;

template <class RefT, class AnyRefT, class IterConceptT = std::contiguous_iterator_tag>
concept any_compatible_contiguous_reference =
    std::convertible_to<RefT, AnyRefT> and
    (not std::derived_from<IterConceptT, std::contiguous_iterator_tag> or
     not uses_nonqualification_pointer_conversion<std::add_pointer_t<RefT>, std::add_pointer_t<AnyRefT>>);

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_CONCEPTS_HPP
