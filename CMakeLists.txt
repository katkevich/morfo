cmake_minimum_required(VERSION 3.30)

project(morfo VERSION 0.5)

set(MORFO_HEADERS
    include/morfo/morfo.hpp
    include/morfo/vector.hpp
    include/morfo/annotation.hpp
    include/morfo/misc/static_vector.hpp
    include/morfo/misc/static_map.hpp
    include/morfo/misc/unordered_map.hpp
    include/morfo/misc/unordered_set.hpp
    include/morfo/misc/algorithm.hpp
)

add_executable(morfo main.cpp)
target_sources(morfo PUBLIC
    FILE_SET morfo_headers_set TYPE HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES ${MORFO_HEADERS}
)
target_compile_options(morfo PRIVATE
    $<$<CXX_COMPILER_ID:Clang>: -std=c++26 -fexpansion-statements -freflection-latest -Wno-unused-value>
)