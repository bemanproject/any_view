// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_ANY_VIEW_OPTIONS_HPP
#define BEMAN_ANY_VIEW_ANY_VIEW_OPTIONS_HPP

namespace beman::any_view {

enum class any_view_options {
    input               = 0b000000001,
    forward             = 0b000000011,
    bidirectional       = 0b000000111,
    random_access       = 0b000001111,
    contiguous          = 0b000011111,
    approximately_sized = 0b000100000,
    sized               = 0b001100000,
    borrowed            = 0b010000000,
    copyable            = 0b100000000,
};

[[nodiscard]] constexpr any_view_options operator|(any_view_options l, any_view_options r) noexcept {
    return any_view_options(static_cast<int>(l) | static_cast<int>(r));
}

[[nodiscard]] constexpr any_view_options operator&(any_view_options l, any_view_options r) noexcept {
    return any_view_options(static_cast<int>(l) & static_cast<int>(r));
}

} // namespace beman::any_view

#endif // BEMAN_ANY_VIEW_ANY_VIEW_OPTIONS_HPP
