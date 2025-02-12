// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_ANY_VIEW_HPP
#define BEMAN_ANY_VIEW_ANY_VIEW_HPP

#include <beman/any_view/detail/view_adaptor.hpp>

namespace beman::any_view {
namespace detail {

template <class RangeT, class AnyViewT>
concept viewable_range_compatible_with =
    not std::same_as<std::remove_cvref_t<RangeT>, AnyViewT> and ext_viewable_range_compatible_with<RangeT, AnyViewT>;

} // namespace detail

template <class ElementT,
          any_view_options OptionsV = any_view_options::input,
          class RefT                = ElementT&,
          class RValueRefT          = detail::as_rvalue_t<RefT>,
          class DiffT               = std::ptrdiff_t>
class any_view : public std::ranges::view_interface<any_view<ElementT, OptionsV, RefT, RValueRefT, DiffT>> {
    using iterator_concept = decltype(detail::get_iter_concept<OptionsV>());
    using iterator         = detail::any_iterator<iterator_concept, ElementT, RefT, RValueRefT, DiffT>;
    using sentinel         = std::default_sentinel_t;
    using size_type        = std::make_unsigned_t<DiffT>;

    static constexpr bool sized    = (OptionsV & any_view_options::sized) == any_view_options::sized;
    static constexpr bool copyable = (OptionsV & any_view_options::copyable) == any_view_options::copyable;

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

} // namespace beman::any_view

template <class ElementT, auto OptionsV, class RefT, class RValueRefT, class DiffT>
inline constexpr bool
    std::ranges::enable_borrowed_range<beman::any_view::any_view<ElementT, OptionsV, RefT, RValueRefT, DiffT>> =
        (OptionsV & beman::any_view::any_view_options::borrowed) == beman::any_view::any_view_options::borrowed;

#endif // BEMAN_ANY_VIEW_ANY_VIEW_HPP
