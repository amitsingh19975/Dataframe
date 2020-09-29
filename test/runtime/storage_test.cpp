#include <catch2/catch.hpp>
#include <core/storage.hpp>

TEST_CASE("Storage Construction", "[Storage]") {
    amt::core::storage<> s1;
    REQUIRE(s1.empty());
    REQUIRE(s1.index() == amt::core::storage<>::npos);
    REQUIRE_THROWS_AS(s1.as<int>(), std::bad_cast);

    amt::core::storage<> s2(10);
    REQUIRE(s2.empty() == false);
    REQUIRE(s2.index() != amt::core::storage<>::npos);
    REQUIRE(s2.as<int>() == 10);
    REQUIRE_THROWS_AS(s2.as<std::string>(), std::bad_cast);

    amt::core::storage<> s3("String");
    REQUIRE(s3.empty() == false);
    REQUIRE(s3.index() != amt::core::storage<>::npos);
    REQUIRE(s3.as<std::string>() == "String");
    REQUIRE_THROWS_AS(s3.as<int>(), std::bad_cast);

    amt::core::storage<> s4(false);
    REQUIRE(s4.empty() == false);
    REQUIRE(s4.index() != amt::core::storage<>::npos);
    REQUIRE(s4.as<bool>() == false);
    REQUIRE_THROWS_AS(s4.as<int>(), std::bad_cast);
}

TEST_CASE("Storage Copy Construction", "[Storage]") {
    {
        amt::core::storage<> s2(10);
        amt::core::storage<> s1(s2);
        REQUIRE(s1.empty() == false);
        REQUIRE(s1.index() != amt::core::storage<>::npos);
        REQUIRE(s1.as<int>() == 10);
        REQUIRE_THROWS_AS(s1.as<std::string>(), std::bad_cast);
    }
    {
        amt::core::storage<> s3("String");
        amt::core::storage<> s1(s3);
        REQUIRE(s1.empty() == false);
        REQUIRE(s1.index() != amt::core::storage<>::npos);
        REQUIRE(s1.as<std::string>() == "String");
        REQUIRE_THROWS_AS(s1.as<int>(), std::bad_cast);
    }
    {
        amt::core::storage<> s4(false);
        amt::core::storage<> s1(s4);
        REQUIRE(s1.empty() == false);
        REQUIRE(s1.index() != amt::core::storage<>::npos);
        REQUIRE(s1.as<bool>() == false);
        REQUIRE_THROWS_AS(s1.as<int>(), std::bad_cast);
    }
}

TEST_CASE("Storage Copy Assignment", "[Storage]") {
    amt::core::storage<> s1;
    REQUIRE(s1.empty());
    REQUIRE(s1.index() == amt::core::storage<>::npos);
    REQUIRE_THROWS_AS(s1.as<int>(), std::bad_cast);

    amt::core::storage<> s2(10);
    s1 = s2;
    REQUIRE(s1.empty() == false);
    REQUIRE(s1.index() != amt::core::storage<>::npos);
    REQUIRE(s1.as<int>() == 10);
    REQUIRE_THROWS_AS(s1.as<std::string>(), std::bad_cast);

    amt::core::storage<> s3("String");
    s1 = s3;
    REQUIRE(s1.empty() == false);
    REQUIRE(s1.index() != amt::core::storage<>::npos);
    REQUIRE(s1.as<std::string>() == "String");
    REQUIRE_THROWS_AS(s1.as<int>(), std::bad_cast);

    amt::core::storage<> s4(false);
    s1 = s4;
    REQUIRE(s1.empty() == false);
    REQUIRE(s1.index() != amt::core::storage<>::npos);
    REQUIRE(s1.as<bool>() == false);
    REQUIRE_THROWS_AS(s1.as<int>(), std::bad_cast);
}

TEST_CASE("Storage Move Construction", "[Storage]") {
    {
        amt::core::storage<> s2(10);
        amt::core::storage<> s1( std::move(s2) );
        REQUIRE(s1.empty() == false);
        REQUIRE(s1.index() != amt::core::storage<>::npos);
        REQUIRE(s1.as<int>() == 10);
        REQUIRE_THROWS_AS(s1.as<std::string>(), std::bad_cast);
    }
    {
        amt::core::storage<> s3("String");
        amt::core::storage<> s1( std::move(s3) );
        REQUIRE(s1.empty() == false);
        REQUIRE(s1.index() != amt::core::storage<>::npos);
        REQUIRE(s1.as<std::string>() == "String");
        REQUIRE_THROWS_AS(s1.as<int>(), std::bad_cast);
    }
    {
        amt::core::storage<> s4(false);
        amt::core::storage<> s1( std::move(s4) );
        REQUIRE(s1.empty() == false);
        REQUIRE(s1.index() != amt::core::storage<>::npos);
        REQUIRE(s1.as<bool>() == false);
        REQUIRE_THROWS_AS(s1.as<int>(), std::bad_cast);
    }
}

