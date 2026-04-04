// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_SMALL_STORAGE_HPP
#define BEMAN_ANY_VIEW_DETAIL_SMALL_STORAGE_HPP

#include <beman/any_view/detail/adaptor_base.hpp>
#include <beman/any_view/detail/policies.hpp>

#include <concepts>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace beman::any_view::detail {

template <std::size_t SizeV>
class small_storage {
    union {
        std::byte inplace[SizeV];
        // static_cast from void pointer is not constexpr until C++26
        adaptor_base* pointer;
    };

  public:
    template <adaptor AdaptorT>
    constexpr explicit small_storage(AdaptorT&& adaptor) {
        visit<AdaptorT>(
            *this,
            [&](auto& inplace) {
                std::construct_at(reinterpret_cast<AdaptorT*>(std::addressof(inplace)),
                                  std::forward<AdaptorT>(adaptor));
            },
            [&](auto& pointer) {
                std::construct_at(std::addressof(pointer), ::new AdaptorT(std::forward<AdaptorT>(adaptor)));
            });
    }

    template <adaptor AdaptorT>
    constexpr small_storage(const small_storage& other, std::in_place_type_t<AdaptorT>) {
        visit<AdaptorT>(
            *this,
            [&](auto& inplace) {
                std::construct_at(reinterpret_cast<AdaptorT*>(std::addressof(inplace)),
                                  reinterpret_cast<const AdaptorT&>(other.inplace));
            },
            [&](auto& pointer) {
                std::construct_at(std::addressof(pointer),
                                  ::new AdaptorT(static_cast<const AdaptorT&>(*other.pointer)));
            });
    }

    template <adaptor AdaptorT>
    constexpr small_storage(small_storage&& other, std::in_place_type_t<AdaptorT>) noexcept {
        visit<AdaptorT>(
            *this,
            [&](auto& inplace) {
                std::construct_at(reinterpret_cast<AdaptorT*>(std::addressof(inplace)),
                                  reinterpret_cast<AdaptorT&&>(std::move(other).inplace));
            },
            [&](auto& pointer) { std::construct_at(std::addressof(pointer), std::exchange(other.pointer, nullptr)); });
    }

    template <adaptor AdaptorT>
    constexpr friend void destroy_as(small_storage& self) noexcept {
        visit<AdaptorT>(
            self,
            [](auto& inplace) { std::destroy_at(reinterpret_cast<AdaptorT*>(std::addressof(inplace))); },
            [](auto& pointer) { ::delete static_cast<AdaptorT*>(std::exchange(pointer, nullptr)); });
    }

    template <adaptor AdaptorT>
    [[nodiscard]] constexpr friend AdaptorT& unchecked_get(small_storage& self) {
        return visit<AdaptorT>(
            self,
            [](auto& inplace) -> AdaptorT& { return reinterpret_cast<AdaptorT&>(inplace); },
            [](auto& pointer) -> AdaptorT& { return static_cast<AdaptorT&>(*pointer); });
    }

    template <adaptor AdaptorT>
    [[nodiscard]] constexpr friend const AdaptorT& unchecked_get(const small_storage& self) {
        return visit<AdaptorT>(
            self,
            [](auto& inplace) -> const AdaptorT& { return reinterpret_cast<const AdaptorT&>(inplace); },
            [](auto& pointer) -> const AdaptorT& { return static_cast<const AdaptorT&>(*pointer); });
    }

  private:
    template <adaptor AdaptorT, class SelfT, class InplaceVisitorT, class PointerVisitorT>
    [[nodiscard]] static constexpr decltype(auto)
    visit(SelfT& self, InplaceVisitorT inplace_visitor, PointerVisitorT pointer_visitor) {
        if constexpr (sizeof(AdaptorT) <= sizeof(small_storage) and alignof(AdaptorT) <= alignof(small_storage) and
                      std::is_nothrow_move_constructible_v<AdaptorT>) {
            if (not std::is_constant_evaluated()) {
                return inplace_visitor(self.inplace);
            }
        }

        return pointer_visitor(self.pointer);
    }
};

template <class T>
inline constexpr bool is_storage_v = false;

template <std::size_t SizeV>
inline constexpr bool is_storage_v<small_storage<SizeV>> = true;

// storage bindings for unary policies
template <std::derived_from<unary_policy> PolicyT,
          adaptor                         AdaptorT,
          class StorageT,
          class RetT,
          class SelfT,
          class... ArgsT>
    requires is_storage_v<StorageT>
struct impl<binding<PolicyT, AdaptorT>, StorageT, RetT(SelfT&, ArgsT...)> {
    [[nodiscard]] static constexpr RetT fn(SelfT& self, ArgsT... args) {
        return dispatch<PolicyT, AdaptorT>(unchecked_get<AdaptorT>(self), std::forward<ArgsT>(args)...);
    }
};

// storage bindings for symmetric binary policies
template <std::derived_from<symmetric_binary_policy> PolicyT, adaptor AdaptorT, class StorageT, class RetT>
    requires is_storage_v<StorageT>
struct impl<binding<PolicyT, AdaptorT>, StorageT, RetT(const StorageT&, const StorageT&, const std::type_info&)> {
    [[nodiscard]] static constexpr RetT
    fn(const StorageT& self, const StorageT& other, const std::type_info& other_type) {
        const auto& self_type = dispatch<type_policy, AdaptorT>();

        // std::type_info::operator== is not constexpr until C++23
        if (std::is_constant_evaluated() ? &self_type != &other_type : self_type != other_type) {
            return PolicyT::default_value();
        }

        return dispatch<PolicyT, AdaptorT>(unchecked_get<AdaptorT>(self), unchecked_get<AdaptorT>(other));
    }
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_SMALL_STORAGE_HPP
