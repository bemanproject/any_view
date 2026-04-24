// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_ANY_VIEW_HPP
#define BEMAN_ANY_VIEW_ANY_VIEW_HPP

#include <beman/any_view/concepts.hpp>
#include <beman/any_view/detail/iterator.hpp>
#include <beman/any_view/detail/polymorphic_view.hpp>

namespace beman::any_view {
namespace detail {

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
    static constexpr bool contiguous_and_sized =
        detail::flag_is_set<OptsV, any_view_options::contiguous | any_view_options::sized>;
    static constexpr bool copyable = detail::flag_is_set<OptsV, any_view_options::copyable>;

    using uncounted_iterator = detail::iterator<ElementT, RefT, RValueRefT, DiffT, OptsV>;
    using iterator =
        std::conditional_t<contiguous_and_sized, std::counted_iterator<std::add_pointer_t<RefT>>, uncounted_iterator>;
    using value_type       = std::remove_cv_t<ElementT>;
    using polymorphic_type = detail::polymorphic_view<value_type, RefT, RValueRefT, DiffT, OptsV>;
    using sentinel         = std::default_sentinel_t;
    using size_type        = std::make_unsigned_t<DiffT>;

    template <detail::protocol ProtocolT>
    static constexpr auto dispatch = detail::dispatch<ProtocolT, polymorphic_type>;

    template <std::ranges::view ViewT>
    using adaptor_for = detail::view_adaptor<ViewT, OptsV>;

    static constexpr polymorphic_type make_default() {
        return polymorphic_type(
            std::in_place_type<adaptor_for<detail::default_view<ElementT, RefT, RValueRefT, DiffT>>>);
    }

    polymorphic_type poly{make_default()};

    constexpr const polymorphic_type& polymorphic() const& noexcept { return poly; }

    constexpr polymorphic_type polymorphic() && noexcept { return std::exchange(poly, make_default()); }

    // type erase
    template <class RangeT, class InPlaceTypeT>
    constexpr any_view(RangeT&& range, InPlaceTypeT)
        : poly(adaptor_for<std::views::all_t<RangeT>>{.view = std::views::all(std::forward<RangeT>(range))}) {}

    // upcast
    template <class RangeT, class OtherElementT, any_view_options OtherOptsV>
        requires std::constructible_from<polymorphic_type, decltype(std::declval<RangeT>().polymorphic())>
    constexpr any_view(
        RangeT&& range,
        std::in_place_type_t<any_view<OtherElementT,
                                      OtherOptsV,
                                      RefT,
                                      RValueRefT,
                                      DiffT>>) noexcept(noexcept(polymorphic_type(std::forward<RangeT>(range)
                                                                                      .polymorphic())))
        : poly(std::forward<RangeT>(range).polymorphic()) {}

    template <detail::protocol ProtocolT, detail::storage StorageT>
    using witness_for = detail::witness<typename ProtocolT::template protocol_for<OptsV>, StorageT>;

    template <detail::polymorphic PolyT, class GetStorageT>
        requires std::is_invocable_r_v<detail::view_storage, GetStorageT>
    [[nodiscard]] constexpr PolyT make_const(GetStorageT get_storage) const {
        using reference           = detail::const_reference_t<value_type, RefT>;
        using rvalue_reference    = detail::const_reference_t<value_type, RValueRefT>;
        using const_copyable_type = detail::const_copyable_witness_t<reference, rvalue_reference, DiffT>;
        using const_sized_type    = detail::const_sized_witness_t<reference, rvalue_reference, DiffT>;

        const auto const_copyable_witness_ptr =
            static_cast<const witness_for<const_copyable_type, detail::view_storage>*>(
                dispatch<const_copyable_type>(poly));
        const auto const_sized_witness_ptr =
            static_cast<const witness_for<const_sized_type, detail::view_storage>*>(dispatch<const_sized_type>(poly));

        return PolyT(get_storage, const_copyable_witness_ptr, const_sized_witness_ptr);
    }

