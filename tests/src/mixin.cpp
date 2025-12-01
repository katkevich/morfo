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

MRF_TEST_CASE_CTRT("copy mrf::vector::reference into original value") {
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

MRF_TEST_CASE_CTRT("steal mrf::vector::reference into original value") {
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

MRF_TEST_CASE_CTRT("forward mrf::vector::reference into original value") {
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

MRF_TEST_CASE_CTRT("copy mrf::vector::reference into tuple") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    objs.push_back(Obj{ 2, "2222222222222222222222222" });

    mrf::vector<Obj>::reference obj_0 = objs[0];
    mrf::vector<Obj>::reference obj_1 = objs[1];

    {
        auto t0 = obj_0.into_tuple();
        static_assert(std::is_same_v<decltype(t0), std::tuple<int, std::string>>);
        MRF_REQUIRE_EQ(obj_0.name, std::get<1>(t0));
    }
    {
        auto t0 = std::move(obj_0).into_tuple();
        static_assert(std::is_same_v<decltype(t0), std::tuple<int, std::string>>);
        MRF_REQUIRE_EQ(obj_0.name, std::get<1>(t0));
    }
}

MRF_TEST_CASE_CTRT("steal mrf::vector::reference into tuple") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    objs.push_back(Obj{ 2, "2222222222222222222222222" });

    mrf::vector<Obj>::reference obj_0 = objs[0];
    mrf::vector<Obj>::reference obj_1 = objs[1];

    {
        auto t0 = obj_0.steal_into_tuple();
        MRF_REQUIRE_EQ(std::get<1>(t0), "1111111111111111111111111");
        MRF_REQUIRE(obj_0.name.empty());
    }
    {
        auto t1 = std::move(obj_1).steal_into_tuple();
        MRF_REQUIRE_EQ(std::get<1>(t1), "2222222222222222222222222");
        MRF_REQUIRE(obj_1.name.empty());
    }
}

MRF_TEST_CASE_CTRT("forward mrf::vector::reference into tuple") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    objs.push_back(Obj{ 2, "2222222222222222222222222" });

    mrf::vector<Obj>::reference obj_0 = objs[0];
    mrf::vector<Obj>::reference obj_1 = objs[1];

    {
        auto t0 = obj_0.forward_into_tuple();
        MRF_REQUIRE_EQ(obj_0.name, std::get<1>(t0));
    }
    {
        auto t1 = std::move(obj_1).forward_into_tuple();
        MRF_REQUIRE_EQ(std::get<1>(t1), "2222222222222222222222222");
        MRF_REQUIRE(obj_1.name.empty());
    }
}

MRF_TEST_CASE_CTRT("copy mrf::vector::reference into value of a different type but with the same layout") {
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

MRF_TEST_CASE_CTRT("steal mrf::vector::reference into value of a different type but with the same layout") {
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

MRF_TEST_CASE_CTRT("forward mrf::vector::reference into value of a different type but with the same layout") {
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


MRF_TEST_CASE_CTRT("steal into mrf::vector::reference from another reference") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    objs.push_back(Obj{ 2, "2222222222222222222222222" });

    objs[0].steal_from(objs[1]);

    MRF_REQUIRE(objs[1].name.empty());
    MRF_REQUIRE_EQ(objs[0].name, "2222222222222222222222222");
}

MRF_TEST_CASE_CTRT("copy assign into mrf::vector::reference from another reference") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    objs.push_back(Obj{ 2, "2222222222222222222222222" });

    objs[0].from(objs[1]);
    MRF_REQUIRE_EQ(objs[0], objs[1]);

    objs[0].from(std::move(objs[1]));
    MRF_REQUIRE_EQ(objs[0], objs[1]);
}

MRF_TEST_CASE_CTRT("steal into mrf::vector::reference from original value") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    Obj obj_1{ 2, "2222222222222222222222222" };

    objs[0].steal_from(obj_1);

    MRF_REQUIRE(obj_1.name.empty());
    MRF_REQUIRE_EQ(objs[0].name, "2222222222222222222222222");
}

MRF_TEST_CASE_CTRT("copy assign into mrf::vector::reference from lvalue original value") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    Obj obj_1{ 2, "2222222222222222222222222" };

    objs[0].from(obj_1);
    MRF_REQUIRE_EQ(objs[0].name, obj_1.name);
}

MRF_TEST_CASE_CTRT("move assign into mrf::vector::reference from rvalue original value") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    Obj obj_1{ 2, "2222222222222222222222222" };

    objs[0].from(std::move(obj_1));

    MRF_REQUIRE(obj_1.name.empty());
    MRF_REQUIRE_EQ(objs[0].name, "2222222222222222222222222");
}

MRF_TEST_CASE_CTRT("steal into mrf::vector::reference from std::tuple") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    std::tuple obj_1{ 2, std::string("2222222222222222222222222") };

    objs[0].steal_from(obj_1);

    MRF_REQUIRE(std::get<1>(obj_1).empty());
    MRF_REQUIRE_EQ(objs[0].name, "2222222222222222222222222");
}

