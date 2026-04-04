// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_SMALL_POLYMORPHIC_HPP
#define BEMAN_ANY_VIEW_DETAIL_SMALL_POLYMORPHIC_HPP

#include <beman/any_view/detail/compressed_ptr.hpp>
#include <beman/any_view/detail/policies.hpp>
#include <beman/any_view/detail/vtable.hpp>

#include <memory>
#include <type_traits>
#include <utility>

namespace beman::any_view::detail {

template <policy PolicyT>
struct entry_t {
    explicit entry_t() = default;
};

template <policy PolicyT>
inline constexpr entry_t<PolicyT> entry{};

template <storage StorageT, policy... PolicyTs>
class small_polymorphic {
    using vtable_ptrs_type = compressed_ptr<const vtable<PolicyTs, StorageT>...>;

    StorageT         storage;
    vtable_ptrs_type vtable_ptrs;

  public:
    constexpr small_polymorphic(const small_polymorphic& other)
        : storage(other[entry<copy_policy<StorageT>>](other.get())), vtable_ptrs(other.entries()) {}

    constexpr small_polymorphic(small_polymorphic&& other) noexcept
        : storage(other[entry<move_policy<StorageT>>](std::move(other.get()))), vtable_ptrs(other.entries()) {}

    template <adaptor AdaptorT>
    constexpr small_polymorphic(AdaptorT&& adaptor)
        : storage(std::forward<AdaptorT>(adaptor)),
          vtable_ptrs(std::addressof(vtable_for<PolicyTs, StorageT, AdaptorT>)...) {}

    template <class GetStorageT>
        requires std::is_invocable_r_v<StorageT, GetStorageT>
    constexpr small_polymorphic(GetStorageT get_storage, const vtable<PolicyTs, StorageT>*... vtable_ptrs)
        : storage(get_storage()), vtable_ptrs(vtable_ptrs...) {}

    // converting constructors

    template <std::derived_from<PolicyTs>... OtherPolicyTs>
    constexpr small_polymorphic(const small_polymorphic<StorageT, OtherPolicyTs...>& other)
        : storage(other[entry<copy_policy<StorageT>>](other.get())), vtable_ptrs(other.entries()) {}

    template <std::derived_from<PolicyTs>... OtherPolicyTs>
    constexpr small_polymorphic(small_polymorphic<StorageT, OtherPolicyTs...>&& other) noexcept
        : storage(other[entry<move_policy<StorageT>>](std::move(other.get()))), vtable_ptrs(other.entries()) {}

    constexpr ~small_polymorphic() { dispatch<destroy_policy<StorageT>, small_polymorphic>(*this, storage); }

    constexpr small_polymorphic& operator=(const small_polymorphic& other) {
        if (this == std::addressof(other)) {
            return *this;
        }

        std::destroy_at(this);
        std::construct_at(this, other);
        return *this;
    }

    constexpr small_polymorphic& operator=(small_polymorphic&& other) noexcept {
        if (this == std::addressof(other)) {
            return *this;
        }

        std::destroy_at(this);
        std::construct_at(this, std::move(other));
        return *this;
    }

    template <adaptor AdaptorT>
    constexpr small_polymorphic& operator=(AdaptorT&& adaptor) {
        std::destroy_at(this);
        std::construct_at(this, std::forward<AdaptorT>(adaptor));
        return *this;
    }

    // converting assignment

    template <std::derived_from<PolicyTs>... OtherPolicyTs>
    constexpr small_polymorphic& operator=(const small_polymorphic<StorageT, OtherPolicyTs...>& other) {
        std::destroy_at(this);
        std::construct_at(this, other);
        return *this;
    }

    template <std::derived_from<PolicyTs>... OtherPolicyTs>
    constexpr small_polymorphic& operator=(small_polymorphic<StorageT, OtherPolicyTs...>&& other) noexcept {
        std::destroy_at(this);
        std::construct_at(this, std::move(other));
        return *this;
    }

    constexpr StorageT&       get() noexcept { return storage; }
    constexpr const StorageT& get() const noexcept { return storage; }

    constexpr const vtable_ptrs_type& entries() const noexcept { return vtable_ptrs; }

    template <policy PolicyT>
        requires(... or std::derived_from<PolicyTs, PolicyT>)
    constexpr const signature<PolicyT, StorageT>* operator[](entry_t<PolicyT>) const noexcept {
        return vtable_ptrs->*&vtable<PolicyT, StorageT>::entry;
    }
};

template <class T>
inline constexpr bool enable_polymorphic = false;

template <storage StorageT, policy... PolicyTs>
inline constexpr bool enable_polymorphic<small_polymorphic<StorageT, PolicyTs...>> = true;

template <class T>
concept polymorphic = enable_polymorphic<T>;

// dispatches to storage bindings for nullary policies
template <std::derived_from<nullary_policy> NullaryT, polymorphic PolyT, class RetT, class... ArgsT>
struct impl<NullaryT, PolyT, RetT(ArgsT...)> {
    [[nodiscard]] static constexpr RetT fn(const PolyT& self, ArgsT... args) {
        return self[entry<NullaryT>](std::forward<ArgsT>(args)...);
    }
};

// dispatches to storage bindings for noexcept nullary policies
template <std::derived_from<nullary_policy> NullaryT, polymorphic PolyT, class RetT, class... ArgsT>
struct impl<NullaryT, PolyT, RetT(ArgsT...) noexcept> {
    [[nodiscard]] static constexpr RetT fn(const PolyT& self, ArgsT... args) noexcept {
        return self[entry<NullaryT>](std::forward<ArgsT>(args)...);
    }
};

// dispatches to storage bindings for unary policies
template <std::derived_from<unary_policy> UnaryT, polymorphic PolyT, class RetT, class SelfT, class... ArgsT>
struct impl<UnaryT, PolyT, RetT(SelfT&, ArgsT...)> {
    [[nodiscard]] static constexpr RetT fn(SelfT& self, ArgsT... args) {
        return self[entry<UnaryT>](self.get(), std::forward<ArgsT>(args)...);
    }
};

// dispatches to storage bindings for symmetric binary policies
// requires type_policy to be implemented
template <std::derived_from<symmetric_binary_policy> SymmetricBinaryT, polymorphic PolyT, class RetT>
struct impl<SymmetricBinaryT, PolyT, RetT(const PolyT&, const PolyT&, const std::type_info&)> {
    [[nodiscard]] static constexpr RetT fn(const PolyT& self, const PolyT& other) {
        return self[entry<SymmetricBinaryT>](self.get(), other.get(), other[entry<type_policy>]());
    }
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_SMALL_POLYMORPHIC_HPP
