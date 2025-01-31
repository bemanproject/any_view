// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_ITERATOR_HPP
#define BEMAN_ANY_VIEW_DETAIL_ITERATOR_HPP

#include <beman/any_view/detail/type_traits.hpp>

#include <compare>
#include <type_traits>
#include <iterator>

namespace beman::any_view::detail {

template <class IterConceptT, class ElementT, class RefT, class RValueRefT, class DiffT>
class iterator {
    using reference = RefT;
    using pointer   = std::add_pointer_t<RefT>;

  public:
    using iterator_concept = IterConceptT;
    using element_type     = ElementT;
    using difference_type  = DiffT;

    auto operator*() const -> reference;

    auto operator->() const -> pointer;

    auto operator++() -> iterator&;

    auto operator++(int) -> iterator;

    auto operator==(const iterator&) const -> bool;

    auto operator--() -> iterator&;

    auto operator--(int) -> iterator;

    auto operator<=>(const iterator&) const -> std::partial_ordering;

    auto operator-(const iterator&) const -> difference_type;

    auto operator+=(difference_type) -> iterator&;

    auto operator+(difference_type) const -> iterator;

    friend auto operator+(difference_type, const iterator&) -> iterator;

    auto operator-=(difference_type) -> iterator&;

    auto operator-(difference_type) const -> iterator;

    auto operator[](difference_type) const -> reference;

    auto operator==(std::default_sentinel_t) const -> bool;
};

template <class IterConceptT, class ElementT, class RefT, class RValueRefT, class DiffT>
auto iter_move(const iterator<IterConceptT, ElementT, RefT, RValueRefT, DiffT>&) -> RValueRefT;

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_ITERATOR_HPP
