// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_ANY_VIEW_OPTIONS_HPP
#define BEMAN_ANY_VIEW_ANY_VIEW_OPTIONS_HPP

namespace beman::any_view {

enum class any_view_options {
    input         = 0b00000001,
    forward       = 0b00000011,
    bidirectional = 0b00000111,
    random_access = 0b00001111,
    contiguous    = 0b00011111,
    sized         = 0b00100000,
    borrowed      = 0b01000000,
    copyable      = 0b10000000,
};

[[nodiscard]] constexpr any_view_options operator|(any_view_options l, any_view_options r) noexcept {
    return any_view_options(static_cast<int>(l) | static_cast<int>(r));
}

[[nodiscard]] constexpr any_view_options operator&(any_view_options l, any_view_options r) noexcept {
    return any_view_options(static_cast<int>(l) & static_cast<int>(r));
}

[[nodiscard]] constexpr any_view_options operator^(any_view_options l, any_view_options r) noexcept {
    return any_view_options(static_cast<int>(l) ^ static_cast<int>(r));
}

[[nodiscard]] constexpr any_view_options operator~(any_view_options o) noexcept {
    return any_view_options(~static_cast<int>(o));
}

constexpr any_view_options& operator|=(any_view_options& l, any_view_options r) noexcept { return l = l | r; }

constexpr any_view_options& operator&=(any_view_options& l, any_view_options r) noexcept { return l = l & r; }

constexpr any_view_options& operator^=(any_view_options& l, any_view_options r) noexcept { return l = l ^ r; }

} // namespace beman::any_view

#endif // BEMAN_ANY_VIEW_ANY_VIEW_OPTIONS_HPP
