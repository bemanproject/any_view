// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_SMALL_POLYMORPHIC_HPP
#define BEMAN_ANY_VIEW_DETAIL_SMALL_POLYMORPHIC_HPP

#include <beman/any_view/detail/adaptor_base.hpp>
#include <beman/any_view/detail/compressed_ptr.hpp>
#include <beman/any_view/detail/policies.hpp>
#include <beman/any_view/detail/vtable.hpp>

#include <memory>
#include <type_traits>
#include <utility>

namespace beman::any_view::detail {

template <class PolyT>
struct polymorphic_traits;

template <class StorageT, class... PolicyTs>
class small_polymorphic {
    friend polymorphic_traits<small_polymorphic>;

    template <class OtherStorageT, class... OtherPolicyTs>
    friend class small_polymorphic;

    template <class PolicyT>
    using vtable_ptr = compressed_ptr<const vtable<PolicyT, StorageT>>;

    template <class... OtherPolicyTs>
    using rebind_traits = polymorphic_traits<small_polymorphic<StorageT, OtherPolicyTs...>>;

    StorageT                         storage;
    inherit<vtable_ptr<PolicyTs>...> vtable_ptrs;

  public:
    constexpr small_polymorphic(const small_polymorphic& other)
        : storage(detail::fn<copy_policy<StorageT>, small_polymorphic>(other, other.storage)),
          vtable_ptrs(other.vtable_ptrs) {}

    constexpr small_polymorphic(small_polymorphic&& other) noexcept
        : storage(detail::fn<move_policy<StorageT>, small_polymorphic>(other, std::move(other.storage))),
          vtable_ptrs(other.vtable_ptrs) {}

    template <adaptor AdaptorT>
    constexpr small_polymorphic(AdaptorT&& adaptor)
        : storage(std::forward<AdaptorT>(adaptor)),
          vtable_ptrs(std::addressof(detail::vtable_for<PolicyTs, StorageT, AdaptorT>)...) {}

    template <class GetStorageT>
        requires std::is_invocable_r_v<StorageT, GetStorageT>
    constexpr small_polymorphic(GetStorageT&& get_storage, const vtable<PolicyTs, StorageT>*... vtable_ptrs)
        : storage(std::forward<GetStorageT>(get_storage)()), vtable_ptrs(vtable_ptrs...) {}

    // converting constructors

    template <std::derived_from<PolicyTs>... OtherPolicyTs>
    constexpr small_polymorphic(const small_polymorphic<StorageT, OtherPolicyTs...>& other)
        : storage(
              detail::fn<copy_policy<StorageT>, small_polymorphic<StorageT, OtherPolicyTs...>>(other, other.storage)),
          vtable_ptrs(std::to_address<vtable_ptr<OtherPolicyTs>>(other.vtable_ptrs)...) {}

    template <std::derived_from<PolicyTs>... OtherPolicyTs>
    constexpr small_polymorphic(small_polymorphic<StorageT, OtherPolicyTs...>&& other) noexcept
        : storage(detail::fn<move_policy<StorageT>, small_polymorphic<StorageT, OtherPolicyTs...>>(
              other, std::move(other.storage))),
          vtable_ptrs(std::to_address<vtable_ptr<OtherPolicyTs>>(other.vtable_ptrs)...) {}

    constexpr ~small_polymorphic() { detail::fn<destroy_policy<StorageT>, small_polymorphic>(*this, storage); }

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
};

template <class T>
inline constexpr bool is_polymorphic_v = false;

template <class StorageT, class... PolicyTs>
inline constexpr bool is_polymorphic_v<small_polymorphic<StorageT, PolicyTs...>> = true;

template <class StorageT, class... PolicyTs>
struct polymorphic_traits<small_polymorphic<StorageT, PolicyTs...>> {
    using polymorphic_type = small_polymorphic<StorageT, PolicyTs...>;

    template <class PolicyT>
    [[nodiscard]] static constexpr auto fn(const polymorphic_type& poly, std::in_place_type_t<PolicyT>) {
        constexpr auto recur = [](auto self, auto first, auto... rest) {
            using first_type = typename decltype(first)::type;
            if constexpr (std::is_base_of_v<PolicyT, first_type>) {
                return first;
            } else if constexpr (sizeof...(rest) > 0) {
                return self(self, rest...);
            }
        };

        using derived_type    = typename decltype(recur(recur, std::type_identity<PolicyTs>{}...))::type;
        using vtable_ptr_type = typename polymorphic_type::template vtable_ptr<derived_type>;
        using vtable_type     = vtable<PolicyT, StorageT>;

        const vtable_ptr_type& vtable_ptr = poly.vtable_ptrs;
        const vtable_type&     vtable     = *vtable_ptr;

        return vtable.fn;
    }

    template <class PolyT>
    [[nodiscard]] static constexpr auto&& storage(PolyT&& poly) {
        return std::forward<PolyT>(poly).storage;
    }
};

// dispatches to storage bindings for nullary policies
template <std::derived_from<nullary_policy> PolicyT, class PolyT, class RetT, class... ArgsT>
    requires is_polymorphic_v<PolyT>
struct impl<PolicyT, PolyT, RetT(ArgsT...)> {
    [[nodiscard]] static constexpr RetT fn(const PolyT& self, ArgsT... args) {
        return polymorphic_traits<PolyT>::fn(self, std::in_place_type<PolicyT>)(std::forward<ArgsT>(args)...);
    }
};

// dispatches to storage bindings for noexcept nullary policies
template <std::derived_from<nullary_policy> PolicyT, class PolyT, class RetT, class... ArgsT>
    requires is_polymorphic_v<PolyT>
struct impl<PolicyT, PolyT, RetT(ArgsT...) noexcept> {
    [[nodiscard]] static constexpr RetT fn(const PolyT& self, ArgsT... args) noexcept {
        return polymorphic_traits<PolyT>::fn(self, std::in_place_type<PolicyT>)(std::forward<ArgsT>(args)...);
    }
};

// dispatches to storage bindings for unary policies
template <std::derived_from<unary_policy> PolicyT, class PolyT, class RetT, class SelfT, class... ArgsT>
    requires is_polymorphic_v<PolyT>
struct impl<PolicyT, PolyT, RetT(SelfT&, ArgsT...)> {
    [[nodiscard]] static constexpr RetT fn(SelfT& self, ArgsT... args) {
        return polymorphic_traits<PolyT>::fn(self, std::in_place_type<PolicyT>)(
            polymorphic_traits<PolyT>::storage(self), std::forward<ArgsT>(args)...);
    }
};

// dispatches to storage bindings for symmetric binary policies
// requires type_policy to be implemented
template <std::derived_from<symmetric_binary_policy> PolicyT, class PolyT, class RetT>
    requires is_polymorphic_v<PolyT>
struct impl<PolicyT, PolyT, RetT(const PolyT&, const PolyT&)> {
    [[nodiscard]] static constexpr RetT fn(const PolyT& self, const PolyT& other) {
        const auto& self_type  = detail::fn<type_policy, PolyT>(self);
        const auto& other_type = detail::fn<type_policy, PolyT>(other);

        // std::type_info::operator== is not constexpr until C++23
        if (std::is_constant_evaluated() ? &self_type != &other_type : self_type != other_type) {
            return PolicyT::default_value();
        }

        return polymorphic_traits<PolyT>::fn(self, std::in_place_type<PolicyT>)(
            polymorphic_traits<PolyT>::storage(self), polymorphic_traits<PolyT>::storage(other));
    }
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_SMALL_POLYMORPHIC_HPP
