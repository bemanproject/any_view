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
    input                   = 0b000000001,
    forward                 = 0b000000011,
    bidirectional           = 0b000000111,
    random_access           = 0b000001111,
    contiguous              = 0b000011111,
    sized                   = 0b000100000,
    borrowed                = 0b001000000,
    BEMAN_ANY_VIEW_OPTION() = 0b010000000,
    simple                  = 0b100000000,
};

constexpr auto operator|(any_view_options l, any_view_options r) noexcept -> any_view_options {
    return any_view_options(static_cast<int>(l) | static_cast<int>(r));
}

constexpr auto operator&(any_view_options l, any_view_options r) noexcept -> any_view_options {
    return any_view_options(static_cast<int>(l) & static_cast<int>(r));
}

constexpr auto operator^(any_view_options l, any_view_options r) noexcept -> any_view_options {
    return any_view_options(static_cast<int>(l) ^ static_cast<int>(r));
}

constexpr auto operator~(any_view_options k) noexcept -> any_view_options {
    return any_view_options(~static_cast<int>(k));
}

constexpr auto operator|=(any_view_options& l, any_view_options r) noexcept -> any_view_options& { return l = l | r; }

constexpr auto operator&=(any_view_options& l, any_view_options r) noexcept -> any_view_options& { return l = l & r; }

constexpr auto operator^=(any_view_options& l, any_view_options r) noexcept -> any_view_options& { return l = l ^ r; }

namespace detail {

template <any_view_options OptionsV>
consteval auto get_iterator_concept() {
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
        static_assert(iterator_concept == input);
        return std::input_iterator_tag{};
    }
}

template <any_view_options OptionsV>
using iterator_concept_t = decltype(detail::get_iterator_concept<OptionsV>());

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
    type_t<IterConceptT> iterator_concept;
    bool                 sized;
    bool                 BEMAN_ANY_VIEW_OPTION();
    bool                 borrowed;
    bool                 simple;
    type_t<RValueRefT>   rvalue_reference_type;
    type_t<DiffT>        difference_type;
};

#endif

} // namespace beman::any_view

#endif // BEMAN_ANY_VIEW_ANY_VIEW_OPTIONS_HPP
