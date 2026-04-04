// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_VTABLE_HPP
#define BEMAN_ANY_VIEW_DETAIL_VTABLE_HPP

namespace beman::any_view::detail {

template <class PolicyT, class T>
using signature = decltype(PolicyT::template fn<T>);

template <class PolicyT, class T, class SignatureT = signature<PolicyT, T>>
struct impl {
    static constexpr SignatureT* fn = PolicyT::template fn<T>;
};

template <class PolicyT, class T>
inline constexpr auto& dispatch = impl<PolicyT, T>::fn;

template <class... Ts>
struct inherit : Ts... {};

template <class PolicyT, class AdaptorT>
struct binding : PolicyT {};

template <class PolicyT, class StorageT>
struct vtable {
    signature<PolicyT, StorageT>* entry;
};

template <class... PolicyTs, class StorageT>
struct vtable<inherit<PolicyTs...>, StorageT> : vtable<PolicyTs, StorageT>... {};

template <class PolicyT, class StorageT, class AdaptorT>
inline constexpr vtable<PolicyT, StorageT> vtable_for{
    .entry = dispatch<binding<PolicyT, AdaptorT>, StorageT>,
};

template <class... PolicyTs, class StorageT, class AdaptorT>
inline constexpr vtable<inherit<PolicyTs...>, StorageT> vtable_for<inherit<PolicyTs...>, StorageT, AdaptorT>{
    vtable_for<PolicyTs, StorageT, AdaptorT>...,
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_VTABLE_HPP
