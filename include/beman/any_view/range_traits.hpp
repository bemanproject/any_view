// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_RANGE_TRAITS_HPP
#define BEMAN_ANY_VIEW_RANGE_TRAITS_HPP

#include <beman/any_view/config.hpp>
#include <beman/any_view/detail/concepts.hpp>

#include <ranges>

namespace beman::any_view {
namespace detail {

template <std::ranges::range RangeT>
[[nodiscard]] consteval auto get_range_concept() {
    if constexpr (std::ranges::contiguous_range<RangeT>) {
        return std::contiguous_iterator_tag{};
    } else if constexpr (std::ranges::random_access_range<RangeT>) {
        return std::random_access_iterator_tag{};
    } else if constexpr (std::ranges::bidirectional_range<RangeT>) {
        return std::bidirectional_iterator_tag{};
    } else if constexpr (std::ranges::forward_range<RangeT>) {
        return std::forward_iterator_tag{};
    } else {
        return std::input_iterator_tag{};
    }
}

template <class RangeT>
using range_concept_t = decltype(detail::get_range_concept<RangeT>());

template <class DefaultT, template <class...> class OpT, class... ArgsT>
struct detected_or : std::type_identity<DefaultT> {};

template <class DefaultT, template <class...> class OpT, class... ArgsT>
    requires requires { typename OpT<ArgsT...>; }
struct detected_or<DefaultT, OpT, ArgsT...> : std::type_identity<OpT<ArgsT...>> {};

template <class DefaultT, template <class...> class OpT, class... ArgsT>
using detected_or_t = detected_or<DefaultT, OpT, ArgsT...>::type;

#define BEMAN_ANY_VIEW_TYPE_TRAIT(name)           \
    template <class RangeTraitsT>                 \
    using name##_t = RangeTraitsT::name;          \
                                                  \
    template <class DefaultT, class RangeTraitsT> \
    using name##_or_t = detected_or_t<DefaultT, name##_t, RangeTraitsT>

BEMAN_ANY_VIEW_TYPE_TRAIT(iterator_concept);
BEMAN_ANY_VIEW_TYPE_TRAIT(reference_type);
BEMAN_ANY_VIEW_TYPE_TRAIT(rvalue_reference_type);
BEMAN_ANY_VIEW_TYPE_TRAIT(difference_type);

#undef BEMAN_ANY_VIEW_TYPE_TRAIT

#define BEMAN_ANY_VIEW_BOOL_TRAIT(name)                  \
    template <class RangeTraitsT>                        \
    using name = std::bool_constant<RangeTraitsT::name>; \
                                                         \
    template <bool DefaultV, class RangeTraitsT>         \
    inline constexpr bool name##_or_v = detected_or_t<std::bool_constant<DefaultV>, name, RangeTraitsT>::value

BEMAN_ANY_VIEW_BOOL_TRAIT(sized);
BEMAN_ANY_VIEW_BOOL_TRAIT(borrowed);
#if BEMAN_ANY_VIEW_USE_COPYABLE()
BEMAN_ANY_VIEW_BOOL_TRAIT(copyable);
#elif BEMAN_ANY_VIEW_USE_MOVE_ONLY()
BEMAN_ANY_VIEW_BOOL_TRAIT(move_only);
#endif

#undef BEMAN_ANY_VIEW_BOOL_TRAIT

} // namespace detail

struct default_range_traits {};

template <std::ranges::range RangeT>
struct range_traits {
    using iterator_concept      = detail::range_concept_t<RangeT>;
    using reference_type        = std::ranges::range_reference_t<RangeT>;
    using rvalue_reference_type = std::ranges::range_rvalue_reference_t<RangeT>;
    using difference_type       = std::ranges::range_difference_t<RangeT>;

    static constexpr bool sized    = std::ranges::sized_range<RangeT>;
    static constexpr bool borrowed = std::ranges::borrowed_range<RangeT>;
#if BEMAN_ANY_VIEW_USE_COPYABLE()
    static constexpr bool copyable = std::copyable<RangeT>;
#elif BEMAN_ANY_VIEW_USE_MOVE_ONLY()
    static constexpr bool move_only = not std::copyable<RangeT>;
#endif
};

} // namespace beman::any_view

#endif // BEMAN_ANY_VIEW_RANGE_TRAITS_HPP
