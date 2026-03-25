// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_UNREACHABLE_ITERATOR_HPP
#define BEMAN_ANY_VIEW_DETAIL_UNREACHABLE_ITERATOR_HPP

#include <beman/any_view/detail/utility.hpp>

#include <compare>
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
class unreachable_iterator {
  public:
    using iterator_concept = typename iterator_concept<ElementT, RefT, RValueRefT, DiffT>::type;
    using element_type     = ElementT;
    using difference_type  = DiffT;

    constexpr RefT operator*() const noexcept { unreachable(); }

    constexpr friend RValueRefT iter_move(unreachable_iterator) noexcept { unreachable(); }

    constexpr unreachable_iterator& operator++() noexcept { return *this; }

    constexpr unreachable_iterator operator++(int) noexcept { return *this; }

    constexpr std::strong_ordering operator<=>(const unreachable_iterator&) const noexcept = default;

    constexpr unreachable_iterator& operator--() noexcept { return *this; }

    constexpr unreachable_iterator operator--(int) noexcept { return *this; }

    constexpr difference_type operator-(unreachable_iterator) const noexcept { return 0; }

    constexpr unreachable_iterator& operator+=(difference_type) noexcept { return *this; }

    constexpr unreachable_iterator operator+(difference_type) const noexcept { return *this; }

    constexpr friend unreachable_iterator operator+(difference_type, unreachable_iterator) noexcept { return {}; }

    constexpr unreachable_iterator& operator-=(difference_type) noexcept { return *this; }

    constexpr unreachable_iterator operator-(difference_type) const noexcept { return *this; }

    constexpr RefT operator[](difference_type) const noexcept { unreachable(); }

    constexpr element_type* operator->() const noexcept
        requires std::derived_from<iterator_concept, std::contiguous_iterator_tag>
    {
        return nullptr;
    }
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_UNREACHABLE_ITERATOR_HPP
