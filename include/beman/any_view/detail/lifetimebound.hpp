// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_LIFETIMEBOUND_HPP
#define BEMAN_ANY_VIEW_DETAIL_LIFETIMEBOUND_HPP

#ifndef BEMAN_ANY_VIEW_LIFETIMEBOUND
    #ifdef __has_cpp_attribute
        #if __has_cpp_attribute(clang::lifetimebound)
            #define BEMAN_ANY_VIEW_LIFETIMEBOUND [[clang::lifetimebound]]
        #elif __has_cpp_attribute(msvc::lifetimebound)
            #define BEMAN_ANY_VIEW_LIFETIMEBOUND [[msvc::lifetimebound]]
        #endif
    #endif // __has_cpp_attribute
#endif     // BEMAN_ANY_VIEW_LIFETIMEBOUND

#endif // BEMAN_ANY_VIEW_DETAIL_LIFETIMEBOUND_HPP
