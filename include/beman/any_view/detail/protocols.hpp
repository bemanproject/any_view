// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_PROTOCOLS_HPP
#define BEMAN_ANY_VIEW_DETAIL_PROTOCOLS_HPP

#include <beman/any_view/detail/witness.hpp>

#include <typeinfo>
#include <type_traits>
#include <utility>

namespace beman::any_view::detail {

struct nullary_protocol : protocol_base {};

struct unary_protocol : protocol_base {};

struct symmetric_binary_protocol : protocol_base {};

template <class T>
concept nullary = protocol<T> and std::derived_from<T, nullary_protocol>;

template <class T>
concept unary = protocol<T> and std::derived_from<T, unary_protocol>;

template <class T>
concept symmetric_binary = protocol<T> and std::derived_from<T, symmetric_binary_protocol>;

// bindings for nullary protocols
template <nullary NullaryT, adaptor AdaptorT, storage StorageT, class RetT, class... ArgsT>
struct bridge<thunk<NullaryT, AdaptorT>, StorageT, RetT(ArgsT...)> {
    [[nodiscard]] static constexpr RetT fn(ArgsT... args) {
        return dispatch<NullaryT, AdaptorT>(std::forward<ArgsT>(args)...);
    }
};

// bindings for noexcept nullary protocols
template <nullary NullaryT, adaptor AdaptorT, storage StorageT, class RetT, class... ArgsT>
struct bridge<thunk<NullaryT, AdaptorT>, StorageT, RetT(ArgsT...) noexcept> {
    [[nodiscard]] static constexpr RetT fn(ArgsT... args) noexcept {
        return dispatch<NullaryT, AdaptorT>(std::forward<ArgsT>(args)...);
    }
};

// bindings for unary protocols
template <unary UnaryT, adaptor AdaptorT, storage StorageT, class RetT, class SelfT, class... ArgsT>
struct bridge<thunk<UnaryT, AdaptorT>, StorageT, RetT(SelfT, ArgsT...)> {
    [[nodiscard]] static constexpr RetT fn(SelfT self, ArgsT... args) {
        return dispatch<UnaryT, AdaptorT>(std::forward<SelfT>(self).template unchecked_get<AdaptorT>(),
                                          std::forward<ArgsT>(args)...);
    }
};

// bindings for noexcept unary protocols
template <unary UnaryT, adaptor AdaptorT, storage StorageT, class RetT, class SelfT, class... ArgsT>
struct bridge<thunk<UnaryT, AdaptorT>, StorageT, RetT(SelfT, ArgsT...) noexcept> {
    [[nodiscard]] static constexpr RetT fn(SelfT self, ArgsT... args) noexcept {
        return dispatch<UnaryT, AdaptorT>(std::forward<SelfT>(self).template unchecked_get<AdaptorT>(),
                                          std::forward<ArgsT>(args)...);
    }
};

struct type_t : nullary_protocol {
    template <class T>
    [[nodiscard]] static constexpr const std::type_info& fn() noexcept {
        return typeid(T);
    }
};

// bindings for symmetric binary protocols
template <symmetric_binary SymmetricBinaryT, adaptor AdaptorT, storage StorageT, class RetT>
struct bridge<thunk<SymmetricBinaryT, AdaptorT>,
              StorageT,
              RetT(const StorageT&, const StorageT&, const std::type_info&)> {
    [[nodiscard]] static constexpr RetT
    fn(const StorageT& self, const StorageT& other, const std::type_info& other_type) {
        const auto& self_type = typeid(AdaptorT);

        // std::type_info::operator== is not constexpr until C++23
        if (std::is_constant_evaluated() ? &self_type != &other_type : self_type != other_type) {
            return SymmetricBinaryT::default_value();
        }

        return dispatch<SymmetricBinaryT, AdaptorT>(self.template unchecked_get<AdaptorT>(),
                                                    other.template unchecked_get<AdaptorT>());
    }
};

// dispatches to bindings for nullary protocols
template <nullary NullaryT, polymorphic PolyT, class RetT, class... ArgsT>
struct bridge<NullaryT, PolyT, RetT(ArgsT...)> {
    [[nodiscard]] static constexpr RetT fn(const PolyT& self, ArgsT... args) {
        return self.entry(NullaryT{})(std::forward<ArgsT>(args)...);
    }
};

// dispatches to bindings for noexcept nullary protocols
template <nullary NullaryT, polymorphic PolyT, class RetT, class... ArgsT>
struct bridge<NullaryT, PolyT, RetT(ArgsT...) noexcept> {
    [[nodiscard]] static constexpr RetT fn(const PolyT& self, ArgsT... args) noexcept {
        return self.entry(NullaryT{})(std::forward<ArgsT>(args)...);
    }
};

// dispatches to bindings for unary protocols
template <unary UnaryT, polymorphic PolyT, class RetT, class SelfT, class... ArgsT>
struct bridge<UnaryT, PolyT, RetT(SelfT, ArgsT...)> {
    [[nodiscard]] static constexpr RetT fn(SelfT self, ArgsT... args) {
        return self.entry(UnaryT{})(std::forward<SelfT>(self).get(), std::forward<ArgsT>(args)...);
    }
};

// dispatches to bindings for noexcept unary protocols
template <unary UnaryT, polymorphic PolyT, class RetT, class SelfT, class... ArgsT>
struct bridge<UnaryT, PolyT, RetT(SelfT, ArgsT...) noexcept> {
    [[nodiscard]] static constexpr RetT fn(SelfT self, ArgsT... args) noexcept {
        return self.entry(UnaryT{})(std::forward<SelfT>(self).get(), std::forward<ArgsT>(args)...);
    }
};

// dispatches to bindings for symmetric binary protocols
// requires type_t to be implemented
template <symmetric_binary SymmetricBinaryT, polymorphic PolyT, class RetT>
struct bridge<SymmetricBinaryT, PolyT, RetT(const PolyT&, const PolyT&)> {
    [[nodiscard]] static constexpr RetT fn(const PolyT& self, const PolyT& other) {
        return self.entry(SymmetricBinaryT{})(self.get(), other.get(), other.entry(type_t{})());
    }
};

template <storage StorageT>
struct move_t : nullary_protocol {
    template <not_adaptor>
    static StorageT fn(StorageT&& source) noexcept;

    template <adaptor AdaptorT>
    [[nodiscard]] static constexpr StorageT fn(StorageT&& source) noexcept {
        return {std::move(source), std::in_place_type<AdaptorT>};
    }
};

template <storage StorageT>
struct copy_t : nullary_protocol {
    template <not_adaptor>
    static StorageT fn(const StorageT& source);

    template <adaptor AdaptorT>
    [[nodiscard]] static constexpr StorageT fn(const StorageT& source) {
        return {source, std::in_place_type<AdaptorT>};
    }
};

template <storage StorageT>
struct destroy_t : nullary_protocol {
    template <not_adaptor>
    static void fn(StorageT& source) noexcept;

    template <adaptor AdaptorT>
    static constexpr void fn(StorageT& source) noexcept {
        source.template destroy_as<AdaptorT>();
    }
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_PROTOCOLS_HPP
