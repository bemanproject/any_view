// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_VIEW_POLICIES_HPP
#define BEMAN_ANY_VIEW_DETAIL_VIEW_POLICIES_HPP

#include <beman/any_view/any_view_options.hpp>
#include <beman/any_view/detail/iterator_adaptor.hpp>
#include <beman/any_view/detail/iterator_policies.hpp>
#include <beman/any_view/detail/policies.hpp>
#include <beman/any_view/detail/small_polymorphic.hpp>
#include <beman/any_view/detail/small_storage.hpp>
#include <beman/any_view/detail/vtable.hpp>
#include <beman/any_view/reserve_hint.hpp>

#include <iterator>

namespace beman::any_view::detail {

template <class RefT, class RValueRefT, class DiffT>
struct vtable_policy : nullary_policy {
    template <class ViewAdaptorT>
    [[nodiscard]] static constexpr const vtable<input_policy<RefT, RValueRefT>, iterator_storage>* fn() noexcept {
        constexpr auto options = ViewAdaptorT::options;
        using view_type        = typename ViewAdaptorT::view_type;
        return std::addressof(vtable_for<iterator_policy<RefT, RValueRefT, DiffT, options>,
                                         iterator_storage,
                                         iterator_adaptor_for<view_type>>);
    }
};

template <class RefT, class RValueRefT, class DiffT>
struct begin_policy : unary_policy {
    template <class ViewAdaptorT>
    [[nodiscard]] static constexpr iterator_storage fn(ViewAdaptorT& adaptor) {
        using view_type = typename ViewAdaptorT::view_type;
        return iterator_storage{iterator_adaptor_for<view_type>{
            .iterator = std::ranges::begin(adaptor.view),
            .sentinel = std::ranges::end(adaptor.view),
        }};
    }
};

template <class DiffT>
struct reserve_hint_policy : unary_policy {
    template <class ViewAdaptorT>
    [[nodiscard]] static constexpr std::make_unsigned_t<DiffT> fn(const ViewAdaptorT& adaptor) {
        return beman::any_view::reserve_hint(adaptor.view);
    }
};

template <class DiffT>
struct size_policy : unary_policy {
    template <class ViewAdaptorT>
    [[nodiscard]] static constexpr std::make_unsigned_t<DiffT> fn(const ViewAdaptorT& adaptor) {
        return std::ranges::size(adaptor.view);
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
using approximately_sized_policy = inherit<unsized_policy, reserve_hint_policy<DiffT>>;

template <class DiffT>
using sized_policy = inherit<approximately_sized_policy<DiffT>, size_policy<DiffT>>;

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

    if constexpr (flag_is_set<OptsV, sized>) {
        return sized_policy<DiffT>{};
    } else if constexpr (flag_is_set<OptsV, approximately_sized>) {
        return approximately_sized_policy<DiffT>{};
    } else {
        return unsized_policy{};
    }
}

template <class RefT, class RValueRefT, class DiffT, any_view_options OptsV>
using polymorphic_view = small_polymorphic<view_storage,
                                           decltype(get_copyable_policy<RefT, RValueRefT, DiffT, OptsV>()),
                                           decltype(get_sized_policy<DiffT, OptsV>())>;

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_VIEW_POLICIES_HPP
