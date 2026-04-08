// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_DEFAULT_ITERATOR_HPP
#define BEMAN_ANY_VIEW_DETAIL_DEFAULT_ITERATOR_HPP

#include <beman/any_view/detail/unreachable.hpp>

#include <compare>
#include <iterator>

namespace beman::any_view::detail {

template <class ElementT, class RefT, class RValueRefT, class DiffT>
class default_iterator {
  public:
    using iterator_concept = std::contiguous_iterator_tag;
    using element_type     = ElementT;

    RefT operator*() const noexcept { unreachable(); }

    friend RValueRefT iter_move(default_iterator) noexcept { unreachable(); }

    default_iterator& operator++() noexcept { unreachable(); }

    default_iterator operator++(int) noexcept { unreachable(); }

    constexpr bool operator==(std::default_sentinel_t) const noexcept { return true; }

    constexpr bool operator==(const default_iterator&) const noexcept = default;

    constexpr std::strong_ordering operator<=>(const default_iterator&) const noexcept = default;

    default_iterator& operator--() noexcept { unreachable(); }

    default_iterator operator--(int) noexcept { unreachable(); }

    constexpr DiffT operator-(std::default_sentinel_t) const noexcept { return 0; }

    constexpr friend DiffT operator-(std::default_sentinel_t, default_iterator) noexcept { return 0; }

    constexpr DiffT operator-(default_iterator) const noexcept { return 0; }

    default_iterator& operator+=(DiffT) noexcept { unreachable(); }

    default_iterator operator+(DiffT) const noexcept { unreachable(); }

    friend default_iterator operator+(DiffT, default_iterator) noexcept { unreachable(); }

    default_iterator& operator-=(DiffT) noexcept { unreachable(); }

    default_iterator operator-(DiffT) const noexcept { unreachable(); }

    RefT operator[](DiffT) const noexcept { unreachable(); }

    constexpr element_type* operator->() const noexcept { return nullptr; }
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_DEFAULT_ITERATOR_HPP
