// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_REFERENCE_CONVERTS_FROM_TEMPORARY_HPP
#define BEMAN_ANY_VIEW_DETAIL_REFERENCE_CONVERTS_FROM_TEMPORARY_HPP

namespace beman::any_view::detail {

template <class T, class U>
inline constexpr bool reference_converts_from_temporary_v = false;

#ifdef __has_builtin
#if __has_builtin(__reference_converts_from_temporary)

template <class T, class U>
    requires true
inline constexpr bool reference_converts_from_temporary_v<T, U> = __reference_converts_from_temporary(T, U);

#endif // __has_builtin(__reference_converts_from_temporary)
#endif // __has_builtin

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_REFERENCE_CONVERTS_FROM_TEMPORARY_HPP
