// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_ANY_VIEW_OPTIONS_HPP
#define BEMAN_ANY_VIEW_ANY_VIEW_OPTIONS_HPP

namespace beman::any_view {

enum class any_view_options {
    input         = 0b0000000,
    forward       = 0b0000001,
    bidirectional = 0b0000011,
    random_access = 0b0000111,
    contiguous    = 0b0001111,
    sized         = 0b0010000,
    borrowed      = 0b0100000,
    copyable      = 0b1000000,
};

[[nodiscard]] constexpr auto operator|(any_view_options l, any_view_options r) noexcept -> any_view_options {
    return any_view_options(static_cast<int>(l) | static_cast<int>(r));
}

[[nodiscard]] constexpr auto operator&(any_view_options l, any_view_options r) noexcept -> any_view_options {
    return any_view_options(static_cast<int>(l) & static_cast<int>(r));
}

[[nodiscard]] constexpr auto operator^(any_view_options l, any_view_options r) noexcept -> any_view_options {
    return any_view_options(static_cast<int>(l) ^ static_cast<int>(r));
}

[[nodiscard]] constexpr auto operator~(any_view_options o) noexcept -> any_view_options {
    return any_view_options(~static_cast<int>(o));
}

constexpr auto operator|=(any_view_options& l, any_view_options r) noexcept -> any_view_options& { return l = l | r; }

constexpr auto operator&=(any_view_options& l, any_view_options r) noexcept -> any_view_options& { return l = l & r; }

constexpr auto operator^=(any_view_options& l, any_view_options r) noexcept -> any_view_options& { return l = l ^ r; }

} // namespace beman::any_view

#endif // BEMAN_ANY_VIEW_ANY_VIEW_OPTIONS_HPP
