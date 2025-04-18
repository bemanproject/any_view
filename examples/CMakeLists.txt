# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

function(beman_add_example)
    beman_add_executable(${ARGN} CATEGORY examples)
endfunction()

beman_add_example(TARGET sum_views)
