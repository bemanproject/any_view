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

#if _MSC_VER
    using compressed_ptr<Ts>::operator->*...;
#endif // _MSC_VER
};

template <class T>
class compressed_ptr<T> {
    T* value;

  public:
    constexpr explicit compressed_ptr(T* value) noexcept : value(value) {}

    [[nodiscard]] constexpr operator T*() const noexcept { return value; }

#if _MSC_VER
    template <class MemberT, class BaseT>
        requires std::derived_from<T, BaseT>
    [[nodiscard]] constexpr const MemberT& operator->*(MemberT BaseT::*member_ptr) const noexcept {
        return value->*member_ptr;
    }
#endif // _MSC_VER
};

template <class T>
    requires std::is_empty_v<T>
class compressed_ptr<T> {
  public:
    constexpr explicit compressed_ptr(T*) noexcept {}

    [[nodiscard]] constexpr operator T*() const noexcept { return nullptr; }

#if _MSC_VER
    template <class MemberT, class BaseT>
        requires std::derived_from<T, BaseT>
    const MemberT& operator->*(MemberT BaseT::*) const = delete;
#endif // _MSC_VER
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_COMPRESSED_PTR_HPP
