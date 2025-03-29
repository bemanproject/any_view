// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_VIEW_ADAPTOR_HPP
#define BEMAN_ANY_VIEW_DETAIL_VIEW_ADAPTOR_HPP

#include "view_interface.hpp"

namespace beman::any_view::detail {

template <class IterConceptT, class ElementT, class RefT, class RValueRefT, class DiffT, std::ranges::view ViewT>
class view_adaptor final : public view_interface<IterConceptT, ElementT, RefT, RValueRefT, DiffT> {
    using iterator  = detail::any_iterator<IterConceptT, ElementT, RefT, RValueRefT, DiffT>;
    using size_type = std::make_unsigned_t<DiffT>;

    [[no_unique_address]] ViewT view;

    static constexpr auto get_noexcept() { return std::is_nothrow_move_constructible_v<ViewT>; }

  public:
    constexpr view_adaptor(ViewT&& view) noexcept(get_noexcept()) : view(std::move(view)) {}

    constexpr view_adaptor(const view_adaptor&) = default;

    constexpr view_adaptor(view_adaptor&&) noexcept = default;

    auto copy_to(void* destination) const -> void override {
        if constexpr (std::copy_constructible<ViewT>) {
            ::new (destination) view_adaptor(*this);
        } else {
            unreachable();
        }
    }

    auto move_to(void* destination) noexcept -> void override {
        if constexpr (std::is_nothrow_move_constructible_v<ViewT>) {
            ::new (destination) view_adaptor(std::move(*this));
        } else {
            unreachable();
        }
    }

    [[nodiscard]] constexpr auto copy() const -> view_adaptor* override {
        if constexpr (std::copy_constructible<ViewT>) {
            return new view_adaptor(*this);
        } else {
            unreachable();
        }
    }

    [[nodiscard]] constexpr auto begin() -> iterator override {
        if constexpr (any_compatible_contiguous_reference<std::ranges::range_reference_t<ViewT>, RefT, IterConceptT>) {
            return iterator{std::ranges::begin(view), std::ranges::end(view)};
        } else {
            unreachable();
        }
    }

    [[nodiscard]] constexpr auto size() const -> size_type override {
        if constexpr (std::ranges::sized_range<ViewT>) {
            return std::ranges::size(view);
        } else {
            unreachable();
        }
    }

    // workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=104653
    constexpr ~view_adaptor() noexcept override {}
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_VIEW_ADAPTOR_HPP
