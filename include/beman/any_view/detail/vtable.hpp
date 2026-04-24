// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_VTABLE_HPP
#define BEMAN_ANY_VIEW_DETAIL_VTABLE_HPP

#include <beman/any_view/detail/adaptors.hpp>

#include <type_traits>

namespace beman::any_view::detail {

struct capability_base {};

template <class T>
inline constexpr bool enable_capability = std::derived_from<T, capability_base>;

template <class T>
inline constexpr bool enable_storage = false;

template <class T>
inline constexpr bool enable_polymorphic = false;

template <class T>
concept capability = std::is_empty_v<T> and enable_capability<T>;

template <class T>
concept storage = enable_storage<T>;

template <class T>
concept polymorphic = enable_polymorphic<T>;

template <capability CapabilityT, class T>
using signature = decltype(CapabilityT::template fn<T>);

template <capability CapabilityT, class T, class SignatureT = signature<CapabilityT, T>>
    requires std::is_function_v<SignatureT>
struct bridge {
    static constexpr SignatureT* fn = CapabilityT::template fn<T>;
};

template <capability CapabilityT, class T>
inline constexpr auto& dispatch = *bridge<CapabilityT, T>::fn;

template <class... Ts>
struct inherit : Ts... {
    using base_type = inherit;
};

template <class T>
using base_type_t = typename T::base_type;

template <class T>
concept derived_from_base_type = std::derived_from<T, base_type_t<T>>;

template <class... Ts>
inline constexpr bool enable_capability<inherit<Ts...>> = (... and capability<Ts>);

template <derived_from_base_type T>
inline constexpr bool enable_capability<T> = capability<base_type_t<T>>;

template <capability CapabilityT, adaptor AdaptorT>
struct thunk : CapabilityT {};

template <capability CapabilityT, storage StorageT>
struct witness {
    signature<CapabilityT, StorageT>* entry;
};

template <capability... CapabilityTs, storage StorageT>
struct witness<inherit<CapabilityTs...>, StorageT> : witness<CapabilityTs, StorageT>... {};

template <capability CapabilityT, storage StorageT>
    requires derived_from_base_type<CapabilityT>
struct witness<CapabilityT, StorageT> : witness<base_type_t<CapabilityT>, StorageT> {};

template <capability CapabilityT, storage StorageT, adaptor AdaptorT>
inline constexpr witness<CapabilityT, StorageT> witness_for{
    .entry = dispatch<thunk<CapabilityT, AdaptorT>, StorageT>,
};

template <capability... CapabilityTs, storage StorageT, adaptor AdaptorT>
inline constexpr witness<inherit<CapabilityTs...>, StorageT> witness_for<inherit<CapabilityTs...>, StorageT, AdaptorT>{
    witness_for<CapabilityTs, StorageT, AdaptorT>...,
};

template <capability CapabilityT, storage StorageT, adaptor AdaptorT>
    requires derived_from_base_type<CapabilityT>
inline constexpr witness<CapabilityT, StorageT> witness_for<CapabilityT, StorageT, AdaptorT>{
    witness_for<base_type_t<CapabilityT>, StorageT, AdaptorT>,
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_VTABLE_HPP
