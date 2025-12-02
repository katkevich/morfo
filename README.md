# morfo
SoA (Struct Of Arrays) library using C++26 reflection. Drop-in replacement for standard containers (such as std::vector).


### Quick example
```c++
// mrf::cold here is the default bucket (for "cold" data). Person::age and Person::surname will get here
struct[[= mrf::cold]] Person {
    int age = 0;
    [[= mrf::hot]] std::string_view name; // this member will be put in a different bucket (for "hot" data)
    std::string_view surname;
};

mrf::vector<Person> persons;
persons.push_back(Person{ 27, "Ada", "Lovelance" });

const std::vector<mrf::bucket<Person, mrf::cold>>& cold_bucket = persons.bucket<mrf::cold>();
const std::vector<mrf::bucket<Person, mrf::hot>>& hot_bucket = persons.bucket<mrf::hot>();
```

If you do not specify a _default_ bucket then each non-annotated member will be put in a separate bucket:

```c++
// Note: no default bucket for this struct
struct Person {
    int age = 0;                             // 'age' will be put in a separate named bucket (
    [[= mrf::hot]] std::string_view name;    // `name` and `surname` will be put in "hot" bucket
    [[= mrf::hot]] std::string_view surname;
};

mrf::vector<Person> persons;
persons.push_back(Person{ 27, "Ada", "Lovelance" });

const std::vector<mrf::bucket<Person, ^^Person::age>>& age_bucket = persons.bucket<^^Person::age>(); // separate "named" bucket
const std::vector<mrf::bucket<Person, mrf::hot>>& hot_bucket = persons.bucket<mrf::hot>();           // "hot" bucket
```
