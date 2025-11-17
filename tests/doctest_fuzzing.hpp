#pragma once
#include "doctest_pp.hpp"
#include <cstddef>
#include <random>
#include <string>
#include <tuple>
#include <vector>

namespace mrf::fuzz {

struct lcg_random_engine {
    static constexpr std::uint64_t lcg_mult = 6364136223846793005ULL;
    static constexpr std::uint64_t lcg_inc = 1442695040888963407ULL;

    constexpr lcg_random_engine(std::uint64_t seed)
        : state(seed) {}

    constexpr std::uint64_t operator()() noexcept {
        state = (lcg_mult * state) + lcg_inc;
        return state;
    }

    std::uint64_t state{};
};

struct runtime_random_source {
    runtime_random_source()
        : runtime_random_source(random_device()) {}
    runtime_random_source(std::uint64_t seed)
        : seed(seed)
        , random_engine(seed) {}

    std::size_t gen() {
        std::uniform_int_distribution<std::size_t> distrib;

        return distrib(random_engine);
    }

    static inline std::random_device random_device{};

    std::uint64_t seed{};
    std::mt19937 random_engine;
};

struct comptime_random_source {
    constexpr comptime_random_source()
        : comptime_random_source(generate_seed()) {}
    constexpr comptime_random_source(std::uint64_t seed)
        : seed(seed)
        , random_engine(seed) {}

    constexpr std::size_t gen() {
        return random_engine();
    }

    static constexpr std::uint64_t generate_seed() {
        constexpr char inits[] = __TIME__;

        constexpr std::uint64_t s =     //
            (inits[0] - '0') * 100000 + //
            (inits[1] - '0') * 10000 +  //
            (inits[3] - '0') * 1000 +   //
            (inits[4] - '0') * 100 +    //
            (inits[6] - '0') * 10 +     //
            (inits[7] - '0');

        return s;
    }

    std::uint64_t seed{};
    lcg_random_engine random_engine;
};

namespace domain {
struct size {
    using result_type = std::size_t;

    template <typename TRandomSource>
    constexpr result_type gen(TRandomSource& random_source) const {
        return min + random_source.gen() % (max - min + 1);
    }
    std::size_t min{};
    std::size_t max{};
};

template <typename TValue>
struct any;

template <typename... TDomains>
struct pack {
    template <typename TItem>
    constexpr auto select_for() const {
        template for (auto domain : domains) {
            if constexpr (std::is_same_v<typename decltype(domain)::result_type, TItem>) {
                return domain;
            }
        }

        return domain::any<TItem>{};
    }

    std::tuple<TDomains...> domains{};
};

template <typename TValue>
struct any {
    static_assert(std::is_aggregate_v<TValue>);
    static_assert(std::is_default_constructible_v<TValue>);

    using result_type = TValue;

    template <typename TRandomSource, typename... OtherDomains>
    constexpr result_type gen(TRandomSource& random_source, domain::pack<OtherDomains...> other_domains) const {
        TValue value{};

        auto& [... members] = value;
        ((members = domain::any<std::remove_cvref_t<decltype(members)>>{}.gen(random_source, other_domains)), ...);

        return value;
    }
};

template <>
struct any<int> {
    using result_type = int;

    template <typename TRandomSource, typename... OtherDomains>
    constexpr result_type gen(TRandomSource& random_source, domain::pack<OtherDomains...>) const {
        return result_type(random_source.gen() % std::numeric_limits<result_type>::max());
    }
};

template <>
struct any<std::string> {
    using result_type = std::string;

    template <typename TRandomSource, typename... OtherDomains>
    constexpr result_type gen(TRandomSource& random_source, domain::pack<OtherDomains...>) const {
        const auto min_length = 1;
        const auto max_length = 24;
        const auto length = (random_source.gen() % (max_length - min_length)) + min_length;

        result_type str;
        str.resize(length);

        constexpr auto printable_range_begin = 'a';
        constexpr auto printable_range = 'z' - printable_range_begin;

        for (char& ch : str) {
            const auto rand_ch = char(random_source.gen() % printable_range);
            ch = rand_ch + printable_range_begin;
        }

        return str;
    }
};

struct no_sort {};

template <template <typename...> typename TContainer, typename TItem, typename TSortOrder, typename... TValueDomains>
struct container {
    using result_type = TContainer<TItem>;

    template <typename TRandomSource>
    constexpr result_type gen(TRandomSource& random_source) const {
        const auto size = size_domain.gen(random_source);
        result_type output{};

        for (std::size_t i = 0; i < size; ++i) {
            const auto value_domain = value_domains.template select_for<TItem>();
            output.insert(output.end(), value_domain.gen(random_source, value_domains));
        }

        return output;
    }

