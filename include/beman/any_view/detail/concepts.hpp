// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_CONCEPTS_HPP
#define BEMAN_ANY_VIEW_DETAIL_CONCEPTS_HPP

#include <beman/any_view/config.hpp>

#include <iterator>

namespace beman::any_view::detail {

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

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_CONCEPTS_HPP
