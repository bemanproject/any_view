// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_POLYMORPHIC_VIEW_HPP
#define BEMAN_ANY_VIEW_DETAIL_POLYMORPHIC_VIEW_HPP

#include <beman/any_view/detail/polymorphic_iterator.hpp>
#include <beman/any_view/reserve_hint.hpp>

namespace beman::any_view::detail {

template <class RefT, class RValueRefT, class DiffT>
struct iterator_vtable_t : nullary_capability {
    using vtable_type = vtable<input_capabilities<RefT, RValueRefT>, iterator_storage>;

    template <not_adaptor>
    static const vtable_type* fn() noexcept;

    template <adaptor ViewAdaptorT>
    [[nodiscard]] static constexpr const vtable_type* fn() noexcept {
        constexpr auto options = ViewAdaptorT::options;
        using capability_type  = iterator_capabilities<RefT, RValueRefT, DiffT, options>;
        using adaptor_type     = typename ViewAdaptorT::adaptor_type;
        return std::addressof(vtable_for<capability_type, iterator_storage, adaptor_type>);
    }
};

template <class RefT, class RValueRefT, class DiffT>
struct begin_t : unary_capability {
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
struct reserve_hint_t : unary_capability {
    using size_type = std::make_unsigned_t<DiffT>;

    template <not_adaptor T>
    static size_type fn(const T& self);

    template <adaptor ViewAdaptorT>
    [[nodiscard]] static constexpr size_type fn(const ViewAdaptorT& adaptor) {
        return beman::any_view::reserve_hint(adaptor.view);
    }
};

// inplace storage sufficient for a std::vector<T>
using view_storage = small_storage<3 * sizeof(void*)>;

template <class RefT, class RValueRefT, class DiffT>
using uncopyable_capabilities = inherit<move_t<view_storage>,
                                        destroy_t<view_storage>,
                                        iterator_vtable_t<RefT, RValueRefT, DiffT>,
                                        begin_t<RefT, RValueRefT, DiffT>>;

template <class RefT, class RValueRefT, class DiffT>
using copyable_capabilities = inherit<uncopyable_capabilities<RefT, RValueRefT, DiffT>, copy_t<view_storage>>;

using unsized_capabilities = inherit<>;

template <class DiffT>
using sized_capabilities = inherit<unsized_capabilities, reserve_hint_t<DiffT>>;

template <class RefT, class RValueRefT, class DiffT, any_view_options OptsV>
consteval auto get_copyable_capabilities() {
    using enum any_view_options;

    if constexpr (flag_is_set<OptsV, copyable>) {
        return copyable_capabilities<RefT, RValueRefT, DiffT>{};
    } else {
        return uncopyable_capabilities<RefT, RValueRefT, DiffT>{};
    }
}

template <class DiffT, any_view_options OptsV>
consteval auto get_sized_capabilities() {
    using enum any_view_options;

    if constexpr (flag_is_set<OptsV, approximately_sized>) {
        return sized_capabilities<DiffT>{};
    } else {
        return unsized_capabilities{};
    }
}

template <class RefT, class RValueRefT, class DiffT, any_view_options OptsV>
using polymorphic_view = basic_polymorphic<view_storage,
                                           decltype(get_copyable_capabilities<RefT, RValueRefT, DiffT, OptsV>()),
                                           decltype(get_sized_capabilities<DiffT, OptsV>())>;

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_POLYMORPHIC_VIEW_HPP
