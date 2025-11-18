#include "doctest_comptime.hpp"
#include "doctest_fuzzing.hpp"
#include "morfo/morfo.hpp"
#include <algorithm>

namespace mrf::test::sort {

struct Person {
    int id{};
    std::string name{};

    auto operator<=>(const Person&) const = default;

    friend std::ostream& operator<<(std::ostream& os, const Person& person) {
        os << "{" << person.id << ": " << person.name << "}";
        return os;
    }
};

MRF_FUZZ_TEST_DOMAIN_SEEDED("mrf::introsort: medium size vector in random order using pointer-to-member predicate",
    2317667317,
    fuzz::loop(50),
    fuzz::vector_of<Person>().size(500, 1000))
MRF_FUZZ_TEST_CASE(std::vector<Person> persons) {

    mrf::vector<Person> mrf_persons;
    for (const Person& person : persons) {
        mrf_persons.push_back(person);
    }

    mrf::introsort(mrf_persons, std::less{}, mrf::proj::member<^^Person::id>);
    std::ranges::sort(persons, std::less{}, &Person::id);

    std::vector<Person> actual_sorted;
    std::ranges::transform(mrf_persons, std::back_inserter(actual_sorted), mrf::into);

    std::vector<Person> expected_sorted = persons;

    MRF_REQUIRE_EQ(actual_sorted.size(), expected_sorted.size());
    for (std::size_t i = 0; i < actual_sorted.size(); ++i) {
        CAPTURE(i);
        MRF_CHECK_EQ(actual_sorted[i].id, expected_sorted[i].id);
    }
}


MRF_FUZZ_TEST_DOMAIN("mrf::insertsort: medium size vector in random order using pointer-to-member predicate",
    fuzz::loop(50),
    fuzz::vector_of<Person>().size(500, 1000))
MRF_FUZZ_TEST_CASE(std::vector<Person> persons) {
    mrf::vector<Person> mrf_persons;
    for (const Person& person : persons) {
        mrf_persons.push_back(person);
    }

    mrf::insertsort(mrf_persons, std::less{}, mrf::proj::member<^^Person::id>);
    std::ranges::sort(persons, std::less{}, &Person::id);

    std::vector<Person> actual_sorted;
    std::ranges::transform(mrf_persons, std::back_inserter(actual_sorted), mrf::into);

    std::vector<Person> expected_sorted = persons;

    MRF_REQUIRE_EQ(actual_sorted.size(), expected_sorted.size());
    for (std::size_t i = 0; i < actual_sorted.size(); ++i) {
        CAPTURE(i);
        MRF_CHECK_EQ(actual_sorted[i].id, expected_sorted[i].id);
    }
}
} // namespace mrf::test::sort