// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_POLYMORPHIC_HPP
#define BEMAN_ANY_VIEW_DETAIL_POLYMORPHIC_HPP

#include <beman/any_view/detail/capabilities.hpp>
#include <beman/any_view/detail/compressed_ptr.hpp>

#include <memory>

namespace beman::any_view::detail {

template <storage StorageT, capability... CapabilityTs>
class basic_polymorphic {
    using vtable_ptrs_type = compressed_ptr<const vtable<CapabilityTs, StorageT>...>;

    StorageT         storage;
    vtable_ptrs_type vtable_ptrs;

  public:
    constexpr basic_polymorphic(const basic_polymorphic& other)
        : storage(other.entry(copy_t<StorageT>{})(other.storage)), vtable_ptrs(other.vtable_ptrs) {}

    constexpr basic_polymorphic(basic_polymorphic&& other) noexcept
        : storage(other.entry(move_t<StorageT>{})(std::move(other.storage))), vtable_ptrs(other.vtable_ptrs) {}

    template <adaptor AdaptorT>
    constexpr basic_polymorphic(AdaptorT&& adaptor)
        : storage(std::forward<AdaptorT>(adaptor)),
          vtable_ptrs(std::addressof(vtable_for<CapabilityTs, StorageT, AdaptorT>)...) {}

    template <adaptor AdaptorT>
    constexpr basic_polymorphic(std::in_place_type_t<AdaptorT> tag)
        : storage(tag), vtable_ptrs(std::addressof(vtable_for<CapabilityTs, StorageT, AdaptorT>)...) {}

    template <class GetStorageT>
        requires std::is_invocable_r_v<StorageT, GetStorageT>
    constexpr basic_polymorphic(GetStorageT get_storage, const vtable<CapabilityTs, StorageT>*... vtable_ptrs)
        : storage(get_storage()), vtable_ptrs(vtable_ptrs...) {}

    // converting constructors

    template <std::derived_from<CapabilityTs>... OtherCapabilityTs>
    constexpr basic_polymorphic(const basic_polymorphic<StorageT, OtherCapabilityTs...>& other)
        : storage(other.entry(copy_t<StorageT>{})(other.get())), vtable_ptrs(other.entries()) {}

    template <std::derived_from<CapabilityTs>... OtherCapabilityTs>
    constexpr basic_polymorphic(basic_polymorphic<StorageT, OtherCapabilityTs...>&& other) noexcept
        : storage(other.entry(move_t<StorageT>{})(std::move(other.get()))), vtable_ptrs(other.entries()) {}

    constexpr ~basic_polymorphic() { entry(destroy_t<StorageT>{})(storage); }

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

    template <std::derived_from<CapabilityTs>... OtherCapabilityTs>
    constexpr basic_polymorphic& operator=(const basic_polymorphic<StorageT, OtherCapabilityTs...>& other) {
        std::destroy_at(this);
        std::construct_at(this, other);
        return *this;
    }

    template <std::derived_from<CapabilityTs>... OtherCapabilityTs>
    constexpr basic_polymorphic& operator=(basic_polymorphic<StorageT, OtherCapabilityTs...>&& other) noexcept {
        std::destroy_at(this);
        std::construct_at(this, std::move(other));
        return *this;
    }

    constexpr StorageT&       get() & noexcept { return storage; }
    constexpr const StorageT& get() const& noexcept { return storage; }
    constexpr StorageT&&      get() && noexcept { return std::move(storage); }
    // constexpr const StorageT&& get() const&& noexcept { return std::move(storage); }

    constexpr vtable_ptrs_type entries() const noexcept { return vtable_ptrs; }

    template <capability CapabilityT>
        requires(... or std::derived_from<CapabilityTs, CapabilityT>)
    constexpr const signature<CapabilityT, StorageT>* entry(CapabilityT) const noexcept {
        return vtable_ptrs->*&vtable<CapabilityT, StorageT>::entry;
    }
};

template <storage StorageT, capability... CapabilityTs>
inline constexpr bool enable_polymorphic<basic_polymorphic<StorageT, CapabilityTs...>> = true;

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_POLYMORPHIC_HPP
