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
    using iterator_concept = iterator_concept<ElementT, RefT, RValueRefT, DiffT>::type;
    using element_type     = ElementT;
    using difference_type  = DiffT;

    constexpr auto operator*() const noexcept -> RefT { unreachable(); }

    constexpr friend auto iter_move(unreachable_iterator) noexcept -> RValueRefT { unreachable(); }

    constexpr auto operator++() noexcept -> unreachable_iterator& { return *this; }

    constexpr auto operator++(int) noexcept -> unreachable_iterator { return *this; }

    constexpr auto operator<=>(const unreachable_iterator&) const noexcept -> std::strong_ordering = default;

    constexpr auto operator--() noexcept -> unreachable_iterator& { return *this; }

    constexpr auto operator--(int) noexcept -> unreachable_iterator { return *this; }

    constexpr auto operator-(unreachable_iterator) const noexcept -> difference_type { return 0; }

    constexpr auto operator+=(difference_type) noexcept -> unreachable_iterator& { return *this; }

    constexpr auto operator+(difference_type) const noexcept -> unreachable_iterator { return *this; }

    constexpr friend auto operator+(difference_type, unreachable_iterator) noexcept -> unreachable_iterator {
        return {};
    }

    constexpr auto operator-=(difference_type) noexcept -> unreachable_iterator& { return *this; }

    constexpr auto operator-(difference_type) const noexcept -> unreachable_iterator { return *this; }

    constexpr auto operator[](difference_type) const noexcept -> RefT { unreachable(); }

    constexpr auto operator->() const noexcept -> element_type*
        requires std::derived_from<iterator_concept, std::contiguous_iterator_tag>
    {
        return nullptr;
    }
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_UNREACHABLE_ITERATOR_HPP
