#pragma once
#include <doctest/doctest.h>

#define MRF_CHECK(expr) \
    if consteval {      \
        (bool)(expr);   \
    } else {            \
        CHECK(expr);    \
    }

#define MRF_CHECK_EQ(actual, expected) \
    if consteval {                     \
        (void)(actual == expected);    \
    } else {                           \
        CHECK_EQ(actual, expected);    \
    }

#define MRF_CHECK_NE(actual, expected) \
    if consteval {                     \
        (void)(actual != expected);    \
    } else {                           \
        CHECK_NE(actual, expected);    \
    }

#define MRF_REQUIRE(expr) \
    if consteval {        \
        (bool)(expr);     \
    } else {              \
        REQUIRE(expr);    \
    }

#define MRF_REQUIRE_EQ(actual, expected) \
    if consteval {                       \
        (void)(actual == expected);      \
    } else {                             \
        REQUIRE_EQ(actual, expected);    \
    }

#define MRF_REQUIRE_NE(actual, expected) \
    if consteval {                       \
        (void)(actual != expected);      \
    } else {                             \
        REQUIRE_NE(actual, expected);    \
    }

#define MRF_REQUIRE_LT(actual, expected) \
    if consteval {                       \
        (void)(actual < expected);       \
    } else {                             \
        REQUIRE_LT(actual, expected);    \
    }

#define MRF_REQUIRE_LE(actual, expected) \
    if consteval {                       \
        (void)(actual <= expected);      \
    } else {                             \
        REQUIRE_LE(actual, expected);    \
    }

#define MRF_REQUIRE_GT(actual, expected) \
    if consteval {                       \
        (void)(actual > expected);       \
    } else {                             \
        REQUIRE_GT(actual, expected);    \
    }

#define MRF_REQUIRE_GE(actual, expected) \
    if consteval {                       \
        (void)(actual >= expected);      \
    } else {                             \
        REQUIRE_GE(actual, expected);    \
    }

#define MRF_TEST_CASE_CTRT_IMPL(f, name)                                                    \
    static constexpr void f();                                                              \
    template <auto Fn>                                                                      \
    void DOCTEST_CAT(f, _comptime)() {                                                      \
        [[maybe_unused]] constexpr int _ = (Fn(), 0);                                       \
    }                                                                                       \
    template <auto Fn>                                                                      \
    void DOCTEST_CAT(f, _runtime)() {                                                       \
        Fn();                                                                               \
    }                                                                                       \
    DOCTEST_REGISTER_FUNCTION(DOCTEST_EMPTY, DOCTEST_CAT(f, _runtime) < f >, name)          \
    DOCTEST_REGISTER_FUNCTION(DOCTEST_EMPTY, DOCTEST_CAT(f, _comptime) < f >, "[ct] " name) \
    static constexpr void f()

#define MRF_TEST_CASE_CT_IMPL(f, name)                                                      \
    static constexpr void f();                                                              \
    template <auto Fn>                                                                      \
    void DOCTEST_CAT(f, _comptime)() {                                                      \
        [[maybe_unused]] constexpr int _ = (Fn(), 0);                                       \
    }                                                                                       \
    DOCTEST_REGISTER_FUNCTION(DOCTEST_EMPTY, DOCTEST_CAT(f, _comptime) < f >, "[ct] " name) \
    static constexpr void f()

/* Run test in both runtime and compile time. */
#define MRF_TEST_CASE_CTRT(name) MRF_TEST_CASE_CTRT_IMPL(DOCTEST_ANONYMOUS(DOCTEST_ANON_FUNC_), name)

/* Run test in compile time. */
#define MRF_TEST_CASE_CT(name) MRF_TEST_CASE_CT_IMPL(DOCTEST_ANONYMOUS(DOCTEST_ANON_FUNC_), name)

/* Run test in runtime. */
#define MRF_TEST_CASE_RT(name) TEST_CASE(name)
