// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_ANY_VIEW_OPTIONS_HPP
#define BEMAN_ANY_VIEW_ANY_VIEW_OPTIONS_HPP

#include <beman/any_view/config.hpp>

#if BEMAN_ANY_VIEW_USE_ENUM()

#include <iterator>

#elif BEMAN_ANY_VIEW_USE_NAMED()

#include <beman/any_view/detail/type_traits.hpp>

#include <iterator>

#endif

namespace beman::any_view {

#if BEMAN_ANY_VIEW_USE_ENUM()

enum class any_view_options {
    input         = 0b0000000,
    forward       = 0b0000001,
    bidirectional = 0b0000011,
    random_access = 0b0000111,
    contiguous    = 0b0001111,
    sized         = 0b0010000,
    borrowed      = 0b0100000,
#if BEMAN_ANY_VIEW_USE_COPYABLE()
    copyable = 0b1000000,
#elif BEMAN_ANY_VIEW_USE_MOVE_ONLY()
    move_only = 0b1000000,
#endif
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
    type_t<IterConceptT> iterator_concept = {};
    bool                 sized            = false;
#if BEMAN_ANY_VIEW_USE_COPYABLE()
    bool copyable = false;
#elif BEMAN_ANY_VIEW_USE_MOVE_ONLY()
    bool move_only = false;
#endif
    bool               borrowed              = false;
    type_t<RValueRefT> rvalue_reference_type = {};
    type_t<DiffT>      difference_type       = {};
};

#endif

} // namespace beman::any_view

#endif // BEMAN_ANY_VIEW_ANY_VIEW_OPTIONS_HPP
