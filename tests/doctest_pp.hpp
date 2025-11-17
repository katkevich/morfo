#pragma once

#define MRF_VA_ARGS_COUNT_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define MRF_VA_ARGS_COUNT(...) MRF_VA_ARGS_COUNT_IMPL(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define MRF_CONCAT_IMPL(a, b) a##b
#define MRF_CONCAT(a, b) MRF_CONCAT_IMPL(a, b)

// clang-format off
#define MRF_FOR_EACH_1(macro, sep, a) macro(a)
#define MRF_FOR_EACH_2(macro, sep, a, b) macro(a) sep() macro(b)
#define MRF_FOR_EACH_3(macro, sep, a, b, c) macro(a) sep() macro(b) sep() macro(c)
#define MRF_FOR_EACH_4(macro, sep, a, b, c, d) macro(a) sep() macro(b) sep() macro(c) sep() macro(d)
#define MRF_FOR_EACH_5(macro, sep, a, b, c, d, e) macro(a) sep() macro(b) sep() macro(c) sep() macro(d) sep() macro(e)
#define MRF_FOR_EACH_6(macro, sep, a, b, c, d, e, f) macro(a) sep() macro(b) sep() macro(c) sep() macro(d) sep() macro(e) sep() macro(f)
#define MRF_FOR_EACH_7(macro, sep, a, b, c, d, e, f, g) macro(a) sep() macro(b) sep() macro(c) sep() macro(d) sep() macro(e) sep() macro(f) sep() macro(g)
#define MRF_FOR_EACH_8(macro, sep, a, b, c, d, e, f, g, h) macro(a) sep() macro(b) sep() macro(c) sep() macro(d) sep() macro(e) sep() macro(f) sep() macro(g) sep() macro(h)
#define MRF_FOR_EACH_9(macro, sep, a, b, c, d, e, f, g, h, i) macro(a) sep() macro(b) sep() macro(c) sep() macro(d) sep() macro(e) sep() macro(f) sep() macro(g) sep() macro(h) sep() macro(i)
#define MRF_FOR_EACH_10(macro, sep, a, b, c, d, e, f, g, h, i, j) macro(a) sep() macro(b) sep() macro(c) sep() macro(d) sep() macro(e) sep() macro(f) sep() macro(g) sep() macro(h) sep() macro(i) sep() macro(j)
// clang-format on

#define MRF_FOR_EACH(macro, sep, ...) MRF_CONCAT(MRF_FOR_EACH_, MRF_VA_ARGS_COUNT(__VA_ARGS__))(macro, sep, __VA_ARGS__)

#define MRF_COMMA() ,