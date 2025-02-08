// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_ANY_VIEW_HPP
#define BEMAN_ANY_VIEW_ANY_VIEW_HPP

#include <beman/any_view/concepts.hpp>
#include <beman/any_view/config.hpp>
#include <beman/any_view/detail/intrusive_small_ptr.hpp>
#include <beman/any_view/detail/any_iterator.hpp>
#include <beman/any_view/detail/view_adaptor.hpp>
#include <beman/any_view/detail/view_interface.hpp>

#if BEMAN_ANY_VIEW_USE_ENUM()

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
namespace detail {

template <class RangeT, class AnyViewT>
concept viewable_range_compatible_with = not std::same_as<std::remove_cvref_t<RangeT>, AnyViewT> and
                                         ext_viewable_range_compatible_with<RangeT, range_traits<AnyViewT>>;

} // namespace detail

#if BEMAN_ANY_VIEW_USE_ENUM()

template <class ElementT,
          any_view_options OptionsV = any_view_options::input,
          class RefT                = ElementT&,
          class RValueRefT          = detail::as_rvalue_t<RefT>,
          class DiffT               = std::ptrdiff_t>
class any_view : public std::ranges::view_interface<any_view<ElementT, OptionsV, RefT, RValueRefT, DiffT>> {
    using iterator_concept = decltype(detail::get_iterator_concept<OptionsV>());
    using iterator         = detail::any_iterator<iterator_concept, ElementT, RefT, RValueRefT, DiffT>;
    using sentinel         = std::default_sentinel_t;
    using size_type        = std::make_unsigned_t<DiffT>;

    static constexpr bool sized = (OptionsV & any_view_options::sized) == any_view_options::sized;
    static constexpr bool copyable =
#if BEMAN_ANY_VIEW_USE_MOVE_ONLY()
        not
#endif
        ((OptionsV & any_view_options::BEMAN_ANY_VIEW_OPTION()) == any_view_options::BEMAN_ANY_VIEW_OPTION());

    using interface_type = detail::view_interface<iterator_concept, ElementT, RefT, RValueRefT, DiffT>;
    // inplace storage sufficient for a vtable pointer and a std::vector<T>
    detail::intrusive_small_ptr<interface_type, 4 * sizeof(void*)> view_ptr;

    template <class RangeT>
    static consteval auto get_in_place_adaptor_type() {
        return std::in_place_type<
            detail::view_adaptor<iterator_concept, ElementT, RefT, RValueRefT, DiffT, std::views::all_t<RangeT>>>;
    }

  public:
    template <detail::viewable_range_compatible_with<any_view> RangeT>
    constexpr any_view(RangeT&& range)
        : view_ptr(get_in_place_adaptor_type<RangeT>(), std::views::all(std::forward<RangeT>(range))) {}

    constexpr any_view(const any_view&)
        requires copyable
    = default;

    constexpr any_view(any_view&&) noexcept = default;

    constexpr auto operator=(const any_view&) -> any_view&
        requires copyable
    = default;

    constexpr auto operator=(any_view&&) noexcept -> any_view& = default;

    [[nodiscard]] constexpr auto begin() -> iterator { return view_ptr->begin(); }

    [[nodiscard]] constexpr auto end() -> sentinel { return std::default_sentinel; }

    [[nodiscard]] constexpr auto size() const -> size_type
        requires sized
    {
        return view_ptr->size();
    }
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
        detail::any_iterator<iterator_concept, ElementT, reference_type, rvalue_reference_type, difference_type>;
    using sentinel  = std::default_sentinel_t;
    using size_type = std::make_unsigned_t<difference_type>;

    static constexpr bool sized = detail::sized_or_v<false, RangeTraitsT>;
    static constexpr bool copyable =
#if BEMAN_ANY_VIEW_USE_MOVE_ONLY()
        not
#endif
        detail::BEMAN_ANY_VIEW_OPTION_(or_v)<false, RangeTraitsT>;

    using interface_type =
        detail::view_interface<iterator_concept, ElementT, reference_type, rvalue_reference_type, difference_type>;
    // inplace storage sufficient for a vtable pointer and a std::vector<T>
    detail::intrusive_small_ptr<interface_type, 4 * sizeof(void*)> view_ptr;

    template <class RangeT>
    static consteval auto get_in_place_adaptor_type() {
        return std::in_place_type<detail::view_adaptor<iterator_concept,
                                                       ElementT,
                                                       reference_type,
                                                       rvalue_reference_type,
                                                       difference_type,
                                                       std::views::all_t<RangeT>>>;
    }

