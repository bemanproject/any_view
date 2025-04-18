# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(ALL_EXAMPLES sum_views)

message("Examples to be built: ${ALL_EXAMPLES}")

foreach(example ${ALL_EXAMPLES})
    add_executable(${example} "")
    target_sources(${example} PRIVATE ${example}.cpp)
    target_link_libraries(${example} beman.any_view)
endforeach()
