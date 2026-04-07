// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_ANY_VIEW_HPP
#define BEMAN_ANY_VIEW_ANY_VIEW_HPP

#include <beman/any_view/concepts.hpp>
#include <beman/any_view/detail/iterator.hpp>
#include <beman/any_view/detail/polymorphic_view.hpp>

namespace beman::any_view {
namespace detail {

template <class>
struct is_any_view : std::false_type {};

template <class T>
struct rvalue_ref {
    using type = T;
};

template <class T>
struct rvalue_ref<T&> {
    using type = T&&;
};

template <class T>
using rvalue_ref_t = typename rvalue_ref<T>::type;

} // namespace detail

template <class ElementT,
          any_view_options OptsV = any_view_options::input,
          class RefT             = ElementT&,
          class RValueRefT       = detail::rvalue_ref_t<RefT>,
          class DiffT            = std::ptrdiff_t>
class any_view : public std::ranges::view_interface<any_view<ElementT, OptsV, RefT, RValueRefT, DiffT>> {
    static_assert(detail::flag_is_set<OptsV, any_view_options::input>, "any_view must model input_range");

    template <class OtherElementT,
              any_view_options OtherOptsV,
              class OtherRefT,
              class OtherRValueRefT,
              class OtherDiffT>
    friend class any_view;

    static constexpr bool approximately_sized = detail::flag_is_set<OptsV, any_view_options::approximately_sized>;
    static constexpr bool sized               = detail::flag_is_set<OptsV, any_view_options::sized>;
    static constexpr bool contiguous_bitor_sized =
        detail::flag_is_set<OptsV, any_view_options::contiguous | any_view_options::sized>;
    static constexpr bool copyable = detail::flag_is_set<OptsV, any_view_options::copyable>;

    using iterator         = std::conditional_t<contiguous_bitor_sized,
                                                std::counted_iterator<std::add_pointer_t<RefT>>,
                                                detail::iterator<ElementT, RefT, RValueRefT, DiffT, OptsV>>;
    using polymorphic_type = detail::polymorphic_view<RefT, RValueRefT, DiffT, OptsV>;
    using sentinel         = std::default_sentinel_t;
    using size_type        = std::make_unsigned_t<DiffT>;

    template <detail::policy PolicyT>
    static constexpr auto dispatch = detail::dispatch<PolicyT, polymorphic_type>;

    polymorphic_type poly;

    template <class RangeT, bool IsAnyView>
    constexpr any_view(RangeT&& range, std::bool_constant<IsAnyView>)
        : poly(detail::view_adaptor<std::views::all_t<RangeT>, OptsV>{
              .view = std::views::all(std::forward<RangeT>(range))}) {}

    template <class RangeT>
    static consteval bool is_noexcept() {
        return detail::is_any_view<std::remove_cvref_t<RangeT>>::value and requires {
            { polymorphic_type(std::declval<RangeT>().poly) } noexcept;
        };
    }

    template <class RangeT>
        requires std::constructible_from<polymorphic_type, decltype(std::declval<RangeT>().poly)>
    constexpr any_view(RangeT&& range, std::true_type) noexcept(is_noexcept<RangeT>())
        : poly(std::forward<RangeT>(range).poly) {}

  public:
    // [range.any.ctor]
    template <class RangeT>
        requires detail::different_from<RangeT, any_view> and
                 ext_any_compatible_range<RangeT, RefT, RValueRefT, DiffT, OptsV>
    constexpr any_view(RangeT&& range) noexcept(is_noexcept<RangeT>())
        : any_view(std::forward<RangeT>(range), detail::is_any_view<std::remove_cvref_t<RangeT>>{}) {
        static_assert(std::ranges::viewable_range<RangeT>, "range must be viewable");
        if constexpr (copyable) {
            static_assert(std::copyable<std::views::all_t<RangeT>>,
                          "range must be convertible to copyable view if any_view is copyable");
        }
    }

    constexpr any_view() noexcept : any_view(detail::default_view<ElementT, RefT, RValueRefT, DiffT>{}) {}

    constexpr any_view(const any_view&)
        requires copyable
    = default;

    constexpr any_view(any_view&& other) noexcept : any_view() { swap(other); }

    constexpr any_view& operator=(const any_view&)
        requires copyable
    = default;

    constexpr any_view& operator=(any_view&& other) noexcept {
        any_view(std::move(other)).swap(*this);
        return *this;
    }

    constexpr ~any_view() = default;

    // [range.any.access]
    [[nodiscard]] constexpr iterator begin() {
        using derived_vtable_type =
            detail::vtable<detail::iterator_policy<RefT, RValueRefT, DiffT, OptsV>, detail::iterator_storage>;

        const auto get_poly = [this] {
            const auto base_vtable_ptr = dispatch<detail::vtable_policy<RefT, RValueRefT, DiffT>>(poly);

            return detail::polymorphic_iterator<RefT, RValueRefT, DiffT, OptsV>{
                [this] { return dispatch<detail::begin_policy<RefT, RValueRefT, DiffT>>(poly); },
                static_cast<const derived_vtable_type*>(base_vtable_ptr)};
        };

        if constexpr (contiguous_bitor_sized) {
            return iterator{std::to_address(detail::iterator<ElementT, RefT, RValueRefT, DiffT, OptsV>{get_poly}),
                            static_cast<DiffT>(size())};
        } else {
            return iterator{get_poly};
        }
    }

    [[nodiscard]] constexpr sentinel end() { return std::default_sentinel; }

    [[nodiscard]] constexpr size_type size() const
        requires sized
    {
        return dispatch<detail::size_policy<DiffT>>(poly);
    }

    [[nodiscard]] constexpr size_type reserve_hint() const
        requires approximately_sized
    {
        return dispatch<detail::reserve_hint_policy<DiffT>>(poly);
    }

    // [range.any.swap]
    constexpr void swap(any_view& other) noexcept { std::swap(poly, other.poly); }

    constexpr friend void swap(any_view& lhs, any_view& rhs) noexcept { return lhs.swap(rhs); }
};

template <class ElementT, any_view_options OptsV, class RefT, class RValueRefT, class DiffT>
struct detail::is_any_view<any_view<ElementT, OptsV, RefT, RValueRefT, DiffT>> : std::true_type {};

} // namespace beman::any_view

template <class ElementT, beman::any_view::any_view_options OptsV, class RefT, class RValueRefT, class DiffT>
inline constexpr bool
    std::ranges::enable_borrowed_range<beman::any_view::any_view<ElementT, OptsV, RefT, RValueRefT, DiffT>> =
        (OptsV & beman::any_view::any_view_options::borrowed) == beman::any_view::any_view_options::borrowed;

#endif // BEMAN_ANY_VIEW_ANY_VIEW_HPP
