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
    using reference        = RefT;
    using rvalue_reference = RValueRefT;
    using pointer          = std::add_pointer_t<RefT>;

  public:
    using iterator_concept = IterConceptT;
    using element_type     = ElementT;
    using difference_type  = DiffT;

    [[nodiscard]] constexpr auto operator*() const -> reference;

    friend constexpr auto iter_move(const iterator&) -> rvalue_reference;

    [[nodiscard]] constexpr auto operator->() const -> pointer;

    constexpr auto operator++() -> iterator&;

    [[nodiscard]] constexpr auto operator++(int) -> iterator;

    [[nodiscard]] constexpr auto operator==(const iterator&) const -> bool;

    constexpr auto operator--() -> iterator&;

    [[nodiscard]] constexpr auto operator--(int) -> iterator;

    [[nodiscard]] constexpr auto operator<=>(const iterator&) const -> std::partial_ordering;

    [[nodiscard]] constexpr auto operator-(const iterator&) const -> difference_type;

    constexpr auto operator+=(difference_type) -> iterator&;

    [[nodiscard]] constexpr auto operator+(difference_type) const -> iterator;

    friend constexpr auto operator+(difference_type, const iterator&) -> iterator;

    constexpr auto operator-=(difference_type) -> iterator&;

    [[nodiscard]] constexpr auto operator-(difference_type) const -> iterator;

    [[nodiscard]] constexpr auto operator[](difference_type) const -> reference;

    [[nodiscard]] constexpr auto operator==(std::default_sentinel_t) const -> bool;
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_ITERATOR_HPP