  public:
    template <detail::viewable_range_compatible_with<any_view> RangeT>
    constexpr any_view(RangeT&& range)
        : view_ptr(get_in_place_adaptor_type<RangeT>(), std::views::all(std::forward<RangeT>(range))) {}

    constexpr any_view(const any_view&)
        requires copyable
    = default;

    constexpr any_view(any_view&&) noexcept = default;

    constexpr auto operator=(const any_view&) -> any_view&
        requires copyable
    = default;

    constexpr auto operator=(any_view&&) noexcept -> any_view& = default;

    [[nodiscard]] constexpr auto begin() -> iterator { return view_ptr->begin(); }

    [[nodiscard]] constexpr auto end() -> sentinel { return std::default_sentinel; }

    [[nodiscard]] constexpr auto size() const -> size_type
        requires sized
    {
        return view_ptr->size();
    }
};

#elif BEMAN_ANY_VIEW_USE_NAMED()

template <class ElementT, any_view_options OptionsV = {.reference_type = type<ElementT&>}>
class any_view : public std::ranges::view_interface<any_view<ElementT, OptionsV>> {
    using iterator_concept      = decltype(OptionsV.iterator_concept)::type;
    using reference_type        = decltype(OptionsV.reference_type)::type;
    using rvalue_reference_type = decltype(OptionsV.rvalue_reference_type)::type;
    using difference_type       = decltype(OptionsV.difference_type)::type;
    using iterator =
        detail::any_iterator<iterator_concept, ElementT, reference_type, rvalue_reference_type, difference_type>;
    using sentinel  = std::default_sentinel_t;
    using size_type = std::make_unsigned_t<difference_type>;

    static constexpr bool sized = OptionsV.sized;
    static constexpr bool copyable =
#if BEMAN_ANY_VIEW_USE_MOVE_ONLY()
        not
#endif
        OptionsV.BEMAN_ANY_VIEW_OPTION();

    using interface_type =
        detail::view_interface<iterator_concept, ElementT, reference_type, rvalue_reference_type, difference_type>;
    // inplace storage sufficient for a vtable pointer and a std::vector<T>
    detail::intrusive_small_ptr<interface_type, 4 * sizeof(void*)> view_ptr;

    template <class RangeT>
    static consteval auto get_in_place_adaptor_type() {
        return std::in_place_type<detail::view_adaptor<iterator_concept,
                                                       ElementT,
                                                       reference_type,
                                                       rvalue_reference_type,
                                                       difference_type,
                                                       std::views::all_t<RangeT>>>;
    }

  public:
    template <detail::viewable_range_compatible_with<any_view> RangeT>
    constexpr any_view(RangeT&& range)
        : view_ptr(get_in_place_adaptor_type<RangeT>(), std::views::all(std::forward<RangeT>(range))) {}

    constexpr any_view(const any_view&)
        requires copyable
    = default;

    constexpr any_view(any_view&&) noexcept = default;

    constexpr auto operator=(const any_view&) -> any_view&
        requires copyable
    = default;

    constexpr auto operator=(any_view&&) noexcept -> any_view& = default;

    [[nodiscard]] constexpr auto begin() -> iterator { return view_ptr->begin(); }

    [[nodiscard]] constexpr auto end() -> sentinel { return std::default_sentinel; }

    [[nodiscard]] constexpr auto size() const -> size_type
        requires sized
    {
        return view_ptr->size();
    }
};

#endif

} // namespace beman::any_view

#if BEMAN_ANY_VIEW_USE_ENUM()

template <class ElementT, auto OptionsV, class RefT, class RValueRefT, class DiffT>
inline constexpr bool
    std::ranges::enable_borrowed_range<beman::any_view::any_view<ElementT, OptionsV, RefT, RValueRefT, DiffT>> =
        (OptionsV & beman::any_view::any_view_options::borrowed) == beman::any_view::any_view_options::borrowed;

#elif BEMAN_ANY_VIEW_USE_TRAITS()

template <class ElementT, class RangeTraitsT>
inline constexpr bool std::ranges::enable_borrowed_range<beman::any_view::any_view<ElementT, RangeTraitsT>> =
    beman::any_view::detail::borrowed_or_v<false, RangeTraitsT>;

#elif BEMAN_ANY_VIEW_USE_NAMED()

template <class ElementT, auto OptionsV>
inline constexpr bool std::ranges::enable_borrowed_range<beman::any_view::any_view<ElementT, OptionsV>> =
    OptionsV.borrowed;

#endif

#endif // BEMAN_ANY_VIEW_ANY_VIEW_HPP
