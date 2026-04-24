// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_POLYMORPHIC_HPP
#define BEMAN_ANY_VIEW_DETAIL_POLYMORPHIC_HPP

#include <beman/any_view/detail/compressed_ptr.hpp>
#include <beman/any_view/detail/protocols.hpp>

#include <memory>

namespace beman::any_view::detail {

template <storage StorageT, protocol... ProtocolTs>
class basic_polymorphic {
    using witness_ptrs_type = compressed_ptr<const witness<ProtocolTs, StorageT>...>;

    StorageT          storage;
    witness_ptrs_type witness_ptrs;

  public:
    constexpr basic_polymorphic(const basic_polymorphic& other)
        : storage(other.entry(copy_t<StorageT>{})(other.storage)), witness_ptrs(other.witness_ptrs) {}

    constexpr basic_polymorphic(basic_polymorphic&& other) noexcept
        : storage(other.entry(move_t<StorageT>{})(std::move(other.storage))), witness_ptrs(other.witness_ptrs) {}

    template <adaptor AdaptorT>
    constexpr basic_polymorphic(AdaptorT&& adaptor)
        : storage(std::forward<AdaptorT>(adaptor)),
          witness_ptrs(std::addressof(witness_for<ProtocolTs, StorageT, AdaptorT>)...) {}

    template <adaptor AdaptorT>
    constexpr basic_polymorphic(std::in_place_type_t<AdaptorT> tag)
        : storage(tag), witness_ptrs(std::addressof(witness_for<ProtocolTs, StorageT, AdaptorT>)...) {}

    template <class GetStorageT>
        requires std::is_invocable_r_v<StorageT, GetStorageT>
    constexpr basic_polymorphic(GetStorageT get_storage, const witness<ProtocolTs, StorageT>*... witness_ptrs)
        : storage(get_storage()), witness_ptrs(witness_ptrs...) {}

    // converting constructors

    template <std::derived_from<ProtocolTs>... OtherProtocolTs>
    constexpr basic_polymorphic(const basic_polymorphic<StorageT, OtherProtocolTs...>& other)
        : storage(other.entry(copy_t<StorageT>{})(other.get())), witness_ptrs(other.witnesses()) {}

    template <std::derived_from<ProtocolTs>... OtherProtocolTs>
    constexpr basic_polymorphic(basic_polymorphic<StorageT, OtherProtocolTs...>&& other) noexcept
        : storage(other.entry(move_t<StorageT>{})(std::move(other.get()))), witness_ptrs(other.witnesses()) {}

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

    template <std::derived_from<ProtocolTs>... OtherProtocolTs>
    constexpr basic_polymorphic& operator=(const basic_polymorphic<StorageT, OtherProtocolTs...>& other) {
        std::destroy_at(this);
        std::construct_at(this, other);
        return *this;
    }

    template <std::derived_from<ProtocolTs>... OtherProtocolTs>
    constexpr basic_polymorphic& operator=(basic_polymorphic<StorageT, OtherProtocolTs...>&& other) noexcept {
        std::destroy_at(this);
        std::construct_at(this, std::move(other));
        return *this;
    }

    constexpr StorageT&       get() & noexcept { return storage; }
    constexpr const StorageT& get() const& noexcept { return storage; }
    constexpr StorageT&&      get() && noexcept { return std::move(storage); }
    // constexpr const StorageT&& get() const&& noexcept { return std::move(storage); }

    constexpr witness_ptrs_type witnesses() const noexcept { return witness_ptrs; }

    template <protocol ProtocolT>
        requires(... or std::derived_from<ProtocolTs, ProtocolT>)
    constexpr const signature<ProtocolT, StorageT>* entry(ProtocolT) const noexcept {
        return witness_ptrs->*&witness<ProtocolT, StorageT>::entry;
    }
};

template <storage StorageT, protocol... ProtocolTs>
inline constexpr bool enable_polymorphic<basic_polymorphic<StorageT, ProtocolTs...>> = true;

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_POLYMORPHIC_HPP