    template <detail::polymorphic PolyT>
    [[nodiscard]] constexpr explicit operator PolyT() const& {
        return make_const<PolyT>([this] { return dispatch<detail::copy_t<detail::view_storage>>(poly); });
    }

    template <detail::polymorphic PolyT>
    [[nodiscard]] constexpr explicit operator PolyT() && noexcept {
        return make_const<PolyT>([this] {
            return poly.entry(detail::move_t<detail::view_storage>{})(std::move(*this).polymorphic().get());
        });
    }

    // const_cast
    template <class RangeT, class OtherElementT, any_view_options OtherOptsV, class OtherRefT, class OtherRValueRefT>
        requires detail::const_capable<std::remove_cv_t<OtherElementT>, OtherRefT, OtherRValueRefT>
    constexpr any_view(
        RangeT&& range,
        std::in_place_type_t<any_view<OtherElementT,
                                      OtherOptsV,
                                      OtherRefT,
                                      OtherRValueRefT,
                                      DiffT>>) noexcept(noexcept(polymorphic_type(std::forward<RangeT>(range))))
        : poly(std::forward<RangeT>(range)) {}

  public:
    // [range.any.ctor]
    template <class RangeT>
        requires detail::different_from<RangeT, any_view> and
                 ext_any_compatible_range<RangeT, RefT, RValueRefT, DiffT, OptsV>
    constexpr any_view(RangeT&& range) noexcept(noexcept(any_view(std::forward<RangeT>(range),
                                                                  std::in_place_type<std::remove_cvref_t<RangeT>>)))
        : any_view(std::forward<RangeT>(range), std::in_place_type<std::remove_cvref_t<RangeT>>) {
        static_assert(std::ranges::viewable_range<RangeT>, "range must be viewable");
        if constexpr (copyable) {
            static_assert(std::copyable<std::views::all_t<RangeT>>,
                          "range must be convertible to copyable view if any_view is copyable");
        }
    }

    constexpr any_view() noexcept = default;

    constexpr any_view(const any_view&)
        requires copyable
    = default;

    constexpr any_view(any_view&& other) noexcept { swap(other); }

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
        using protocol_type = detail::iterator_witness_t<RefT, RValueRefT, DiffT>;

        const auto get_storage = [this] { return dispatch<detail::begin_t>(poly); };
        const auto witness_ptr =
            static_cast<const witness_for<protocol_type, detail::iterator_storage>*>(dispatch<protocol_type>(poly));

        if constexpr (contiguous_and_sized) {
            const auto to_address = &detail::witness<detail::cache_t<RefT>, detail::iterator_storage>::entry;
            return iterator{(witness_ptr->*to_address)(get_storage()), static_cast<DiffT>(size())};
        } else {
            return iterator{[=] {
                return detail::polymorphic_iterator<RefT, RValueRefT, DiffT, OptsV>{get_storage, witness_ptr};
            }};
        }
    }

    [[nodiscard]] constexpr sentinel end() { return std::default_sentinel; }

    [[nodiscard]] constexpr size_type size() const
        requires sized
    {
        return dispatch<detail::reserve_hint_t<DiffT>>(poly);
    }

    [[nodiscard]] constexpr size_type reserve_hint() const
        requires approximately_sized
    {
        return dispatch<detail::reserve_hint_t<DiffT>>(poly);
    }

    // [range.any.swap]
    constexpr void swap(any_view& other) noexcept { std::swap(poly, other.poly); }

    constexpr friend void swap(any_view& lhs, any_view& rhs) noexcept { return lhs.swap(rhs); }
};

} // namespace beman::any_view

template <class ElementT, beman::any_view::any_view_options OptsV, class RefT, class RValueRefT, class DiffT>
inline constexpr bool
    std::ranges::enable_borrowed_range<beman::any_view::any_view<ElementT, OptsV, RefT, RValueRefT, DiffT>> =
        beman::any_view::detail::flag_is_set<OptsV, beman::any_view::any_view_options::borrowed>;

#endif // BEMAN_ANY_VIEW_ANY_VIEW_HPP
