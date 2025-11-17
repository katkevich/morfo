#include "doctest_comptime.hpp"
#include <morfo/morfo.hpp>

namespace mrf::test::annotations {
MRF_TEST_CASE_CTRT("class-level annotation moves all non-annotated members into a default bucket") {
    struct[[= mrf::hot]] Person {
        int age = 0;
        std::string_view name;
    };

    mrf::vector<Person> persons;
    persons.push_back(Person{ 19, "Ann" });

    const std::vector<mrf::bucket<Person, mrf::hot>>& bucket = persons.bucket<mrf::hot>();

    MRF_CHECK_EQ(bucket[0].age, 19);
    MRF_CHECK_EQ(bucket[0].name, "Ann");
}

MRF_TEST_CASE_CTRT("absence of the class annotation and member annotation moves member into its own named bucket") {
    struct Person {
        int age = 0;
        std::string_view name;
    };

    mrf::vector<Person> persons;
    persons.push_back(Person{ 19, "Ann" });

    const std::vector<mrf::bucket<Person, "age">>& age_bucket = persons.bucket<"age">();
    const std::vector<mrf::bucket<Person, "name">>& name_bucket = persons.bucket<"name">();

    MRF_CHECK_EQ(age_bucket[0].age, 19);
    MRF_CHECK_EQ(name_bucket[0].name, "Ann");
}

MRF_TEST_CASE_CTRT("members marked with the same member annotation go into the same bucket") {
    struct Person {
        int age = 0;
        [[= mrf::cold]] std::string_view name;
        [[= mrf::cold]] std::string_view surname;
    };

    mrf::vector<Person> persons;
    persons.push_back(Person{ 27, "Ada", "Lovelance" });

    const std::vector<mrf::bucket<Person, "age">>& age_bucket = persons.bucket<"age">();
    const std::vector<mrf::bucket<Person, mrf::cold>>& cold_bucket = persons.bucket<mrf::cold>();

    MRF_CHECK_EQ(age_bucket[0].age, 27);
    MRF_CHECK_EQ(cold_bucket[0].name, "Ada");
    MRF_CHECK_EQ(cold_bucket[0].surname, "Lovelance");
}

MRF_TEST_CASE_CTRT("members annotations override class annotation (basically class annotation define the default)") {
    struct[[= mrf::cold]] Person {
        int age = 0;
        [[= mrf::hot]] std::string_view name;
        std::string_view surname;
    };

    mrf::vector<Person> persons;
    persons.push_back(Person{ 27, "Ada", "Lovelance" });

    const std::vector<mrf::bucket<Person, mrf::cold>>& cold_bucket = persons.bucket<mrf::cold>();
    const std::vector<mrf::bucket<Person, mrf::hot>>& hot_bucket = persons.bucket<mrf::hot>();

    MRF_CHECK_EQ(hot_bucket[0].name, "Ada");
    MRF_CHECK_EQ(cold_bucket[0].age, 27);
    MRF_CHECK_EQ(cold_bucket[0].surname, "Lovelance");
}

MRF_TEST_CASE_CTRT("user-defined bucket tag") {
    struct rare_access;
    struct Person {
        int age = 0;
        std::string_view name;
        [[= mrf::tag<rare_access>]] std::string_view surname;
    };

    mrf::vector<Person> persons;
    persons.push_back(Person{ 27, "Ada", "Lovelance" });

    const std::vector<mrf::bucket<Person, mrf::tag<rare_access>>>& rare_access_bucket = persons.bucket<mrf::tag<rare_access>>();

    MRF_CHECK_EQ(rare_access_bucket[0].surname, "Lovelance");
}
} // namespace mrf::test::annotations