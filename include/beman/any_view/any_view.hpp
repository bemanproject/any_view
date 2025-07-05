// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_ANY_VIEW_HPP
#define BEMAN_ANY_VIEW_ANY_VIEW_HPP

#include <beman/any_view/detail/default_view.hpp>
#include <beman/any_view/detail/view_adaptor.hpp>

namespace beman::any_view {

template <class ElementT,
          any_view_options OptsV = any_view_options::input,
          class RefT             = ElementT&,
          class RValueRefT       = detail::rvalue_ref_t<RefT>,
          class DiffT            = std::ptrdiff_t>
class any_view : public std::ranges::view_interface<any_view<ElementT, OptsV, RefT, RValueRefT, DiffT>> {
    static_assert(bool(OptsV & any_view_options::input), "any_view must model input_range");

    using iterator_concept = decltype(detail::get_iter_concept<OptsV>());
    using iterator         = detail::any_iterator<iterator_concept, ElementT, RefT, RValueRefT, DiffT>;
    using sentinel         = std::default_sentinel_t;
    using size_type        = std::make_unsigned_t<DiffT>;

    using interface_type = detail::view_interface<iterator_concept, ElementT, RefT, RValueRefT, DiffT>;
    // inplace storage sufficient for a vtable pointer and a std::vector<T>
    detail::intrusive_small_ptr<interface_type, 4 * sizeof(void*)> view_ptr;

    template <class RangeT>
    static consteval auto get_in_place_adaptor_type() {
        return std::in_place_type<
            detail::view_adaptor<iterator_concept, ElementT, RefT, RValueRefT, DiffT, std::views::all_t<RangeT>>>;
    }

  public:
    // [range.any.ctor]
    template <class RangeT>
        requires(not std::same_as<std::remove_cvref_t<RangeT>, any_view> and
                 ext_any_compatible_viewable_range<RangeT, any_view>)
    constexpr any_view(RangeT&& range)
        : view_ptr(get_in_place_adaptor_type<RangeT>(), std::views::all(std::forward<RangeT>(range))) {}

    constexpr any_view() noexcept : any_view(detail::default_view<ElementT, RefT, RValueRefT, DiffT>{}) {}

    constexpr any_view(const any_view&)
        requires(bool(OptsV& any_view_options::copyable))
    = default;

    constexpr any_view(any_view&& other) noexcept : any_view() { swap(other); }

    constexpr any_view& operator=(const any_view&)
        requires(bool(OptsV& any_view_options::copyable))
    = default;

    constexpr any_view& operator=(any_view&& other) noexcept {
        view_ptr = {get_in_place_adaptor_type<detail::default_view<ElementT, RefT, RValueRefT, DiffT>>(),
                    detail::default_view<ElementT, RefT, RValueRefT, DiffT>{}};
        swap(other);
        return *this;
    }

    constexpr ~any_view() = default;

    // [range.any.access]
    [[nodiscard]] constexpr iterator begin() { return view_ptr->begin(); }

    [[nodiscard]] constexpr sentinel end() { return std::default_sentinel; }

    [[nodiscard]] constexpr size_type size() const
        requires(bool(OptsV& any_view_options::sized))
    {
        return view_ptr->size();
    }

    // [range.any.swap]
    constexpr void swap(any_view& other) noexcept { std::swap(view_ptr, other.view_ptr); }

    constexpr friend void swap(any_view& lhs, any_view& rhs) noexcept { return lhs.swap(rhs); }
};

} // namespace beman::any_view

template <class ElementT, beman::any_view::any_view_options OptsV, class RefT, class RValueRefT, class DiffT>
inline constexpr bool
    std::ranges::enable_borrowed_range<beman::any_view::any_view<ElementT, OptsV, RefT, RValueRefT, DiffT>> =
        bool(OptsV & beman::any_view::any_view_options::borrowed);

#endif // BEMAN_ANY_VIEW_ANY_VIEW_HPP
