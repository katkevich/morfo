#include "doctest_comptime.hpp"
#include <morfo/morfo.hpp>

namespace mrf::test::mixin {
struct Obj {
    int id = 0;
    std::string_view name;
};

TEST_CASE("mrf::vector reference copy assign from another reference") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 5, "5" });
    objs.push_back(Obj{ 2, "2" });

    objs[0].from(objs[1]);

    MRF_REQUIRE_EQ(objs[0], objs[1]);
}
} // namespace mrf::test::mixin