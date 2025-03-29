// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_VIEW_INTERFACE_HPP
#define BEMAN_ANY_VIEW_DETAIL_VIEW_INTERFACE_HPP

#include "any_iterator.hpp"

namespace beman::any_view::detail {

template <class IterConceptT, class ElementT, class RefT, class RValueRefT, class DiffT>
class view_interface {
    using iterator  = detail::any_iterator<IterConceptT, ElementT, RefT, RValueRefT, DiffT>;
    using size_type = std::make_unsigned_t<DiffT>;

  public:
    // in-place construction
    virtual auto copy_to(void* destination) const -> void    = 0;
    virtual auto move_to(void* destination) noexcept -> void = 0;

    // out-of-place construction
    [[nodiscard]] virtual constexpr auto copy() const -> view_interface* = 0;

    // view methods
    [[nodiscard]] virtual constexpr auto begin() -> iterator = 0;

    [[nodiscard]] virtual constexpr auto size() const -> size_type = 0;

    virtual constexpr ~view_interface() noexcept = default;
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_VIEW_INTERFACE_HPP
