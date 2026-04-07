// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_RESERVE_HINT_HPP
#define BEMAN_ANY_VIEW_RESERVE_HINT_HPP

#include <concepts>
#include <ranges>
#include <type_traits>
#include <utility>

namespace beman::any_view {
namespace detail {

template <class T>
    requires std::convertible_to<T, std::decay_t<T>>
std::decay_t<T> decay_copy(T&& value) noexcept(std::is_nothrow_convertible_v<T, std::decay_t<T>>);

// TODO: implement and use is-integer-like instead of std::integral

template <class T>
concept member_reserve_hint = requires(T& t) {
    { decay_copy(t.reserve_hint()) } -> std::integral;
};

// force argument dependent lookup by failing unqualified name lookup
void reserve_hint() = delete;

template <class T>
concept adl_reserve_hint = requires(T& t) {
    { decay_copy(reserve_hint(t)) } -> std::integral;
};

struct reserve_hint_cpo {
  private:
    template <class T>
    static consteval bool is_noexcept() {
        if constexpr (std::ranges::sized_range<T>) {
            return noexcept(std::ranges::size(std::declval<T&>()));
        } else if constexpr (member_reserve_hint<T>) {
            return noexcept(decay_copy(std::declval<T&>().reserve_hint()));
        } else if constexpr (adl_reserve_hint<T>) {
            return noexcept(decay_copy(reserve_hint(std::declval<T&>())));
        } else {
            return false;
        }
    }

  public:
    template <class T>
        requires std::ranges::sized_range<T> or member_reserve_hint<T> or adl_reserve_hint<T>
    [[nodiscard]] constexpr auto operator()(T&& t) const noexcept(is_noexcept<T&>()) {
        if constexpr (std::ranges::sized_range<T>) {
            return std::ranges::size(t);
        } else if constexpr (member_reserve_hint<T>) {
            return t.reserve_hint();
        } else if constexpr (adl_reserve_hint<T>) {
            return reserve_hint(t);
        }
    }
};

} // namespace detail

inline constexpr detail::reserve_hint_cpo reserve_hint{};

template <class T>
concept approximately_sized_range =
    std::ranges::range<T> and
    // use disjunction with std::ranges::sized_range so it subsumes beman::any_view::approximately_sized_range
    (std::ranges::sized_range<T> or requires(T& t) { beman::any_view::reserve_hint(t); });

} // namespace beman::any_view

#endif // BEMAN_ANY_VIEW_RESERVE_HINT_HPP
