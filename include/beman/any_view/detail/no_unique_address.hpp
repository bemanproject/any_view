// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_NO_UNIQUE_ADDRESS_HPP
#define BEMAN_ANY_VIEW_DETAIL_NO_UNIQUE_ADDRESS_HPP

#ifndef BEMAN_ANY_VIEW_NO_UNIQUE_ADDRESS
    #ifndef __has_cpp_attribute
        #define BEMAN_ANY_VIEW_NO_UNIQUE_ADDRESS
    #elif __has_cpp_attribute(msvc::no_unique_address)
        #define BEMAN_ANY_VIEW_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
    #elif __has_cpp_attribute(no_unique_address)
        #define BEMAN_ANY_VIEW_NO_UNIQUE_ADDRESS [[no_unique_address]]
    #else
        #define BEMAN_ANY_VIEW_NO_UNIQUE_ADDRESS
    #endif // __has_cpp_attribute
#endif     // BEMAN_ANY_VIEW_NO_UNIQUE_ADDRESS

#endif // BEMAN_ANY_VIEW_DETAIL_NO_UNIQUE_ADDRESS_HPP
