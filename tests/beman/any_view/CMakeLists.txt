# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

include(GoogleTest)

function(beman_add_test)
  beman_add_executable(${ARGN} CATEGORY tests LIBRARIES GTest::gtest
                       GTest::gtest_main)
  gtest_discover_tests(${target})
endfunction()

beman_add_test(TARGET concepts SOURCES concepts.test.cpp)
