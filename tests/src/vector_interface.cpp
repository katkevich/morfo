#include "doctest_comptime.hpp"
#include <morfo/morfo.hpp>
#include <algorithm>
#include <tuple>

namespace mrf::test::vector_interface {

struct Person {
    [[= mrf::hot]] int id = 0;
    [[= mrf::cold]] int age = 0;
    [[= mrf::hot]] std::string_view name = "default_name";
    std::string_view surname = "default_surname";

    auto operator<=>(const Person&) const = default;
};

MRF_TEST_CASE("default constructed mrf::vector should be empty") {
    mrf::vector<Person> persons;
    MRF_REQUIRE(persons.empty());
}

MRF_TEST_CASE("default constructed mrf::vector should have size 0") {
    mrf::vector<Person> persons;
    MRF_REQUIRE_EQ(persons.size(), 0);
}

MRF_TEST_CASE("mrf::vector push_back indeed pushes items to the back") {
    mrf::vector<Person> persons;
    persons.push_back(Person{ 1, 19, "Bob", "Guy" });
    persons.push_back(Person{ 1, 25, "Alice", "Guy" });

    MRF_REQUIRE_EQ(persons.size(), 2);
    MRF_REQUIRE_EQ(persons.front().name, "Bob");
    MRF_REQUIRE_EQ(persons.back().name, "Alice");
}

MRF_TEST_CASE("resize should default-construct items") {
    mrf::vector<Person> persons;
    MRF_CHECK_EQ(persons.size(), 0);

    persons.resize(2);
    MRF_REQUIRE_EQ(persons.size(), 2);
    MRF_REQUIRE_EQ(persons.front().name, "default_name");
    MRF_REQUIRE_EQ(persons.back().name, "default_name");
}

MRF_TEST_CASE("resize with default value should override default-initialilzers of the members") {
    mrf::vector<Person> persons;

    persons.resize(2, Person{ 1, 19, "Ken", "Block" });
    MRF_REQUIRE_EQ(persons.size(), 2);
    MRF_REQUIRE_EQ(persons.front().surname, "Block");
    MRF_REQUIRE_EQ(persons.back().surname, "Block");
}

MRF_TEST_CASE("reserve should increase the capacity but not the size") {
    mrf::vector<Person> persons;
    MRF_CHECK_EQ(persons.capacity(), 0);

    persons.reserve(2);
    MRF_REQUIRE_EQ(persons.capacity(), 2);
    MRF_REQUIRE_EQ(persons.size(), 0);
}

MRF_TEST_CASE("clear should remove all items but keep the capacity unchanged") {
    mrf::vector<Person> persons;
    persons.push_back(Person{ 1, 19, "Bob", "Guy" });
    persons.push_back(Person{ 1, 25, "Alice", "Guy" });
    persons.push_back(Person{ 1, 33, "Jesus", "Christs" });

    MRF_CHECK_EQ(persons.capacity(), 4);
    MRF_CHECK_EQ(persons.size(), 3);

    persons.clear();

    MRF_REQUIRE_EQ(persons.capacity(), 4);
    MRF_REQUIRE_EQ(persons.size(), 0);
}

MRF_TEST_CASE("shrink_to_fit should shrink the capacity to the size of items count") {
    mrf::vector<Person> persons;
    persons.push_back(Person{ 1, 19, "Bob", "Guy" });
    persons.push_back(Person{ 1, 25, "Alice", "Guy" });
    persons.push_back(Person{ 1, 33, "Jesus", "Christs" });

    MRF_CHECK_EQ(persons.capacity(), 4);
    MRF_CHECK_EQ(persons.size(), 3);

    persons.shrink_to_fit();

    MRF_REQUIRE_EQ(persons.capacity(), 3);
    MRF_REQUIRE_EQ(persons.size(), 3);
}

MRF_TEST_CASE("pop_back should remove last element but keep the capacity unchanged") {
    mrf::vector<Person> persons;
    persons.push_back(Person{ 1, 19, "Bob", "Guy" });
    persons.push_back(Person{ 1, 25, "Alice", "Guy" });
    persons.push_back(Person{ 1, 33, "Jesus", "Christs" });

    MRF_CHECK_EQ(persons.capacity(), 4);
    MRF_CHECK_EQ(persons.size(), 3);

    persons.pop_back();

    MRF_REQUIRE_EQ(persons.capacity(), 4);
    MRF_REQUIRE_EQ(persons.size(), 2);
}

MRF_TEST_CASE("swap should swap underlying buckets for two mrf::vector") {
    mrf::vector<Person> persons0;
    persons0.push_back(Person{ 1, 19, "Alice", "Bay" });

    mrf::vector<Person> persons1;
    persons1.push_back(Person{ 1, 19, "Bob", "Guy" });

    std::swap(persons0, persons1);

    MRF_REQUIRE_EQ(persons0.back().name, "Bob");
    MRF_REQUIRE_EQ(persons1.back().name, "Alice");

    persons0.swap(persons1);

    MRF_REQUIRE_EQ(persons0.back().name, "Alice");
    MRF_REQUIRE_EQ(persons1.back().name, "Bob");
}

MRF_TEST_CASE("emplace_back should emplace non-default constructible item") {
    struct Pers {
        Pers() = delete;
        constexpr Pers(int id, std::string name)
            : id(id)
            , name(std::move(name)) {}
        int id{};
        std::string name{};
    };

    mrf::vector<Pers> persons;
    persons.emplace_back(1, "Alice");

    MRF_REQUIRE_EQ(persons.back().name, "Alice");
}

MRF_TEST_CASE("iterate non-const vector") {
    mrf::vector<Person> persons;
    persons.push_back(Person{ 1, 19, "Alice", "Bay" });
    persons.push_back(Person{ 2, 25, "Bob", "Guy" });

    for (mrf::vector<Person>::reference person : persons) {
        person.age += 1;
    }

    std::vector<Person> actual;
    std::ranges::transform(persons, std::back_inserter(actual),
        [](mrf::vector<Person>::reference pers) { return Person{ pers.id, pers.age, pers.name, pers.surname }; });

    std::vector<Person> expected = {
        Person{ 1, 20, "Alice", "Bay" },
        Person{ 2, 26, "Bob", "Guy" },
    };

    MRF_REQUIRE(std::ranges::equal(actual, expected));
}
} // namespace mrf::test::vector_interface