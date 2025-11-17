#include "doctest_comptime.hpp"
#include "doctest_fuzzing.hpp"
#include "morfo/morfo.hpp"
#include <algorithm>

namespace mrf::test::sort {

struct Person {
    int id{};
    std::string name{};

    auto operator<=>(const Person&) const = default;
};

MRF_FUZZ_TEST_DOMAIN("mrf::introsort: medium size vector in random order using pointer-to-member predicate",
    fuzz::loop(50),
    fuzz::vector_of<Person>().size(500, 1000))
MRF_FUZZ_TEST_CASE(std::vector<Person> persons) {

    mrf::vector<Person> mrf_persons;
    for (const Person& person : persons) {
        mrf_persons.push_back(person);
    }

    struct Proj {
        constexpr auto operator()(mrf::vector<Person>& vec, std::size_t idx) {
            return vec[idx].id;
        }

        constexpr auto operator()(mrf::vector<Person>::reference& ref) {
            return ref.id;
        }
    };

    mrf::introsort(mrf_persons, 0, std::ssize(persons), std::less{}, Proj{});
    std::ranges::sort(persons, std::less{}, &Person::id);

    std::vector<Person> actual_sorted;
    std::ranges::transform(mrf_persons, std::back_inserter(actual_sorted), mrf::into);

    std::vector<Person> expected_sorted = persons;

    MRF_CHECK_EQ(actual_sorted, expected_sorted);
}


MRF_FUZZ_TEST_DOMAIN("mrf::insertsort: medium size vector in random order using pointer-to-member predicate",
    fuzz::loop(50),
    fuzz::vector_of<Person>().size(500, 1000))
MRF_FUZZ_TEST_CASE(std::vector<Person> persons) {
    mrf::vector<Person> mrf_persons;
    for (const Person& person : persons) {
        mrf_persons.push_back(person);
    }

    struct Proj {
        constexpr auto operator()(mrf::vector<Person>& vec, std::size_t idx) {
            return vec[idx].id;
        }

        constexpr auto operator()(mrf::vector<Person>::reference& ref) {
            return ref.id;
        }
    };

    mrf::insertsort(mrf_persons, 0, std::ssize(persons), std::less{}, Proj{});
    std::ranges::sort(persons, std::less{}, &Person::id);

    std::vector<Person> actual_sorted;
    std::ranges::transform(mrf_persons, std::back_inserter(actual_sorted), mrf::into);

    std::vector<Person> expected_sorted = persons;

    MRF_CHECK_EQ(actual_sorted, expected_sorted);
}
} // namespace mrf::test::sort