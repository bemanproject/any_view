// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_WITNESS_HPP
#define BEMAN_ANY_VIEW_DETAIL_WITNESS_HPP

#include <beman/any_view/detail/adaptors.hpp>

#include <type_traits>

namespace beman::any_view::detail {

struct protocol_base {};

template <class T>
inline constexpr bool enable_protocol = std::derived_from<T, protocol_base>;

template <class T>
inline constexpr bool enable_storage = false;

template <class T>
inline constexpr bool enable_polymorphic = false;

template <class T>
concept protocol = std::is_empty_v<T> and enable_protocol<T>;

template <class T>
concept storage = enable_storage<T>;

template <class T>
concept polymorphic = enable_polymorphic<T>;

template <protocol ProtocolT, class T>
using signature = decltype(ProtocolT::template fn<T>);

template <protocol ProtocolT, class T, class SignatureT = signature<ProtocolT, T>>
    requires std::is_function_v<SignatureT>
struct bridge {
    static constexpr SignatureT* fn = ProtocolT::template fn<T>;
};

template <protocol ProtocolT, class T>
inline constexpr auto dispatch = bridge<ProtocolT, T>::fn;

template <class... Ts>
struct inherit : Ts... {
    using inherit_type = inherit;
};

template <class T>
using inherit_type_t = typename T::inherit_type;

template <class T>
concept derived_from_inherit = std::derived_from<T, inherit_type_t<T>>;

template <class... Ts>
inline constexpr bool enable_protocol<inherit<Ts...>> = (... and protocol<Ts>);

template <derived_from_inherit T>
inline constexpr bool enable_protocol<T> = protocol<inherit_type_t<T>>;

template <protocol ProtocolT, adaptor AdaptorT>
struct thunk : ProtocolT {};

template <protocol ProtocolT, storage StorageT>
struct witness {
    signature<ProtocolT, StorageT>* entry;
};

template <protocol... ProtocolTs, storage StorageT>
struct witness<inherit<ProtocolTs...>, StorageT> : witness<ProtocolTs, StorageT>... {};

template <protocol ProtocolT, storage StorageT>
    requires derived_from_inherit<ProtocolT>
struct witness<ProtocolT, StorageT> : witness<inherit_type_t<ProtocolT>, StorageT> {};

template <protocol ProtocolT, storage StorageT, adaptor AdaptorT>
inline constexpr witness<ProtocolT, StorageT> witness_for{
    .entry = dispatch<thunk<ProtocolT, AdaptorT>, StorageT>,
};

template <protocol... ProtocolTs, storage StorageT, adaptor AdaptorT>
inline constexpr witness<inherit<ProtocolTs...>, StorageT> witness_for<inherit<ProtocolTs...>, StorageT, AdaptorT>{
    witness_for<ProtocolTs, StorageT, AdaptorT>...,
};

template <protocol ProtocolT, storage StorageT, adaptor AdaptorT>
    requires derived_from_inherit<ProtocolT>
inline constexpr witness<ProtocolT, StorageT> witness_for<ProtocolT, StorageT, AdaptorT>{
    witness_for<inherit_type_t<ProtocolT>, StorageT, AdaptorT>,
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_WITNESS_HPP
