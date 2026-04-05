// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_VTABLE_HPP
#define BEMAN_ANY_VIEW_DETAIL_VTABLE_HPP

#include <concepts>

namespace beman::any_view::detail {

template <class T>
inline constexpr bool enable_policy = false;

template <class T>
concept policy = enable_policy<T>;

template <policy PolicyT, class T>
using signature = decltype(PolicyT::template fn<T>);

template <policy PolicyT, class T, class SignatureT = signature<PolicyT, T>>
struct impl {
    static constexpr SignatureT* fn = PolicyT::template fn<T>;
};

template <policy PolicyT, class T>
inline constexpr auto& dispatch = impl<PolicyT, T>::fn;

struct adaptor_base {};

template <class T>
concept adaptor = std::derived_from<T, adaptor_base>;

template <class T>
concept not_adaptor = not adaptor<T>;

template <class T>
inline constexpr bool enable_storage = false;

template <class T>
concept storage = enable_storage<T>;

template <class... Ts>
struct inherit : Ts... {};

template <class... Ts>
inline constexpr bool enable_policy<inherit<Ts...>> = (... and policy<Ts>);

template <policy PolicyT, adaptor AdaptorT>
struct binding : PolicyT {};

template <policy PolicyT, storage StorageT>
struct vtable {
    signature<PolicyT, StorageT>* entry;
};

template <policy... PolicyTs, storage StorageT>
struct vtable<inherit<PolicyTs...>, StorageT> : vtable<PolicyTs, StorageT>... {};

template <policy PolicyT, storage StorageT, adaptor AdaptorT>
inline constexpr vtable<PolicyT, StorageT> vtable_for{
    .entry = dispatch<binding<PolicyT, AdaptorT>, StorageT>,
};

template <policy... PolicyTs, storage StorageT, adaptor AdaptorT>
inline constexpr vtable<inherit<PolicyTs...>, StorageT> vtable_for<inherit<PolicyTs...>, StorageT, AdaptorT>{
    vtable_for<PolicyTs, StorageT, AdaptorT>...,
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_VTABLE_HPP
