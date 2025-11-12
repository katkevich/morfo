cmake_minimum_required(VERSION 3.30)

include(FetchContent)

FetchContent_Declare(doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.12
    GIT_SHALLOW 1
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(doctest)

add_executable(morfo_tests
    src/vector_interface.cpp
    src/annotations.cpp
)
target_sources(morfo_tests PRIVATE
    FILE_SET morfo_tests_headers TYPE HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES doctest_comptime.hpp
)
target_link_libraries(morfo_tests morfo::morfo doctest::doctest_with_main)
target_compile_definitions(morfo_tests PUBLIC
    DOCTEST_CONFIG_VOID_CAST_EXPRESSIONS
)

#
# Make tests discoverable by CTest
#
list(APPEND CMAKE_MODULE_PATH ${doctest_SOURCE_DIR}/scripts/cmake)
include(doctest)
doctest_discover_tests(morfo_tests)