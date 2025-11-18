cmake_minimum_required(VERSION 3.30)

project(morfo VERSION 0.5)

set(MORFO_HEADERS
    include/morfo/morfo.hpp
    include/morfo/vector.hpp
    include/morfo/bucket.hpp
    include/morfo/mixin.hpp
    include/morfo/projection.hpp
    include/morfo/algorithm.hpp
    include/morfo/misc/static_vector.hpp
    include/morfo/misc/static_map.hpp
    include/morfo/misc/unordered_map.hpp
    include/morfo/misc/unordered_set.hpp
    include/morfo/misc/algorithm.hpp
)

add_library(morfo INTERFACE)
add_library(morfo::morfo ALIAS morfo)
target_sources(morfo INTERFACE
    FILE_SET morfo_headers_set TYPE HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES ${MORFO_HEADERS}
)
target_compile_options(morfo INTERFACE
    $<$<CXX_COMPILER_ID:Clang>:-std=c++26 -fexpansion-statements -freflection-latest -Wno-unused-value>
)

enable_testing()
add_subdirectory(tests EXCLUDE_FROM_ALL)
