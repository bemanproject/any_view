// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_POLYMORPHIC_VIEW_HPP
#define BEMAN_ANY_VIEW_DETAIL_POLYMORPHIC_VIEW_HPP

#include <beman/any_view/detail/polymorphic_iterator.hpp>
#include <beman/any_view/reserve_hint.hpp>

namespace beman::any_view::detail {

template <class RefT, class RValueRefT, class DiffT>
struct iterator_witness_t : nullary_capability {
    template <any_view_options OptsV>
    using capabilities_for = iterator_capabilities<RefT, RValueRefT, DiffT, OptsV>;

    using witness_type = witness<capabilities_for<any_view_options::input>, iterator_storage>;

    template <not_adaptor>
    static const witness_type* fn() noexcept;

    template <adaptor ViewAdaptorT>
    [[nodiscard]] static constexpr const witness_type* fn() noexcept {
        using capability_type = capabilities_for<ViewAdaptorT::options>;
        using adaptor_type    = typename ViewAdaptorT::adaptor_type;
        return std::addressof(witness_for<capability_type, iterator_storage, adaptor_type>);
    }
};

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

template <class ValueT, class RefT, class RValueRefT, class DiffT, any_view_options OptsV, class... ConstRefTs>
consteval auto get_copyable_capabilities();

template <class ConstRefT, class ConstRValueRefT, class DiffT>
struct const_copyable_witness_t : nullary_capability {
    template <any_view_options OptsV>
    using capabilities_for = decltype(get_copyable_capabilities<void, ConstRefT, ConstRValueRefT, DiffT, OptsV>());

    using witness_type = witness<capabilities_for<any_view_options{}>, view_storage>;

    template <not_adaptor>
    static const witness_type* fn() noexcept;

    template <adaptor ViewAdaptorT>
    [[nodiscard]] static constexpr const witness_type* fn() noexcept {
        using capability_type = capabilities_for<ViewAdaptorT::options>;
        return std::addressof(witness_for<capability_type, view_storage, ViewAdaptorT>);
    }
};

template <class DiffT, any_view_options OptsV>
consteval auto get_sized_capabilities();

template <class ConstRefT, class ConstRValueRefT, class DiffT>
struct const_sized_witness_t : nullary_capability {
    template <any_view_options OptsV>
    using capabilities_for = decltype(get_sized_capabilities<DiffT, OptsV>());

    using witness_type = witness<capabilities_for<any_view_options{}>, view_storage>;

    template <not_adaptor>
    static const witness_type* fn() noexcept;

    template <adaptor ViewAdaptorT>
    [[nodiscard]] static constexpr const witness_type* fn() noexcept {
        using capability_type = capabilities_for<ViewAdaptorT::options>;
        return std::addressof(witness_for<capability_type, view_storage, ViewAdaptorT>);
    }
};

template <class ValueT, std::common_reference_with<const ValueT&&> RefT>
using const_reference_t = std::common_reference_t<const ValueT&&, RefT>;

template <class ValueT, class RefT, class RValueRefT>
concept const_capable = not std::same_as<const_reference_t<ValueT, RefT>, RefT> or
                        not std::same_as<const_reference_t<ValueT, RValueRefT>, RValueRefT>;

template <class DiffT, class...>
struct const_capabilities : inherit<> {};

template <class ConstRefT, class ConstRValueRefT, class DiffT>
struct const_capabilities<ConstRefT, ConstRValueRefT, DiffT>
    : inherit<const_copyable_witness_t<ConstRefT, ConstRValueRefT, DiffT>,
              const_sized_witness_t<ConstRefT, ConstRValueRefT, DiffT>> {};

template <class RefT, class RValueRefT, class DiffT, class... ConstRefTs>
struct uncopyable_capabilities : inherit<move_t<view_storage>,
                                         destroy_t<view_storage>,
                                         iterator_witness_t<RefT, RValueRefT, DiffT>,
                                         begin_t,
                                         const_capabilities<ConstRefTs..., DiffT>> {};

template <class RefT, class RValueRefT, class DiffT, class... ConstRefTs>
struct copyable_capabilities
    : inherit<uncopyable_capabilities<RefT, RValueRefT, DiffT, ConstRefTs...>, copy_t<view_storage>> {};

struct unsized_capabilities : inherit<> {};

template <class DiffT>
struct sized_capabilities : inherit<unsized_capabilities, reserve_hint_t<DiffT>> {};

template <class ValueT, class RefT, class RValueRefT, class DiffT, any_view_options OptsV, class... ConstRefTs>
consteval auto get_copyable_capabilities() {
    using enum any_view_options;

    if constexpr (const_capable<ValueT, RefT, RValueRefT> and sizeof...(ConstRefTs) == 0) {
        return get_copyable_capabilities<ValueT,
                                         RefT,
                                         RValueRefT,
                                         DiffT,
                                         OptsV,
                                         const_reference_t<ValueT, RefT>,
                                         const_reference_t<ValueT, RValueRefT>>();
    } else if constexpr (flag_is_set<OptsV, copyable>) {
        return copyable_capabilities<RefT, RValueRefT, DiffT, ConstRefTs...>{};
    } else {
        return uncopyable_capabilities<RefT, RValueRefT, DiffT, ConstRefTs...>{};
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

template <class ValueT, class RefT, class RValueRefT, class DiffT, any_view_options OptsV>
using polymorphic_view =
    basic_polymorphic<view_storage,
                      decltype(get_copyable_capabilities<ValueT, RefT, RValueRefT, DiffT, OptsV>()),
                      decltype(get_sized_capabilities<DiffT, OptsV>())>;

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_POLYMORPHIC_VIEW_HPP
