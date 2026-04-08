// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_DEFAULT_VIEW_HPP
#define BEMAN_ANY_VIEW_DETAIL_DEFAULT_VIEW_HPP

#include <beman/any_view/detail/default_iterator.hpp>

#include <ranges>

namespace beman::any_view::detail {

template <class ElementT, class RefT, class RValueRefT, class DiffT>
class default_view : public std::ranges::view_interface<default_view<ElementT, RefT, RValueRefT, DiffT>> {
    using iterator = default_iterator<ElementT, RefT, RValueRefT, DiffT>;
    using sentinel = std::default_sentinel_t;

  public:
    [[nodiscard]] constexpr iterator begin() const { return {}; }
    [[nodiscard]] constexpr sentinel end() const { return std::default_sentinel; }
};

} // namespace beman::any_view::detail

template <class ElementT, class RefT, class RValueRefT, class DiffT>
inline constexpr bool
    std::ranges::enable_borrowed_range<beman::any_view::detail::default_view<ElementT, RefT, RValueRefT, DiffT>> =
        true;

#endif // BEMAN_ANY_VIEW_DETAIL_DEFAULT_VIEW_HPP
