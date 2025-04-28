# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

function(beman_add_examples)
    foreach(name ${ARGN})
        beman_add_executable(
                TARGET ${name}
                SOURCES ${name}.cpp
                CATEGORY examples
        )
    endforeach()
endfunction()

beman_add_examples(
    istream
    sum_views
)