MRF_TEST_CASE_CTRT("copy assign into mrf::vector::reference from std::tuple") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    std::tuple obj_1{ 2, std::string("2222222222222222222222222") };

    objs[0].from(obj_1);
    MRF_REQUIRE_EQ(objs[0].name, std::get<1>(obj_1));
}

MRF_TEST_CASE_CTRT("move assign into mrf::vector::reference from std::tuple") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    std::tuple obj_1{ 2, std::string("2222222222222222222222222") };

    objs[0].from(std::move(obj_1));

    MRF_REQUIRE(std::get<1>(obj_1).empty());
    MRF_REQUIRE_EQ(objs[0].name, "2222222222222222222222222");
}

MRF_TEST_CASE_CTRT("steal into mrf::vector::reference from a tuple-like (implements get<size_t> of a tuple protocol)") {
    struct Foo {
        std::string s1;
        std::string s2;
    };
    mrf::vector<Foo> objs;
    objs.push_back(Foo{ "1111111111111111111111111", "111111111111111111111111*" });

    /* std::array implements tuple protocol */
    std::array tuple_like_obj{
        std::string("2222222222222222222222222"),
        std::string("222222222222222222222222*"),
    };

    objs[0].steal_from(tuple_like_obj);

    MRF_REQUIRE(tuple_like_obj[0].empty());
    MRF_REQUIRE(tuple_like_obj[1].empty());
    MRF_REQUIRE_EQ(objs[0].s1, "2222222222222222222222222");
    MRF_REQUIRE_EQ(objs[0].s2, "222222222222222222222222*");
}

MRF_TEST_CASE_CTRT(
    "copy assign into mrf::vector::reference from a tuple-like (implements get<size_t> of a tuple protocol)") {
    struct Foo {
        std::string s1;
        std::string s2;
    };
    mrf::vector<Foo> objs;
    objs.push_back(Foo{ "1111111111111111111111111", "111111111111111111111111*" });

    /* std::array implements tuple protocol */
    std::array tuple_like_obj{
        std::string("2222222222222222222222222"),
        std::string("222222222222222222222222*"),
    };

    objs[0].from(tuple_like_obj);
    MRF_REQUIRE_EQ(objs[0].s1, tuple_like_obj[0]);
    MRF_REQUIRE_EQ(objs[0].s2, tuple_like_obj[1]);
}

MRF_TEST_CASE_CTRT(
    "move assign into mrf::vector::reference from a tuple-like (implements get<size_t> of a tuple protocol)") {
    struct Foo {
        std::string s1;
        std::string s2;
    };
    mrf::vector<Foo> objs;
    objs.push_back(Foo{ "1111111111111111111111111", "111111111111111111111111*" });

    /* std::array implements tuple protocol */
    std::array tuple_like_obj{
        std::string("2222222222222222222222222"),
        std::string("222222222222222222222222*"),
    };

    objs[0].from(std::move(tuple_like_obj));

    MRF_REQUIRE(tuple_like_obj[0].empty());
    MRF_REQUIRE(tuple_like_obj[1].empty());
    MRF_REQUIRE_EQ(objs[0].s1, "2222222222222222222222222");
    MRF_REQUIRE_EQ(objs[0].s2, "222222222222222222222222*");
}

MRF_TEST_CASE_CTRT("mrf::vector non-const reference is implicitly convertible to const reference") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    objs.push_back(Obj{ 2, "2222222222222222222222222" });

    mrf::vector<Obj>::reference nonconst_ref = objs[0];
    mrf::vector<Obj>::const_reference const_ref = nonconst_ref;
}

MRF_TEST_CASE_CTRT("mrf::vector const reference can be created from non-const reference using 'into'") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    objs.push_back(Obj{ 2, "2222222222222222222222222" });

    mrf::vector<Obj>::reference nonconst_ref = objs[0];
    auto const_ref = nonconst_ref.into<mrf::vector<Obj>::const_reference>();

    static_assert(std::is_same_v<decltype(const_ref), mrf::vector<Obj>::const_reference>);
}

MRF_TEST_CASE_CTRT("mrf::vector non-const bucket reference is implicitly convertible to const bucket reference") {
    mrf::vector<Obj> objs;
    objs.push_back(Obj{ 1, "1111111111111111111111111" });
    objs.push_back(Obj{ 2, "2222222222222222222222222" });

    auto& buckets = objs.bucket<^^Obj::name>();

    auto ref = mrf::bucket_reference<Obj, ^^Obj::name>::make(buckets[0]);

    auto const_ref_0 = static_cast<mrf::bucket_const_reference<Obj, ^^Obj::name>>(ref);
    auto const_ref_1 = mrf::bucket_const_reference<Obj, ^^Obj::name>(ref);
    mrf::bucket_const_reference<Obj, ^^Obj::name> const_ref_2 = ref;
}

} // namespace mrf::test::mixin