// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_INTRUSIVE_SMALL_PTR_HPP
#define BEMAN_ANY_VIEW_DETAIL_INTRUSIVE_SMALL_PTR_HPP

#include <concepts>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace beman::any_view::detail {

template <class InterfaceT>
concept interface_movable =
    std::has_virtual_destructor_v<InterfaceT> and requires(InterfaceT instance, void* destination) {
        { instance.move_to(destination) } noexcept -> std::same_as<void>;
    };

template <class InterfaceT>
concept interface_copyable = interface_movable<InterfaceT> and requires(const InterfaceT instance, void* destination) {
    { instance.copy_to(destination) } -> std::same_as<void>;
    { instance.copy() } -> std::same_as<InterfaceT*>;
};

template <class T>
using value_type_t = T::value_type;

enum class index_type : bool {
    is_inplace,
    is_pointer,
};

template <interface_movable InterfaceT, std::size_t SizeV = sizeof(void*), std::size_t AlignV = alignof(void*)>
class intrusive_small_ptr {
    struct alignas(AlignV) inplace_type {
        std::byte data[SizeV];
    };

    using pointer_type = InterfaceT*;

    union {
        // mutable inplace storage allows "shallow const" semantics consistent with a pointer type
        mutable inplace_type inplace;
        pointer_type         pointer;
    };

    index_type index;

    [[nodiscard]] auto get_inplace_ptr() const -> pointer_type {
        return static_cast<pointer_type>(static_cast<void*>(&inplace));
    }

  public:
    constexpr intrusive_small_ptr() noexcept : pointer(nullptr), index(index_type::is_pointer) {}

    template <std::derived_from<InterfaceT> AdaptorT, class... ArgsT>
        requires std::constructible_from<AdaptorT, ArgsT...>
    constexpr intrusive_small_ptr([[maybe_unused]] std::in_place_type_t<AdaptorT> tag, ArgsT&&... args) {
        if constexpr (sizeof(AdaptorT) <= sizeof(inplace_type) and alignof(AdaptorT) <= alignof(inplace_type) and
                      // SBO requires nothrow move construction
                      std::is_nothrow_move_constructible_v<value_type_t<AdaptorT>>) {
            // placement new is not allowed in a constant expression
            if (not std::is_constant_evaluated()) {
                ::new (&inplace) AdaptorT(std::forward<ArgsT>(args)...);
                index = index_type::is_inplace;
                return;
            }
        }

        pointer = new AdaptorT(std::forward<ArgsT>(args)...);
        index   = index_type::is_pointer;
    }

    constexpr intrusive_small_ptr(const intrusive_small_ptr& other)
        requires interface_copyable<InterfaceT>
        : index(other.index) {
        if (index == index_type::is_inplace) {
            other.get_inplace_ptr()->copy_to(&inplace);
        } else {
            pointer = other.pointer->copy();
        }
    }

    constexpr intrusive_small_ptr(intrusive_small_ptr&& other) noexcept : index(other.index) {
        if (index == index_type::is_inplace) {
            other.get_inplace_ptr()->move_to(&inplace);
        } else {
            pointer = std::exchange(other.pointer, nullptr);
        }
    }

    constexpr auto operator=(const intrusive_small_ptr& other) -> intrusive_small_ptr&
        requires interface_copyable<InterfaceT>
    {
        // prevent self-assignment
        if (this == std::addressof(other)) {
            return *this;
        }

        this->~intrusive_small_ptr();
        std::construct_at(this, other);
        return *this;
    }

    constexpr auto operator=(intrusive_small_ptr&& other) noexcept -> intrusive_small_ptr& {
        this->~intrusive_small_ptr();
        std::construct_at(this, std::move(other));
        return *this;
    }

    [[nodiscard]] constexpr explicit operator bool() const noexcept {
        return index == index_type::is_inplace or pointer != nullptr;
    }

    [[nodiscard]] constexpr auto get() const noexcept -> InterfaceT* {
        if (index == index_type::is_inplace) {
            return get_inplace_ptr();
        }

        return pointer;
    }

    [[nodiscard]] constexpr auto operator*() const noexcept -> InterfaceT& { return *get(); }

    [[nodiscard]] constexpr auto operator->() const noexcept -> InterfaceT* { return get(); }

    constexpr ~intrusive_small_ptr() noexcept {
        if (index == index_type::is_inplace) {
            get_inplace_ptr()->~InterfaceT();
        } else {
            delete pointer;
        }

        pointer = nullptr;
        index   = index_type::is_pointer;
    }
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_INTRUSIVE_SMALL_PTR_HPP
