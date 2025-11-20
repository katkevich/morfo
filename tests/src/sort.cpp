#include "doctest_comptime.hpp"
#include "doctest_fuzzing.hpp"
#include "morfo/morfo.hpp"
#include <algorithm>

namespace mrf::test::sort {

struct Person {
    [[= mrf::hot]] int id{};
    [[= mrf::hot]] std::string name{};
    int age{};

    auto operator<=>(const Person&) const = default;
};

MRF_FUZZ_TEST_DOMAIN("mrf::introsort: medium size vector in random order using `proj::member` projection",
    fuzz::loop(50),
    fuzz::vector_of<Person>().size(500, 1000))
MRF_FUZZ_TEST_CASE(std::vector<Person> persons) {

    mrf::vector<Person> mrf_persons;
    std::ranges::transform(persons, std::back_inserter(mrf_persons), mrf::from);

    mrf::introsort(mrf_persons, std::less{}, mrf::proj::member<^^Person::id>);
    std::ranges::sort(persons, std::less{}, &Person::id);

    std::vector<Person> actual_sorted;
    std::ranges::transform(mrf_persons, std::back_inserter(actual_sorted), mrf::into);

    std::vector<Person> expected_sorted = persons;

    MRF_REQUIRE(std::ranges::equal(actual_sorted, expected_sorted, std::equal_to{}, &Person::id, &Person::id));
}


MRF_FUZZ_TEST_DOMAIN("mrf::insertsort: medium size vector in random order using `proj::member` projection",
    fuzz::loop(50),
    fuzz::vector_of<Person>().size(500, 1000))
MRF_FUZZ_TEST_CASE(std::vector<Person> persons) {
    mrf::vector<Person> mrf_persons;
    std::ranges::transform(persons, std::back_inserter(mrf_persons), mrf::from);

    mrf::insertsort(mrf_persons, std::less{}, mrf::proj::member<^^Person::id>);
    std::ranges::sort(persons, std::less{}, &Person::id);

    std::vector<Person> actual_sorted;
    std::ranges::transform(mrf_persons, std::back_inserter(actual_sorted), mrf::into);

    std::vector<Person> expected_sorted = persons;

    MRF_REQUIRE(std::ranges::equal(actual_sorted, expected_sorted, std::equal_to{}, &Person::id, &Person::id));
}

MRF_FUZZ_TEST_DOMAIN("mrf::insertsort: medium size vector in random order using `proj::bucket<tag>` projection",
    fuzz::loop(50),
    fuzz::vector_of<Person>().size(500, 1000))
MRF_FUZZ_TEST_CASE(std::vector<Person> persons) {
    mrf::vector<Person> mrf_persons;
    std::ranges::transform(persons, std::back_inserter(mrf_persons), mrf::from);

    const auto hot_proj = [](const Person& p) { return std::tie(p.id, p.name); };

    mrf::insertsort(mrf_persons, std::less{}, mrf::proj::bucket<mrf::hot>);
    std::ranges::sort(persons, std::less{}, hot_proj);

    std::vector<Person> actual_sorted;
    std::ranges::transform(mrf_persons, std::back_inserter(actual_sorted), mrf::into);

    std::vector<Person> expected_sorted = persons;

    MRF_REQUIRE(std::ranges::equal(actual_sorted, expected_sorted, std::equal_to{}, hot_proj, hot_proj));
}

MRF_FUZZ_TEST_DOMAIN(
    "mrf::insertsort: medium size vector in random order using `proj::bucket<^^Obj::member>` projection",
    fuzz::loop(50),
    fuzz::vector_of<Person>().size(500, 1000))
MRF_FUZZ_TEST_CASE(std::vector<Person> persons) {
    mrf::vector<Person> mrf_persons;
    std::ranges::transform(persons, std::back_inserter(mrf_persons), mrf::from);

    mrf::insertsort(mrf_persons, std::less{}, mrf::proj::bucket<^^Person::age>);
    std::ranges::sort(persons, std::less{}, &Person::age);

    std::vector<Person> actual_sorted;
    std::ranges::transform(mrf_persons, std::back_inserter(actual_sorted), mrf::into);

    std::vector<Person> expected_sorted = persons;

    MRF_REQUIRE(std::ranges::equal(actual_sorted, expected_sorted, std::equal_to{}, &Person::age, &Person::age));
}
} // namespace mrf::test::sort