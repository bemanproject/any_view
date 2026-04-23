// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_SMALL_STORAGE_HPP
#define BEMAN_ANY_VIEW_DETAIL_SMALL_STORAGE_HPP

#include <beman/any_view/detail/vtable.hpp>

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace beman::any_view::detail {

template <std::size_t SizeV>
class small_storage {
    using inplace_type = std::byte[SizeV];
    // static_cast from void pointer is not constexpr until C++26
    using pointer_type = adaptor_base*;

    union {
        inplace_type inplace;
        pointer_type pointer;
    };

  public:
    template <adaptor AdaptorT>
    constexpr explicit small_storage(AdaptorT&& adaptor) {
        visit<AdaptorT>(
            *this,
            [&](inplace_type& inplace) { ::new (&inplace) AdaptorT(std::forward<AdaptorT>(adaptor)); },
            [&](pointer_type& pointer) {
                std::construct_at(&pointer, ::new AdaptorT(std::forward<AdaptorT>(adaptor)));
            });
    }

    template <adaptor AdaptorT>
    constexpr explicit small_storage(std::in_place_type_t<AdaptorT>) {
        visit<AdaptorT>(
            *this,
            [](inplace_type& inplace) { ::new (&inplace) AdaptorT(); },
            [](pointer_type& pointer) { std::construct_at(&pointer, ::new AdaptorT()); });
    }

    template <adaptor AdaptorT>
    constexpr small_storage(const small_storage& other, std::in_place_type_t<AdaptorT>) {
        visit<AdaptorT>(
            *this,
            [&](inplace_type& inplace) {
                ::new (&inplace) AdaptorT(reinterpret_cast<const AdaptorT&>(other.inplace));
            },
            [&](pointer_type& pointer) {
                std::construct_at(&pointer, ::new AdaptorT(static_cast<const AdaptorT&>(*other.pointer)));
            });
    }

    template <adaptor AdaptorT>
    constexpr small_storage(small_storage&& other, std::in_place_type_t<AdaptorT>) noexcept {
        visit<AdaptorT>(
            *this,
            [&](inplace_type& inplace) { ::new (&inplace) AdaptorT(reinterpret_cast<AdaptorT&&>(other.inplace)); },
            [&](pointer_type& pointer) { std::construct_at(&pointer, std::exchange(other.pointer, nullptr)); });
    }

    template <adaptor AdaptorT>
    constexpr void destroy_as() noexcept {
        visit<AdaptorT>(
            *this,
            [](inplace_type& inplace) { reinterpret_cast<AdaptorT&>(inplace).~AdaptorT(); },
            [](pointer_type& pointer) { ::delete static_cast<AdaptorT*>(std::exchange(pointer, nullptr)); });
    }

    template <adaptor AdaptorT>
    [[nodiscard]] constexpr AdaptorT& unchecked_get() & noexcept {
        return visit<AdaptorT>(
            *this,
            [](inplace_type& inplace) -> AdaptorT& { return reinterpret_cast<AdaptorT&>(inplace); },
            [](pointer_type& pointer) -> AdaptorT& { return static_cast<AdaptorT&>(*pointer); });
    }

    template <adaptor AdaptorT>
    [[nodiscard]] constexpr const AdaptorT& unchecked_get() const& noexcept {
        return visit<AdaptorT>(
            *this,
            [](const inplace_type& inplace) -> const AdaptorT& { return reinterpret_cast<const AdaptorT&>(inplace); },
            [](const pointer_type& pointer) -> const AdaptorT& { return static_cast<const AdaptorT&>(*pointer); });
    }

    template <adaptor AdaptorT>
    [[nodiscard]] constexpr AdaptorT&& unchecked_get() && noexcept {
        return visit<AdaptorT>(
            *this,
            [](inplace_type& inplace) -> AdaptorT&& { return reinterpret_cast<AdaptorT&&>(inplace); },
            [](pointer_type& pointer) -> AdaptorT&& { return static_cast<AdaptorT&&>(*pointer); });
    }

    // template <adaptor AdaptorT>
    // [[nodiscard]] constexpr const AdaptorT&& unchecked_get() const&& noexcept;

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

template <std::size_t SizeV>
inline constexpr bool enable_storage<small_storage<SizeV>> = true;

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_SMALL_STORAGE_HPP
