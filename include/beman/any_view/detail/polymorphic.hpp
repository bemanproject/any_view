// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_POLYMORPHIC_HPP
#define BEMAN_ANY_VIEW_DETAIL_POLYMORPHIC_HPP

#include <beman/any_view/detail/compressed_ptr.hpp>
#include <beman/any_view/detail/policies.hpp>

#include <memory>

namespace beman::any_view::detail {

template <storage StorageT, policy... PolicyTs>
class basic_polymorphic {
    using vtable_ptrs_type = compressed_ptr<const vtable<PolicyTs, StorageT>...>;

    StorageT         storage;
    vtable_ptrs_type vtable_ptrs;

  public:
    constexpr basic_polymorphic(const basic_polymorphic& other)
        : storage(other[entry<copy_policy<StorageT>>](other.get())), vtable_ptrs(other.entries()) {}

    constexpr basic_polymorphic(basic_polymorphic&& other) noexcept
        : storage(other[entry<move_policy<StorageT>>](std::move(other.get()))), vtable_ptrs(other.entries()) {}

    template <adaptor AdaptorT>
    constexpr basic_polymorphic(AdaptorT&& adaptor)
        : storage(std::forward<AdaptorT>(adaptor)),
          vtable_ptrs(std::addressof(vtable_for<PolicyTs, StorageT, AdaptorT>)...) {}

    template <class GetStorageT>
        requires std::is_invocable_r_v<StorageT, GetStorageT>
    constexpr basic_polymorphic(GetStorageT get_storage, const vtable<PolicyTs, StorageT>*... vtable_ptrs)
        : storage(get_storage()), vtable_ptrs(vtable_ptrs...) {}

    // converting constructors

    template <std::derived_from<PolicyTs>... OtherPolicyTs>
    constexpr basic_polymorphic(const basic_polymorphic<StorageT, OtherPolicyTs...>& other)
        : storage(other[entry<copy_policy<StorageT>>](other.get())), vtable_ptrs(other.entries()) {}

    template <std::derived_from<PolicyTs>... OtherPolicyTs>
    constexpr basic_polymorphic(basic_polymorphic<StorageT, OtherPolicyTs...>&& other) noexcept
        : storage(other[entry<move_policy<StorageT>>](std::move(other.get()))), vtable_ptrs(other.entries()) {}

    constexpr ~basic_polymorphic() { dispatch<destroy_policy<StorageT>, basic_polymorphic>(*this, storage); }

    constexpr basic_polymorphic& operator=(const basic_polymorphic& other) {
        if (this == std::addressof(other)) {
            return *this;
        }

        std::destroy_at(this);
        std::construct_at(this, other);
        return *this;
    }

    constexpr basic_polymorphic& operator=(basic_polymorphic&& other) noexcept {
        if (this == std::addressof(other)) {
            return *this;
        }

        std::destroy_at(this);
        std::construct_at(this, std::move(other));
        return *this;
    }

    template <adaptor AdaptorT>
    constexpr basic_polymorphic& operator=(AdaptorT&& adaptor) {
        std::destroy_at(this);
        std::construct_at(this, std::forward<AdaptorT>(adaptor));
        return *this;
    }

    // converting assignment

    template <std::derived_from<PolicyTs>... OtherPolicyTs>
    constexpr basic_polymorphic& operator=(const basic_polymorphic<StorageT, OtherPolicyTs...>& other) {
        std::destroy_at(this);
        std::construct_at(this, other);
        return *this;
    }

    template <std::derived_from<PolicyTs>... OtherPolicyTs>
    constexpr basic_polymorphic& operator=(basic_polymorphic<StorageT, OtherPolicyTs...>&& other) noexcept {
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

template <storage StorageT, policy... PolicyTs>
inline constexpr bool enable_polymorphic<basic_polymorphic<StorageT, PolicyTs...>> = true;

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_POLYMORPHIC_HPP
