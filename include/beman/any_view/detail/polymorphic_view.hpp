// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_POLYMORPHIC_VIEW_HPP
#define BEMAN_ANY_VIEW_DETAIL_POLYMORPHIC_VIEW_HPP

#include <beman/any_view/detail/polymorphic_iterator.hpp>
#include <beman/any_view/reserve_hint.hpp>

namespace beman::any_view::detail {

template <class RefT, class RValueRefT, class DiffT>
struct vtable_policy : nullary_policy {
    template <not_adaptor>
    static const vtable<input_policy<RefT, RValueRefT>, iterator_storage>* fn() noexcept;

    template <adaptor ViewAdaptorT>
    [[nodiscard]] static constexpr const vtable<input_policy<RefT, RValueRefT>, iterator_storage>* fn() noexcept {
        constexpr auto options = ViewAdaptorT::options;
        using adaptor_type     = typename ViewAdaptorT::adaptor_type;
        return std::addressof(
            vtable_for<iterator_policy<RefT, RValueRefT, DiffT, options>, iterator_storage, adaptor_type>);
    }
};

template <class RefT, class RValueRefT, class DiffT>
struct begin_policy : unary_policy {
    template <not_adaptor T>
    static iterator_storage fn(T& self);

    template <adaptor ViewAdaptorT>
    [[nodiscard]] static constexpr iterator_storage fn(ViewAdaptorT& adaptor) {
        using adaptor_type = typename ViewAdaptorT::adaptor_type;
        return iterator_storage{adaptor_type{
            .iterator = std::ranges::begin(adaptor.view),
            .sentinel = std::ranges::end(adaptor.view),
        }};
    }
};

template <class DiffT>
struct reserve_hint_policy : unary_policy {
    template <not_adaptor T>
    static std::make_unsigned_t<DiffT> fn(const T& self);

    template <adaptor ViewAdaptorT>
    [[nodiscard]] static constexpr std::make_unsigned_t<DiffT> fn(const ViewAdaptorT& adaptor) {
        return beman::any_view::reserve_hint(adaptor.view);
    }
};

// inplace storage sufficient for a std::vector<T>
using view_storage = small_storage<3 * sizeof(void*)>;

template <class RefT, class RValueRefT, class DiffT>
using uncopyable_policy = inherit<move_policy<view_storage>,
                                  destroy_policy<view_storage>,
                                  vtable_policy<RefT, RValueRefT, DiffT>,
                                  begin_policy<RefT, RValueRefT, DiffT>>;

template <class RefT, class RValueRefT, class DiffT>
using copyable_policy = inherit<uncopyable_policy<RefT, RValueRefT, DiffT>, copy_policy<view_storage>>;

using unsized_policy = inherit<>;

template <class DiffT>
using sized_policy = inherit<unsized_policy, reserve_hint_policy<DiffT>>;

template <class RefT, class RValueRefT, class DiffT, any_view_options OptsV>
consteval auto get_copyable_policy() {
    using enum any_view_options;

    if constexpr (flag_is_set<OptsV, copyable>) {
        return copyable_policy<RefT, RValueRefT, DiffT>{};
    } else {
        return uncopyable_policy<RefT, RValueRefT, DiffT>{};
    }
}

template <class DiffT, any_view_options OptsV>
consteval auto get_sized_policy() {
    using enum any_view_options;

    if constexpr (flag_is_set<OptsV, approximately_sized>) {
        return sized_policy<DiffT>{};
    } else {
        return unsized_policy{};
    }
}

template <class RefT, class RValueRefT, class DiffT, any_view_options OptsV>
using polymorphic_view = basic_polymorphic<view_storage,
                                           decltype(get_copyable_policy<RefT, RValueRefT, DiffT, OptsV>()),
                                           decltype(get_sized_policy<DiffT, OptsV>())>;

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_POLYMORPHIC_VIEW_HPP