TEST_CASE("Storage Move Assignment", "[Storage]") {
    amt::core::storage<> s1;
    REQUIRE(s1.empty());
    REQUIRE(s1.index() == amt::core::storage<>::npos);
    REQUIRE_THROWS_AS(s1.as<int>(), std::bad_cast);

    amt::core::storage<> s2(10);
    s1 = std::move(s2);
    REQUIRE(s1.empty() == false);
    REQUIRE(s1.index() != amt::core::storage<>::npos);
    REQUIRE(s1.as<int>() == 10);
    REQUIRE_THROWS_AS(s1.as<std::string>(), std::bad_cast);

    amt::core::storage<> s3("String");
    s1 = std::move(s3);
    REQUIRE(s1.empty() == false);
    REQUIRE(s1.index() != amt::core::storage<>::npos);
    REQUIRE(s1.as<std::string>() == "String");
    REQUIRE_THROWS_AS(s1.as<int>(), std::bad_cast);

    amt::core::storage<> s4(false);
    s1 = std::move(s4);
    REQUIRE(s1.empty() == false);
    REQUIRE(s1.index() != amt::core::storage<>::npos);
    REQUIRE(s1.as<bool>() == false);
    REQUIRE_THROWS_AS(s1.as<int>(), std::bad_cast);
}


TEST_CASE("Storage Construction using Value", "[Storage]") {
    {
        amt::core::storage<> s1(10);
        REQUIRE(s1.empty() == false);
        REQUIRE(s1.index() != amt::core::storage<>::npos);
        REQUIRE(s1.as<int>() == 10);
        REQUIRE_THROWS_AS(s1.as<std::string>(), std::bad_cast);
    }
    {
        amt::core::storage<> s1("String");
        REQUIRE(s1.empty() == false);
        REQUIRE(s1.index() != amt::core::storage<>::npos);
        REQUIRE(s1.as<std::string>() == "String");
        REQUIRE_THROWS_AS(s1.as<int>(), std::bad_cast);
    }
    {
        amt::core::storage<> s1(false);
        REQUIRE(s1.empty() == false);
        REQUIRE(s1.index() != amt::core::storage<>::npos);
        REQUIRE(s1.as<bool>() == false);
        REQUIRE_THROWS_AS(s1.as<int>(), std::bad_cast);
    }
}

struct custom_type {
    float v1{};
    int v2{};
};

TEST_CASE("Storage Construction using Custom Type", "[Storage]") {
    amt::core::storage<custom_type> s1(custom_type{1.f, 100});
    REQUIRE(s1.empty() == false);
    REQUIRE(s1.type_index<custom_type>() == s1.index());
    REQUIRE(s1.index() != amt::core::storage<custom_type>::npos);
    REQUIRE(s1.as<custom_type>().v1 == 1.f);
    REQUIRE(s1.as<custom_type>().v2 == 100);
    REQUIRE_THROWS_AS(s1.as<std::string>(), std::bad_cast);
}

TEST_CASE("Storage Visitor", "[Visitor]") {
    amt::core::storage<custom_type> s1(custom_type{1.f, 100});
    REQUIRE(s1.empty() == false);
    REQUIRE(s1.type_index<custom_type>() == s1.index());
    REQUIRE(s1.index() != amt::core::storage<custom_type>::npos);
    REQUIRE(s1.as<custom_type>().v1 == 1.f);
    REQUIRE(s1.as<custom_type>().v2 == 100);
    REQUIRE_THROWS_AS(s1.as<std::string>(), std::bad_cast);

    amt::core::visit(s1, [](custom_type const& val){
        REQUIRE(val.v1 == 1.f);
        REQUIRE(val.v2 == 100);
    });

    amt::core::visit(s1, [](auto&& val){
        using type = std::decay_t< decltype(val) >;
        REQUIRE(amt::core::type_to_string<type>() == "custom_type");
        if constexpr ( std::is_same_v< custom_type, type> ){
            REQUIRE(val.v1 == 1.f);
            REQUIRE(val.v2 == 100);
        }
    });

}