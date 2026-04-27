# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

target_sources(
    beman.any_view
    PUBLIC
        FILE_SET HEADERS
            FILES
                any_view.hpp
                any_view_options.hpp
                concepts.hpp
                reserve_hint.hpp
                detail/adaptors.hpp
                detail/compressed_ptr.hpp
                detail/concepts.hpp
                detail/default_iterator.hpp
                detail/default_view.hpp
                detail/iterator.hpp
                detail/no_unique_address.hpp
                detail/polymorphic.hpp
                detail/polymorphic_iterator.hpp
                detail/polymorphic_view.hpp
                detail/protocols.hpp
                detail/reference_converts_from_temporary.hpp
                detail/small_storage.hpp
                detail/unreachable.hpp
                detail/witness.hpp
)
