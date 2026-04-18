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
struct impl {
    static constexpr SignatureT* fn = CapabilityT::template fn<T>;
};

template <capability CapabilityT, class T>
inline constexpr auto& dispatch = impl<CapabilityT, T>::fn;

template <class... Ts>
struct inherit : Ts... {};

template <class... Ts>
inline constexpr bool enable_capability<inherit<Ts...>> = (... and capability<Ts>);

template <capability CapabilityT, adaptor AdaptorT>
struct binding : CapabilityT {};

template <capability CapabilityT, storage StorageT>
struct vtable {
    signature<CapabilityT, StorageT>* entry;
};

template <capability... CapabilityTs, storage StorageT>
struct vtable<inherit<CapabilityTs...>, StorageT> : vtable<CapabilityTs, StorageT>... {};

template <capability CapabilityT, storage StorageT, adaptor AdaptorT>
inline constexpr vtable<CapabilityT, StorageT> vtable_for{
    .entry = dispatch<binding<CapabilityT, AdaptorT>, StorageT>,
};

template <capability... CapabilityTs, storage StorageT, adaptor AdaptorT>
inline constexpr vtable<inherit<CapabilityTs...>, StorageT> vtable_for<inherit<CapabilityTs...>, StorageT, AdaptorT>{
    vtable_for<CapabilityTs, StorageT, AdaptorT>...,
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_VTABLE_HPP
