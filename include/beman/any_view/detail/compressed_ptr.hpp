// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_COMPRESSED_PTR_HPP
#define BEMAN_ANY_VIEW_DETAIL_COMPRESSED_PTR_HPP

#include <type_traits>

namespace beman::any_view::detail {

template <class T>
class compressed_ptr {
    T* value;

  public:
    constexpr compressed_ptr(T* value) noexcept : value(value) {}

    [[nodiscard]] constexpr T* operator->() const noexcept { return value; }
    [[nodiscard]] constexpr T& operator*() const noexcept { return *value; }
};

template <class T>
    requires std::is_empty_v<T>
class compressed_ptr<T> {
  public:
    constexpr compressed_ptr(T*) noexcept {}

    [[nodiscard]] constexpr T* operator->() const noexcept { return nullptr; }
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_COMPRESSED_PTR_HPP
