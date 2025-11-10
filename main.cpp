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
// Expression and NTTP template args) while linking reflection with runtime you are forced to use NTTP EVERYWHERE
// incomplete type what you are defining using c++26 reflection should be in the same scope as a code where you define
// it (bucket should be inside vector)
/* this works:
        constexpr auto ans = define_static_array(annotations_of(^^S));
        constexpr auto type = type_of(ans[0]);
        using T = [:type:];

    this is not:
        const auto ans = annotations_of(^^S);
        const auto type = type_of(ans[0]);
        using T = [:type:];


    OR

    this works:
    constexpr auto tag = mrf::find_template_var(annotations_of(^^T), ^^mrf::bucket_tag);
    this doesn't:
    constexpr auto tag = mrf::find_template_var(annotations_of(outer_function_argument), ^^mrf::bucket_tag);

    wtf??? DO WE HAVE constexpr function arguments???
*/

struct Person {
    [[= mrf::cold]] int id = 0;
    [[= mrf::hot]] int age = 0;
    [[= mrf::cold]] std::string_view name;
    std::string_view surname;
};

constexpr int eval() {
    constexpr auto id = mrf::nsdm_of(^^Person)[0];
    constexpr auto anns = define_static_array(annotations_of(id));
    constexpr auto ann = anns[0];
    static_assert(template_of(type_of(ann)) == ^^mrf::bucket_tag);
    
    constexpr auto ann_val = extract<typename[:type_of(ann):]>(ann);
    static_assert(std::is_same_v<decltype(ann_val), const mrf::bucket_tag<mrf::cold_tag>>);

    constexpr auto bucket_type_1 = substitute(^^mrf::vector<Person>::template bucket_type, { ^^mrf::cold });
    constexpr auto bucket_type_2 = substitute(^^mrf::vector<Person>::template bucket_type, { ^^ann_val });
    constexpr auto bucket_type_3 = substitute(^^mrf::vector<Person>::template bucket_type, { std::meta::reflect_constant(ann_val) });
    using bucket_t = mrf::vector<Person>::template bucket_type<mrf::bucket_tag<mrf::cold_tag>{}>;

    static_assert(is_same_type(bucket_type_1, dealias(^^bucket_t)));
    static_assert(is_same_type(bucket_type_2, dealias(^^bucket_t)));
    static_assert(is_same_type(bucket_type_3, dealias(^^bucket_t)));
    mrf::vector<Person>::template bucket_type<mrf::bucket_tag<mrf::cold_tag>{}> vec;

    template for (constexpr auto ann : anns)
    {
        static_assert(template_of(type_of(ann)) == ^^mrf::bucket_tag);
        constexpr auto ann_val = extract<typename[:type_of(ann):]>(ann);
        static_assert(std::is_same_v<decltype(ann_val), const mrf::bucket_tag<mrf::cold_tag>>);
    }

    Person person;
    mrf::vector<Person>::reference r{ person.id, person.age, person.name, person.surname };
    r.id;
    r.age;
    r.name;
    r.surname;
    mrf::vector<Person>::const_reference cr{ person.id, person.age, person.name, person.surname };

    mrf::vector<Person> pers;
    pers.push_back(Person{ 1, 32, "john", "bay" });

    return 0;
}

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

    const std::vector<mrf::bucket<Person, mrf::cold>>& bucket_cold = persons.bucket<mrf::cold>();
    std::cout << "bucket[0]: " << bucket_cold[0].id << " " << bucket_cold[0].name << "\n";

    // const std::vector<mrf::bucket<Person, mrf::bucket_id<"surname">>>& bucket_surname = persons.bucket<"surname">();
    const auto& bucket_surname = persons.bucket<"surname">();
    std::cout << "bucket_surname[0]: " << bucket_surname[0].surname << "\n";

    constexpr static std::array<char, 3> ArrStr = {'h', 'i', '\0'};
    constexpr char const* Str = ArrStr.data();

    constexpr auto ret = eval();
    auto ret2 = eval();

    return ret;
}