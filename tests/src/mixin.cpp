#include "doctest_comptime.hpp"
#include <morfo/morfo.hpp>

namespace mrf::test::mixin {
struct Obj {
    int id = 0;
    std::string name;
};

struct OtherObj {
    int i = 0;
    std::string n;
};

MRF_TEST_CASE_CTRT("mrf::vector copy reference into original value") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    objs.push_back(Obj{ 2, "2222222222222222222222222" });

    mrf::vector<Obj>::reference obj_0 = objs[0];
    mrf::vector<Obj>::reference obj_1 = objs[1];

    {
        Obj o0 = obj_0.into();
        MRF_REQUIRE_EQ(obj_0.name, o0.name);
    }
    {
        Obj o0 = std::move(obj_0).into();
        MRF_REQUIRE_EQ(obj_0.name, o0.name);
    }
}

MRF_TEST_CASE_CTRT("mrf::vector steal reference into original value") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    objs.push_back(Obj{ 2, "2222222222222222222222222" });

    mrf::vector<Obj>::reference obj_0 = objs[0];
    mrf::vector<Obj>::reference obj_1 = objs[1];

    {
        Obj o0 = obj_0.steal_into();
        MRF_REQUIRE_EQ(o0.name, "1111111111111111111111111");
        MRF_REQUIRE(obj_0.name.empty());
    }
    {
        Obj o1 = std::move(obj_1).steal_into();
        MRF_REQUIRE_EQ(o1.name, "2222222222222222222222222");
        MRF_REQUIRE(obj_1.name.empty());
    }
}

MRF_TEST_CASE_CTRT("mrf::vector forward reference into original value") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    objs.push_back(Obj{ 2, "2222222222222222222222222" });

    mrf::vector<Obj>::reference obj_0 = objs[0];
    mrf::vector<Obj>::reference obj_1 = objs[1];

    {
        Obj o0 = obj_0.forward_into();
        MRF_REQUIRE_EQ(obj_0.name, o0.name);
    }
    {
        Obj o1 = std::move(obj_1).forward_into();
        MRF_REQUIRE_EQ(o1.name, "2222222222222222222222222");
        MRF_REQUIRE(obj_1.name.empty());
    }
}

MRF_TEST_CASE_CTRT("mrf::vector copy reference into value of a different type but with the same layout") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    objs.push_back(Obj{ 2, "2222222222222222222222222" });

    mrf::vector<Obj>::reference obj_0 = objs[0];
    mrf::vector<Obj>::reference obj_1 = objs[1];

    {
        OtherObj o0 = obj_0.into<OtherObj>();
        MRF_REQUIRE_EQ(obj_0.name, o0.n);
    }
    {
        OtherObj o0 = std::move(obj_0).into<OtherObj>();
        MRF_REQUIRE_EQ(obj_0.name, o0.n);
    }
}

MRF_TEST_CASE_CTRT("mrf::vector steal reference into value of a different type but with the same layout") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    objs.push_back(Obj{ 2, "2222222222222222222222222" });

    mrf::vector<Obj>::reference obj_0 = objs[0];
    mrf::vector<Obj>::reference obj_1 = objs[1];

    {
        OtherObj o0 = obj_0.steal_into<OtherObj>();
        MRF_REQUIRE_EQ(o0.n, "1111111111111111111111111");
        MRF_REQUIRE(obj_0.name.empty());
    }
    {
        OtherObj o1 = std::move(obj_1).steal_into<OtherObj>();
        MRF_REQUIRE_EQ(o1.n, "2222222222222222222222222");
        MRF_REQUIRE(obj_1.name.empty());
    }
}

MRF_TEST_CASE_CTRT("mrf::vector forward reference into value of a different type but with the same layout") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    objs.push_back(Obj{ 2, "2222222222222222222222222" });

    mrf::vector<Obj>::reference obj_0 = objs[0];
    mrf::vector<Obj>::reference obj_1 = objs[1];

    {
        OtherObj o0 = obj_0.forward_into<OtherObj>();
        MRF_REQUIRE_EQ(obj_0.name, o0.n);
    }
    {
        OtherObj o1 = std::move(obj_1).forward_into<OtherObj>();
        MRF_REQUIRE_EQ(o1.n, "2222222222222222222222222");
        MRF_REQUIRE(obj_1.name.empty());
    }
}


MRF_TEST_CASE_CTRT("mrf::vector reference steal from another reference") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    objs.push_back(Obj{ 2, "2222222222222222222222222" });

    objs[0].steal_from(objs[1]);

    MRF_REQUIRE(objs[1].name.empty());
    MRF_REQUIRE_EQ(objs[0].name, "2222222222222222222222222");
}

MRF_TEST_CASE_CTRT("mrf::vector reference copy assign from another reference") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    objs.push_back(Obj{ 2, "2222222222222222222222222" });

    objs[0].from(objs[1]);
    MRF_REQUIRE_EQ(objs[0], objs[1]);

    objs[0].from(std::move(objs[1]));
    MRF_REQUIRE_EQ(objs[0], objs[1]);
}

MRF_TEST_CASE_CTRT("mrf::vector reference steal from another reference") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    objs.push_back(Obj{ 2, "2222222222222222222222222" });

    objs[0].steal_from(objs[1]);

    MRF_REQUIRE(objs[1].name.empty());
    MRF_REQUIRE_EQ(objs[0].name, "2222222222222222222222222");
}

MRF_TEST_CASE_CTRT("mrf::vector non-const reference is implicitly convertible to const reference") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    objs.push_back(Obj{ 2, "2222222222222222222222222" });

    mrf::vector<Obj>::reference nonconst_ref = objs[0];
    mrf::vector<Obj>::const_reference const_ref = nonconst_ref;
}

MRF_TEST_CASE_CTRT("mrf::vector non-const bucket reference is implicitly convertible to const bucket reference") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    objs.push_back(Obj{ 2, "2222222222222222222222222" });

    std::vector<mrf::bucket<Obj, ^^Obj::name>>& name_buckets = objs.bucket<^^Obj::name>();

    using bucket_ref = mrf::bucket_reference<Obj, ^^Obj::name>;
    // bucket_ref b0 = name_buckets[0].into<bucket_ref>();
}

} // namespace mrf::test::mixin