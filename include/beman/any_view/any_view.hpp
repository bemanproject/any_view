// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_ANY_VIEW_HPP
#define BEMAN_ANY_VIEW_ANY_VIEW_HPP

#include <beman/any_view/config.hpp>
#include <beman/any_view/detail/iterator.hpp>

#if BEMAN_ANY_VIEW_USE_FLAGS()

#include <beman/any_view/detail/type_traits.hpp>
#include <beman/any_view/any_view_options.hpp>

#elif BEMAN_ANY_VIEW_USE_TRAITS()

#include <beman/any_view/detail/type_traits.hpp>
#include <beman/any_view/range_traits.hpp>

#elif BEMAN_ANY_VIEW_USE_NAMED()

#include <beman/any_view/any_view_options.hpp>

#endif

#include <ranges>

namespace beman::any_view {

#if BEMAN_ANY_VIEW_USE_FLAGS()

template <class ElementT,
          any_view_options OptionsV = any_view_options::input,
          class RefT                = ElementT&,
          class RValueRefT          = detail::as_rvalue_t<RefT>,
          class DiffT               = std::ptrdiff_t>
class any_view : public std::ranges::view_interface<any_view<ElementT, OptionsV, RefT, RValueRefT, DiffT>> {
    using iterator_concept = detail::iterator_concept_t<OptionsV>;
    using iterator         = detail::iterator<iterator_concept, ElementT, RefT, RValueRefT, DiffT>;
    using sentinel         = std::default_sentinel_t;

    static constexpr bool sized    = (OptionsV & any_view_options::sized) == any_view_options::sized;
    static constexpr bool borrowed = (OptionsV & any_view_options::borrowed) == any_view_options::borrowed;
    static constexpr bool BEMAN_ANY_VIEW_OPTION() =
        (OptionsV & any_view_options::BEMAN_ANY_VIEW_OPTION()) == any_view_options::BEMAN_ANY_VIEW_OPTION();
    static constexpr bool simple = (OptionsV & any_view_options::simple) == any_view_options::simple;

  public:
    auto begin() -> iterator;
    auto end() -> sentinel;
};

#elif BEMAN_ANY_VIEW_USE_TRAITS()

template <class ElementT, class RangeTraitsT = default_range_traits>
class any_view : public std::ranges::view_interface<any_view<ElementT, RangeTraitsT>> {
    using iterator_concept = detail::iterator_concept_or_t<std::input_iterator_tag, RangeTraitsT>;
    using reference_type   = detail::reference_type_or_t<ElementT&, RangeTraitsT>;
    using rvalue_reference_type =
        detail::rvalue_reference_type_or_t<detail::as_rvalue_t<reference_type>, RangeTraitsT>;
    using difference_type = detail::difference_type_or_t<std::ptrdiff_t, RangeTraitsT>;
    using iterator =
        detail::iterator<iterator_concept, ElementT, reference_type, rvalue_reference_type, difference_type>;
    using sentinel = std::default_sentinel_t;

    static constexpr bool sized                   = detail::sized_or_v<false, RangeTraitsT>;
    static constexpr bool borrowed                = detail::borrowed_or_v<false, RangeTraitsT>;
    static constexpr bool BEMAN_ANY_VIEW_OPTION() = detail::BEMAN_ANY_VIEW_OPTION_(or_v)<false, RangeTraitsT>;
    static constexpr bool simple                  = detail::simple_or_v<false, RangeTraitsT>;

  public:
    auto begin() -> iterator;
    auto end() -> sentinel;
};

#elif BEMAN_ANY_VIEW_USE_NAMED()

template <class ElementT, any_view_options OptionsV = {.reference_type = type<ElementT&>}>
class any_view : public std::ranges::view_interface<any_view<ElementT, OptionsV>> {
    using iterator_concept      = decltype(OptionsV.iterator_concept)::type;
    using reference_type        = decltype(OptionsV.reference_type)::type;
    using rvalue_reference_type = decltype(OptionsV.rvalue_reference_type)::type;
    using difference_type       = decltype(OptionsV.difference_type)::type;
    using iterator =
        detail::iterator<iterator_concept, ElementT, reference_type, rvalue_reference_type, difference_type>;
    using sentinel = std::default_sentinel_t;

    static constexpr bool sized                   = OptionsV.sized;
    static constexpr bool borrowed                = OptionsV.borrowed;
    static constexpr bool BEMAN_ANY_VIEW_OPTION() = OptionsV.BEMAN_ANY_VIEW_OPTION();
    static constexpr bool simple                  = OptionsV.simple;

  public:
    auto begin() -> iterator;
    auto end() -> sentinel;
};

#endif

} // namespace beman::any_view

#endif // BEMAN_ANY_VIEW_ANY_VIEW_HPP
