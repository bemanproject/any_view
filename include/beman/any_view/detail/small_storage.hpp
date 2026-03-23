// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_SMALL_STORAGE_HPP
#define BEMAN_ANY_VIEW_DETAIL_SMALL_STORAGE_HPP

#include <beman/any_view/detail/base.hpp>
#include <beman/any_view/detail/policies.hpp>

#include <concepts>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace beman::any_view::detail {

template <std::size_t SizeV, std::size_t AlignV = alignof(void*)>
class small_storage {
    union {
        alignas(AlignV) std::byte inplace[SizeV];
        base* pointer{};
    };

  public:
    template <std::derived_from<base> AdaptorT>
    constexpr explicit small_storage(AdaptorT&& adaptor) {
        visit(
            *this,
            std::in_place_type<AdaptorT>,
            [&](auto& storage) {
                std::construct_at(reinterpret_cast<AdaptorT*>(std::addressof(storage)),
                                  std::forward<AdaptorT>(adaptor));
            },
            [&](auto& storage) { storage = ::new AdaptorT(std::forward<AdaptorT>(adaptor)); });
    }

    template <std::derived_from<base> AdaptorT>
    constexpr small_storage(const small_storage& other, std::in_place_type_t<AdaptorT>) {
        visit(
            *this,
            std::in_place_type<AdaptorT>,
            [&](auto& storage) {
                std::construct_at(reinterpret_cast<AdaptorT*>(std::addressof(storage)),
                                  reinterpret_cast<const AdaptorT&>(other.inplace));
            },
            [&](auto& storage) { storage = ::new AdaptorT(static_cast<const AdaptorT&>(*other.pointer)); });
    }

    template <std::derived_from<base> AdaptorT>
    constexpr small_storage(small_storage&& other, std::in_place_type_t<AdaptorT>) noexcept {
        visit(
            *this,
            std::in_place_type<AdaptorT>,
            [&](auto& storage) {
                std::construct_at(reinterpret_cast<AdaptorT*>(std::addressof(storage)),
                                  reinterpret_cast<AdaptorT&&>(std::move(other).inplace));
            },
            [&](auto& storage) { storage = std::exchange(other.pointer, nullptr); });
    }

    template <std::derived_from<base> AdaptorT>
    constexpr void destroy(std::in_place_type_t<AdaptorT>) noexcept {
        visit(
            *this,
            std::in_place_type<AdaptorT>,
            [](auto& storage) { std::destroy_at(reinterpret_cast<AdaptorT*>(std::addressof(storage))); },
            [](auto& storage) { ::delete static_cast<AdaptorT*>(std::exchange(storage, nullptr)); });
    }

    template <class AdaptorT>
    [[nodiscard]] constexpr AdaptorT& unchecked_get(std::in_place_type_t<AdaptorT> tag) & {
        return visit(
            *this,
            tag,
            [](auto& storage) -> AdaptorT& { return reinterpret_cast<AdaptorT&>(storage); },
            [](auto& storage) -> AdaptorT& { return static_cast<AdaptorT&>(*storage); });
    }

    template <class AdaptorT>
    [[nodiscard]] constexpr const AdaptorT& unchecked_get(std::in_place_type_t<AdaptorT> tag) const& {
        return visit(
            *this,
            tag,
            [](auto&& storage) -> const AdaptorT& { return reinterpret_cast<const AdaptorT&>(storage); },
            [](auto&& storage) -> const AdaptorT& { return static_cast<const AdaptorT&>(*storage); });
    }

    template <class AdaptorT>
    [[nodiscard]] constexpr AdaptorT&& unchecked_get(std::in_place_type_t<AdaptorT> tag) && {
        return visit(
            std::move(*this),
            tag,
            [](auto&& storage) -> AdaptorT&& { return reinterpret_cast<AdaptorT&&>(storage); },
            [](auto&& storage) -> AdaptorT&& { return static_cast<AdaptorT&&>(*storage); });
    }

    template <class AdaptorT>
    [[nodiscard]] constexpr const AdaptorT&& unchecked_get(std::in_place_type_t<AdaptorT> tag) const&& {
        return visit(
            std::move(*this),
            tag,
            [](auto&& storage) -> const AdaptorT&& { return reinterpret_cast<const AdaptorT&&>(storage); },
            [](auto&& storage) -> const AdaptorT&& { return static_cast<const AdaptorT&&>(*storage); });
    }

  private:
    template <class SelfT, class AdaptorT, class InplaceVisitorT, class PointerVisitorT>
    [[nodiscard]] static constexpr decltype(auto) visit(SelfT&& self,
                                                        std::in_place_type_t<AdaptorT>,
                                                        InplaceVisitorT&& inplace_visitor,
                                                        PointerVisitorT&& pointer_visitor) {
        if constexpr (sizeof(AdaptorT) <= SizeV and alignof(AdaptorT) <= AlignV and
                      std::is_nothrow_move_constructible_v<AdaptorT>) {
            if (not std::is_constant_evaluated()) {
                return std::forward<InplaceVisitorT>(inplace_visitor)(std::forward<SelfT>(self).inplace);
            }
        }

        return std::forward<PointerVisitorT>(pointer_visitor)(std::forward<SelfT>(self).pointer);
    }
};

template <class T>
inline constexpr bool is_storage_v = false;

template <std::size_t SizeV, std::size_t AlignV>
inline constexpr bool is_storage_v<small_storage<SizeV, AlignV>> = true;

// storage bindings for unary policies
template <std::derived_from<unary_policy> PolicyT,
          class AdaptorT,
          class StorageT,
          class RetT,
          class SelfT,
          class... ArgsT>
    requires is_storage_v<StorageT>
struct impl<binding<PolicyT, AdaptorT>, StorageT, RetT(SelfT&, ArgsT...)> {
    [[nodiscard]] static constexpr RetT fn(SelfT& self, ArgsT... args) {
        return detail::fn<PolicyT, AdaptorT>(self.unchecked_get(std::in_place_type<AdaptorT>),
                                             std::forward<ArgsT>(args)...);
    }
};

// storage bindings for symmetric binary policies
template <std::derived_from<symmetric_binary_policy> PolicyT, class AdaptorT, class StorageT, class RetT>
    requires is_storage_v<StorageT>
struct impl<binding<PolicyT, AdaptorT>, StorageT, RetT(const StorageT&, const StorageT&)> {
    [[nodiscard]] static constexpr RetT fn(const StorageT& self, const StorageT& other) {
        return detail::fn<PolicyT, AdaptorT>(self.unchecked_get(std::in_place_type<AdaptorT>),
                                             other.unchecked_get(std::in_place_type<AdaptorT>));
    }
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_SMALL_STORAGE_HPP
