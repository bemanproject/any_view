// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_COMPRESSED_PTR_HPP
#define BEMAN_ANY_VIEW_DETAIL_COMPRESSED_PTR_HPP

#include <concepts>
#include <type_traits>

namespace beman::any_view::detail {

template <class... Ts>
class compressed_ptr : public compressed_ptr<Ts>... {
  public:
    constexpr explicit compressed_ptr(Ts*... values) noexcept : compressed_ptr<Ts>(values)... {}

    template <std::derived_from<Ts>... OtherTs>
    constexpr compressed_ptr(const compressed_ptr<OtherTs...>& other) noexcept : compressed_ptr<Ts>(other)... {}

    using compressed_ptr<Ts>::operator Ts*...;
};

template <class T>
class compressed_ptr<T> {
    T* value;

  public:
    constexpr explicit compressed_ptr(T* value) noexcept : value(value) {}

    [[nodiscard]] constexpr operator T*() const noexcept { return value; }
};

template <class T>
    requires std::is_empty_v<T>
class compressed_ptr<T> {
  public:
    constexpr explicit compressed_ptr(T*) noexcept {}

    [[nodiscard]] constexpr operator T*() const noexcept { return nullptr; }
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_COMPRESSED_PTR_HPP
