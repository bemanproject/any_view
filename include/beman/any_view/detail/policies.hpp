// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_POLICIES_HPP
#define BEMAN_ANY_VIEW_DETAIL_POLICIES_HPP

#include <beman/any_view/detail/vtable.hpp>

#include <typeinfo>
#include <type_traits>
#include <utility>

namespace beman::any_view::detail {

struct nullary_policy : policy_base {};

struct unary_policy : policy_base {};

struct symmetric_binary_policy : policy_base {};

template <class T>
concept nullary = policy<T> and std::derived_from<T, nullary_policy>;

template <class T>
concept unary = policy<T> and std::derived_from<T, unary_policy>;

template <class T>
concept symmetric_binary = policy<T> and std::derived_from<T, symmetric_binary_policy>;

// bindings for nullary policies
template <nullary NullaryT, adaptor AdaptorT, storage StorageT, class RetT, class... ArgsT>
struct impl<binding<NullaryT, AdaptorT>, StorageT, RetT(ArgsT...)> {
    [[nodiscard]] static constexpr RetT fn(ArgsT... args) {
        return dispatch<NullaryT, AdaptorT>(std::forward<ArgsT>(args)...);
    }
};

// bindings for noexcept nullary policies
template <nullary NullaryT, adaptor AdaptorT, storage StorageT, class RetT, class... ArgsT>
struct impl<binding<NullaryT, AdaptorT>, StorageT, RetT(ArgsT...) noexcept> {
    [[nodiscard]] static constexpr RetT fn(ArgsT... args) noexcept {
        return dispatch<NullaryT, AdaptorT>(std::forward<ArgsT>(args)...);
    }
};

// bindings for unary policies
template <unary UnaryT, adaptor AdaptorT, storage StorageT, class RetT, class SelfT, class... ArgsT>
struct impl<binding<UnaryT, AdaptorT>, StorageT, RetT(SelfT&, ArgsT...)> {
    [[nodiscard]] static constexpr RetT fn(SelfT& self, ArgsT... args) {
        return dispatch<UnaryT, AdaptorT>(unchecked_get<AdaptorT>(self), std::forward<ArgsT>(args)...);
    }
};

struct type_policy : nullary_policy {
    template <class T>
    [[nodiscard]] static constexpr const std::type_info& fn() noexcept {
        return typeid(T);
    }
};

// bindings for symmetric binary policies
template <symmetric_binary SymmetricBinaryT, adaptor AdaptorT, storage StorageT, class RetT>
struct impl<binding<SymmetricBinaryT, AdaptorT>,
            StorageT,
            RetT(const StorageT&, const StorageT&, const std::type_info&)> {
    [[nodiscard]] static constexpr RetT
    fn(const StorageT& self, const StorageT& other, const std::type_info& other_type) {
        const auto& self_type = dispatch<type_policy, AdaptorT>();

        // std::type_info::operator== is not constexpr until C++23
        if (std::is_constant_evaluated() ? &self_type != &other_type : self_type != other_type) {
            return SymmetricBinaryT::default_value();
        }

        return dispatch<SymmetricBinaryT, AdaptorT>(unchecked_get<AdaptorT>(self), unchecked_get<AdaptorT>(other));
    }
};

template <policy PolicyT>
struct entry_t {
    explicit entry_t() = default;
};

template <policy PolicyT>
inline constexpr entry_t<PolicyT> entry{};

// dispatches to bindings for nullary policies
template <nullary NullaryT, polymorphic PolyT, class RetT, class... ArgsT>
struct impl<NullaryT, PolyT, RetT(ArgsT...)> {
    [[nodiscard]] static constexpr RetT fn(const PolyT& self, ArgsT... args) {
        return self[entry<NullaryT>](std::forward<ArgsT>(args)...);
    }
};

// dispatches to bindings for noexcept nullary policies
template <nullary NullaryT, polymorphic PolyT, class RetT, class... ArgsT>
struct impl<NullaryT, PolyT, RetT(ArgsT...) noexcept> {
    [[nodiscard]] static constexpr RetT fn(const PolyT& self, ArgsT... args) noexcept {
        return self[entry<NullaryT>](std::forward<ArgsT>(args)...);
    }
};

// dispatches to bindings for unary policies
template <unary UnaryT, polymorphic PolyT, class RetT, class SelfT, class... ArgsT>
struct impl<UnaryT, PolyT, RetT(SelfT&, ArgsT...)> {
    [[nodiscard]] static constexpr RetT fn(SelfT& self, ArgsT... args) {
        return self[entry<UnaryT>](self.get(), std::forward<ArgsT>(args)...);
    }
};

// dispatches to bindings for symmetric binary policies
// requires type_policy to be implemented
template <symmetric_binary SymmetricBinaryT, polymorphic PolyT, class RetT>
struct impl<SymmetricBinaryT, PolyT, RetT(const PolyT&, const PolyT&)> {
    [[nodiscard]] static constexpr RetT fn(const PolyT& self, const PolyT& other) {
        return self[entry<SymmetricBinaryT>](self.get(), other.get(), other[entry<type_policy>]());
    }
};

template <storage StorageT>
struct move_policy : nullary_policy {
    template <not_adaptor>
    static StorageT fn(StorageT&& source) noexcept;

    template <adaptor AdaptorT>
    [[nodiscard]] static constexpr StorageT fn(StorageT&& source) noexcept {
        return {std::move(source), std::in_place_type<AdaptorT>};
    }
};

template <storage StorageT>
struct copy_policy : nullary_policy {
    template <not_adaptor>
    static StorageT fn(const StorageT& source);

    template <adaptor AdaptorT>
    [[nodiscard]] static constexpr StorageT fn(const StorageT& source) {
        return {source, std::in_place_type<AdaptorT>};
    }
};

template <storage StorageT>
struct destroy_policy : nullary_policy {
    template <not_adaptor>
    static void fn(StorageT& source) noexcept;

    template <adaptor AdaptorT>
    static constexpr void fn(StorageT& source) noexcept {
        destroy_as<AdaptorT>(source);
    }
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_POLICIES_HPP
