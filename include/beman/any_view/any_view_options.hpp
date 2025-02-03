// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_ANY_VIEW_OPTIONS_HPP
#define BEMAN_ANY_VIEW_ANY_VIEW_OPTIONS_HPP

#include <beman/any_view/config.hpp>

#if BEMAN_ANY_VIEW_USE_FLAGS()

#include <iterator>

#elif BEMAN_ANY_VIEW_USE_NAMED()

#include <beman/any_view/detail/type_traits.hpp>

#include <iterator>

#endif

namespace beman::any_view {

#if BEMAN_ANY_VIEW_USE_FLAGS()

enum class any_view_options {
    input                   = 0b00000000,
    forward                 = 0b00000001,
    bidirectional           = 0b00000011,
    random_access           = 0b00000111,
    contiguous              = 0b00001111,
    sized                   = 0b00010000,
    borrowed                = 0b00100000,
    BEMAN_ANY_VIEW_OPTION() = 0b01000000,
    simple                  = 0b10000000,
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

[[nodiscard]] constexpr auto operator~(any_view_options k) noexcept -> any_view_options {
    return any_view_options(~static_cast<int>(k));
}

constexpr auto operator|=(any_view_options& l, any_view_options r) noexcept -> any_view_options& { return l = l | r; }

constexpr auto operator&=(any_view_options& l, any_view_options r) noexcept -> any_view_options& { return l = l & r; }

constexpr auto operator^=(any_view_options& l, any_view_options r) noexcept -> any_view_options& { return l = l ^ r; }

namespace detail {

template <any_view_options OptionsV>
[[nodiscard]] consteval auto get_iterator_concept() {
    using enum any_view_options;
    constexpr auto iterator_concept = OptionsV & contiguous;

    if constexpr (iterator_concept == contiguous) {
        return std::contiguous_iterator_tag{};
    } else if constexpr (iterator_concept == random_access) {
        return std::random_access_iterator_tag{};
    } else if constexpr (iterator_concept == bidirectional) {
        return std::bidirectional_iterator_tag{};
    } else if constexpr (iterator_concept == forward) {
        return std::forward_iterator_tag{};
    } else {
        return std::input_iterator_tag{};
    }
}

} // namespace detail

#elif BEMAN_ANY_VIEW_USE_NAMED()

template <class T>
struct type_t {
    using type = T;
};

template <class T>
inline constexpr type_t<T> type{};

template <class RefT,
          class IterConceptT = std::input_iterator_tag,
          class RValueRefT   = detail::as_rvalue_t<RefT>,
          class DiffT        = std::ptrdiff_t>
struct any_view_options {
    type_t<RefT>         reference_type;
    type_t<IterConceptT> iterator_concept        = {};
    bool                 sized                   = false;
    bool                 BEMAN_ANY_VIEW_OPTION() = false;
    bool                 borrowed                = false;
    bool                 simple                  = false;
    type_t<RValueRefT>   rvalue_reference_type   = {};
    type_t<DiffT>        difference_type         = {};
};

#endif

} // namespace beman::any_view

#endif // BEMAN_ANY_VIEW_ANY_VIEW_OPTIONS_HPP
