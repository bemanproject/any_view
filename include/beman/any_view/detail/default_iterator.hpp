// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_DEFAULT_ITERATOR_HPP
#define BEMAN_ANY_VIEW_DETAIL_DEFAULT_ITERATOR_HPP

#include <beman/any_view/detail/unreachable.hpp>

#include <compare>
#include <concepts>
#include <iterator>

namespace beman::any_view::detail {

template <class ElementT, class RefT, class RValueRefT, class DiffT>
struct iterator_concept {
    using type = std::random_access_iterator_tag;
};

template <class ElementT>
struct iterator_concept<ElementT, ElementT&, ElementT&&, std::ptrdiff_t> {
    using type = std::contiguous_iterator_tag;
};

template <class ElementT, class RefT, class RValueRefT, class DiffT>
class default_iterator {
  public:
    using iterator_concept = typename iterator_concept<ElementT, RefT, RValueRefT, DiffT>::type;
    using element_type     = ElementT;

    constexpr RefT operator*() const noexcept { unreachable(); }

    constexpr friend RValueRefT iter_move(default_iterator) noexcept { unreachable(); }

    constexpr default_iterator& operator++() noexcept { unreachable(); }

    constexpr default_iterator operator++(int) noexcept { unreachable(); }

    constexpr std::strong_ordering operator<=>(const default_iterator&) const noexcept = default;

    constexpr default_iterator& operator--() noexcept { unreachable(); }

    constexpr default_iterator operator--(int) noexcept { unreachable(); }

    constexpr DiffT operator-(default_iterator) const noexcept { return 0; }

    constexpr default_iterator& operator+=(DiffT) noexcept { unreachable(); }

    constexpr default_iterator operator+(DiffT) const noexcept { unreachable(); }

    constexpr friend default_iterator operator+(DiffT, default_iterator) noexcept { unreachable(); }

    constexpr default_iterator& operator-=(DiffT) noexcept { unreachable(); }

    constexpr default_iterator operator-(DiffT) const noexcept { unreachable(); }

    constexpr RefT operator[](DiffT) const noexcept { unreachable(); }

    constexpr element_type* operator->() const noexcept
        requires std::derived_from<iterator_concept, std::contiguous_iterator_tag>
    {
        return nullptr;
    }
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_DEFAULT_ITERATOR_HPP
