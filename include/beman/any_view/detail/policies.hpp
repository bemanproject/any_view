// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_POLICIES_HPP
#define BEMAN_ANY_VIEW_DETAIL_POLICIES_HPP

#include <beman/any_view/detail/adaptor_base.hpp>
#include <beman/any_view/detail/vtable.hpp>

#include <typeinfo>
#include <utility>

namespace beman::any_view::detail {

struct nullary_policy {};

struct unary_policy {};

struct symmetric_binary_policy {};

// bindings for nullary policies
template <std::derived_from<nullary_policy> PolicyT, class AdaptorT, class T, class RetT, class... ArgsT>
struct impl<binding<PolicyT, AdaptorT>, T, RetT(ArgsT...)> {
    [[nodiscard]] static constexpr RetT fn(ArgsT... args) {
        return detail::fn<PolicyT, AdaptorT>(std::forward<ArgsT>(args)...);
    }
};

// bindings for noexcept nullary policies
template <std::derived_from<nullary_policy> PolicyT, class AdaptorT, class T, class RetT, class... ArgsT>
struct impl<binding<PolicyT, AdaptorT>, T, RetT(ArgsT...) noexcept> {
    [[nodiscard]] static constexpr RetT fn(ArgsT... args) noexcept {
        return detail::fn<PolicyT, AdaptorT>(std::forward<ArgsT>(args)...);
    }
};

template <class StorageT>
struct move_policy : nullary_policy {
    template <not_adaptor>
    static StorageT fn(StorageT&& source) noexcept;

    template <adaptor AdaptorT>
    [[nodiscard]] static constexpr StorageT fn(StorageT&& source) noexcept {
        return {std::move(source), std::in_place_type<AdaptorT>};
    }
};

template <class StorageT>
struct copy_policy : nullary_policy {
    template <not_adaptor>
    static StorageT fn(const StorageT& source);

    template <adaptor AdaptorT>
    [[nodiscard]] static constexpr StorageT fn(const StorageT& source) {
        return {source, std::in_place_type<AdaptorT>};
    }
};

template <class StorageT>
struct destroy_policy : nullary_policy {
    template <not_adaptor>
    static void fn(StorageT& source) noexcept;

    template <adaptor AdaptorT>
    static constexpr void fn(StorageT& source) noexcept {
        source.destroy(std::in_place_type<AdaptorT>);
    }
};

struct type_policy : nullary_policy {
    template <class T>
    [[nodiscard]] static constexpr const std::type_info& fn() noexcept {
        return typeid(T);
    }
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_POLICIES_HPP
