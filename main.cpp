#include "morfo/morfo.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <meta>
#include <ranges>
#include <set>
#include <string>
#include <tuple>
#include <type_traits>


// you can't create static constexpr std::span<const Type> local variable (where Type consists of std::meta::info
// members) but you can std::span<std::meta::info> you can't create static constexpr member variable out of define_XXX
// consteval function (which mutates compiler state under the hood) you can't create static constexpr member variable
// which holds std::meta::info AT ALL (meaning you are forced to reevaluate all the thong all the time at compile time)
// you can't really get info (nsdm count for instance) as constexpr value
// where is no way to return std::vector out of consteval function if you don't know the size in advance
// no convenient way to link consteval world and runtime world (you are forced to use Immediately Invoked Lambda
// Expression and NTTP template args) while linking reflection with runtime you are forced to use NTTP EVERYWHERE incomplete
// type what you are defining using c++26 reflection should be in the same scope as a code where you define it (bucket
// should be inside vector)


struct hot;
struct cold;
struct archive;


struct[[= mrf::tag<cold>]] Person {
    [[= mrf::tag<hot>]] int id = 0;

    int age = 0;
    [[= mrf::tag<hot>]] std::string_view name;
    std::string_view surname;
};

struct Stat
{
    std::meta::info type;
    std::meta::info member;
};

consteval int eval(std::meta::info type)
{
    std::vector<std::meta::info> members = mrf::nsdm_of(type);

    mrf::vector<Stat> stats;
    stats.push_back(Stat{ type, type });
    stats.push_back(Stat{ type, type });

    mrf::vector<Person> persons;
    persons.push_back(Person{ 0, 18, "john", "wick" });
    persons.push_back(Person{ 1, 42, "alice", "bonbon" });
    persons.emplace_back(2, 33, "bob", "gor");

    persons.front().name;
    persons.back().name;
    persons[1].name;
    persons.at(2).name;
    persons.empty();

    persons.capacity();
    persons.reserve(9);
    persons.shrink_to_fit();
    persons.pop_back();
    persons.resize(4, Person{ 5, 5, "igor", "gal" });

    for (mrf::vector<Person>::reference person : persons) {
        person.age, person.name;
    }

    return stats.size();
}

template <typename T>
struct foo;

int main() {
    mrf::vector<Person> persons;
    persons.push_back(Person{ 0, 18, "john", "wick" });
    persons.push_back(Person{ 1, 42, "alice", "bonbon" });
    persons.emplace_back(2, 33, "bob", "gor");

    std::cout << "front: " << persons.front().name << "\n";
    std::cout << "back: " << persons.back().name << "\n";
    std::cout << "[1]: " << persons[1].name << "\n";
    std::cout << "at(2): " << persons.at(2).name << "\n";
    std::cout << "empty: " << persons.empty() << "\n";

    std::cout << "capacity before reserve: " << persons.capacity() << "\n";
    persons.reserve(9);
    std::cout << "capacity after reserve(9): " << persons.capacity() << "\n";
    persons.shrink_to_fit();
    std::cout << "capacity after shrink: " << persons.capacity() << "\n";
    persons.pop_back();
    std::cout << "back() after pop_back: " << persons.back().name << "\n";
    persons.resize(4, Person{ 5, 5, "igor", "gal" });
    std::cout << "back() after resize: " << persons.back().name << "\n";

    for (mrf::vector<Person>::reference person : persons) {
        std::cout << person.age << " " << person.name << "\n";
    }

    const mrf::vector<Person> const_persons = persons;
    for (mrf::vector<Person>::const_reference person : const_persons) {
        std::cout << person.age << " " << person.name << "\n";
    }

    static_assert(std::random_access_iterator<mrf::vector<Person>::const_iterator>, "should satisfy random iterator");

    constexpr int result = eval(^^Person);

    std::cout << "eval result: " << result << "\n";

    return result;
}