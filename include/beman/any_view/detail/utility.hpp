// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_UTILITY_HPP
#define BEMAN_ANY_VIEW_DETAIL_UTILITY_HPP

namespace beman::any_view::detail {

[[noreturn]] inline auto unreachable() -> void {
#if defined(_MSC_VER)
    __assume(false);
#elif defined(__GNUG__)
    __builtin_unreachable();
#endif
}

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_UTILITY_HPP