    constexpr auto iterations(std::size_t n) {
        iters = n;
        return *this;
    }

    constexpr auto size(std::size_t min, std::size_t max) {
        size_domain = domain::size{ min, max };
        return *this;
    }

    template <typename USortOrder = std::less<>>
    constexpr auto sorted(USortOrder order = {}) {
        return container<TContainer, TItem, USortOrder, TValueDomains...>{ value_domains, order };
    }

    domain::pack<TValueDomains...> value_domains{};
    TSortOrder sort_order{};

    domain::size size_domain{ 0, 10 };
    std::size_t iters{ 10 };
};
} // namespace domain

static constexpr auto size(std::size_t min, std::size_t max) {
    return domain::size{ min, max };
}

template <typename TValue>
static constexpr auto any() {
    return domain::any<TValue>{};
}

template <typename TValue = std::string>
static constexpr auto name() {
    return domain::any<TValue>{};
}

template <typename TItem, typename... TValueDomains>
static constexpr auto vector_of(TValueDomains... value_domains) {
    return domain::container<std::vector, TItem, domain::no_sort, TValueDomains...>{
        .value_domains = { std::tuple{ value_domains... } },
    };
}

static constexpr auto loop(int count) {
    return count;
}
} // namespace mrf::fuzz


#define MRF_FUZZ_TEST_DOMAIN_RESULT_TYPE_IMPL(expr) typename decltype(expr)::result_type

#define MRF_IMPL_FUZZ_TEST_DOMAIN_CT(f, name, loop_count, ...)                                            \
    static constexpr void f(MRF_FOR_EACH(MRF_FUZZ_TEST_DOMAIN_RESULT_TYPE_IMPL, MRF_COMMA, __VA_ARGS__)); \
    template <auto Fn>                                                                                    \
    void DOCTEST_CAT(f, _comptime)() {                                                                    \
        [[maybe_unused]] constexpr auto _ = [] {                                                          \
            mrf::fuzz::comptime_random_source random_source{};                                            \
            auto domain_instances_tuple = std::tuple{ __VA_ARGS__ };                                      \
            auto& [... domain_instances] = domain_instances_tuple;                                        \
            for (int i = 0; i < loop_count; ++i) {                                                        \
                Fn(domain_instances.gen(random_source)...);                                               \
            }                                                                                             \
            return 0;                                                                                     \
        }();                                                                                              \
    }                                                                                                     \
    DOCTEST_REGISTER_FUNCTION(DOCTEST_EMPTY, DOCTEST_CAT(f, _comptime) < f >, "[fuzzing] [ct] " name)     \
    static constexpr void f

#define MRF_IMPL_FUZZ_TEST_DOMAIN_RT(f, name, loop_count, ...)                                            \
    static constexpr void f(MRF_FOR_EACH(MRF_FUZZ_TEST_DOMAIN_RESULT_TYPE_IMPL, MRF_COMMA, __VA_ARGS__)); \
    template <auto Fn>                                                                                    \
    void DOCTEST_CAT(f, _runtime)() {                                                                     \
        mrf::fuzz::runtime_random_source random_source{};                                                 \
        CAPTURE(random_source.seed);                                                                      \
        auto domain_instances_tuple = std::tuple{ __VA_ARGS__ };                                          \
        auto& [... domain_instances] = domain_instances_tuple;                                            \
        for (int i = 0; i < loop_count; ++i) {                                                            \
            Fn(domain_instances.gen(random_source)...);                                                   \
        }                                                                                                 \
    }                                                                                                     \
    DOCTEST_REGISTER_FUNCTION(DOCTEST_EMPTY, DOCTEST_CAT(f, _runtime) < f >, "[fuzzing] " name)           \
    static constexpr void f


#define MRF_FUZZ_TEST_DOMAIN_CT(name, loop_count, ...) \
    MRF_IMPL_FUZZ_TEST_DOMAIN_CT(DOCTEST_ANONYMOUS(DOCTEST_ANON_FUNC_), name, loop_count, __VA_ARGS__)

#define MRF_FUZZ_TEST_DOMAIN_RT(name, loop_count, ...) \
    MRF_IMPL_FUZZ_TEST_DOMAIN_RT(DOCTEST_ANONYMOUS(DOCTEST_ANON_FUNC_), name, loop_count, __VA_ARGS__)

#define MRF_FUZZ_TEST_DOMAIN(name, loop_count, ...) MRF_FUZZ_TEST_DOMAIN_RT(name, loop_count, __VA_ARGS__)
#define MRF_FUZZ_TEST_CASE(...) (__VA_ARGS__)
