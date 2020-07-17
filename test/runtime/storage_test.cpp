#include <catch2/catch.hpp>
#include <core/storage.hpp>

TEST_CASE("Storage Construction", "[Construction]") {
    amt::storage<> s1;
    REQUIRE(s1.empty());
    REQUIRE(s1.is_string() == false);
    REQUIRE(s1.is_bool() == false);
    REQUIRE(s1.is_integer() == false);
    REQUIRE(s1.is_char() == false);
    REQUIRE(s1.is_float() == false);
    REQUIRE(s1.is_double() == false);
    REQUIRE(s1.index() == amt::storage<>::npos);
    REQUIRE_THROWS_AS(s1.cast<int>(), std::bad_cast);

    amt::storage<> s2(10);
    REQUIRE(s2.empty() == false);
    REQUIRE(s2.is_string() == false);
    REQUIRE(s2.is_bool() == false);
    REQUIRE(s2.is_integer() == true);
    REQUIRE(s2.is_char() == false);
    REQUIRE(s2.is_float() == false);
    REQUIRE(s2.is_double() == false);
    REQUIRE(s2.index() != amt::storage<>::npos);
    REQUIRE(s2.cast<int>() == 10);
    REQUIRE_THROWS_AS(s2.cast<std::string>(), std::bad_cast);

    amt::storage<> s3("String");
    REQUIRE(s3.empty() == false);
    REQUIRE(s3.is_string() == true);
    REQUIRE(s3.is_bool() == false);
    REQUIRE(s3.is_integer() == false);
    REQUIRE(s3.is_char() == false);
    REQUIRE(s3.is_float() == false);
    REQUIRE(s3.is_double() == false);
    REQUIRE(s3.index() != amt::storage<>::npos);
    REQUIRE(s3.cast<std::string>() == "String");
    REQUIRE_THROWS_AS(s3.cast<int>(), std::bad_cast);

    amt::storage<> s4(false);
    REQUIRE(s4.empty() == false);
    REQUIRE(s4.is_string() == false);
    REQUIRE(s4.is_bool() == true);
    REQUIRE(s4.is_integer() == false);
    REQUIRE(s4.is_char() == false);
    REQUIRE(s4.is_float() == false);
    REQUIRE(s4.is_double() == false);
    REQUIRE(s4.index() != amt::storage<>::npos);
    REQUIRE(s4.cast<bool>() == false);
    REQUIRE_THROWS_AS(s4.cast<int>(), std::bad_cast);
}

TEST_CASE("Storage Copy Construction", "[Copy Construction]") {
    {
        amt::storage<> s2(10);
        amt::storage<> s1(s2);
        REQUIRE(s1.empty() == false);
        REQUIRE(s1.is_string() == false);
        REQUIRE(s1.is_bool() == false);
        REQUIRE(s1.is_integer() == true);
        REQUIRE(s1.is_char() == false);
        REQUIRE(s1.is_float() == false);
        REQUIRE(s1.is_double() == false);
        REQUIRE(s1.index() != amt::storage<>::npos);
        REQUIRE(s1.cast<int>() == 10);
        REQUIRE_THROWS_AS(s1.cast<std::string>(), std::bad_cast);
    }
    {
        amt::storage<> s3("String");
        amt::storage<> s1(s3);
        REQUIRE(s1.empty() == false);
        REQUIRE(s1.is_string() == true);
        REQUIRE(s1.is_bool() == false);
        REQUIRE(s1.is_integer() == false);
        REQUIRE(s1.is_char() == false);
        REQUIRE(s1.is_float() == false);
        REQUIRE(s1.is_double() == false);
        REQUIRE(s1.index() != amt::storage<>::npos);
        REQUIRE(s1.cast<std::string>() == "String");
        REQUIRE_THROWS_AS(s1.cast<int>(), std::bad_cast);
    }
    {
        amt::storage<> s4(false);
        amt::storage<> s1(s4);
        REQUIRE(s1.empty() == false);
        REQUIRE(s1.is_string() == false);
        REQUIRE(s1.is_bool() == true);
        REQUIRE(s1.is_integer() == false);
        REQUIRE(s1.is_char() == false);
        REQUIRE(s1.is_float() == false);
        REQUIRE(s1.is_double() == false);
        REQUIRE(s1.index() != amt::storage<>::npos);
        REQUIRE(s1.cast<bool>() == false);
        REQUIRE_THROWS_AS(s1.cast<int>(), std::bad_cast);
    }
}

TEST_CASE("Storage Copy Assignment", "[Copy Assignment]") {
    amt::storage<> s1;
    REQUIRE(s1.empty());
    REQUIRE(s1.is_string() == false);
    REQUIRE(s1.is_bool() == false);
    REQUIRE(s1.is_integer() == false);
    REQUIRE(s1.is_char() == false);
    REQUIRE(s1.is_float() == false);
    REQUIRE(s1.is_double() == false);
    REQUIRE(s1.index() == amt::storage<>::npos);
    REQUIRE_THROWS_AS(s1.cast<int>(), std::bad_cast);

    amt::storage<> s2(10);
    s1 = s2;
    REQUIRE(s1.empty() == false);
    REQUIRE(s1.is_string() == false);
    REQUIRE(s1.is_bool() == false);
    REQUIRE(s1.is_integer() == true);
    REQUIRE(s1.is_char() == false);
    REQUIRE(s1.is_float() == false);
    REQUIRE(s1.is_double() == false);
    REQUIRE(s1.index() != amt::storage<>::npos);
    REQUIRE(s1.cast<int>() == 10);
    REQUIRE_THROWS_AS(s1.cast<std::string>(), std::bad_cast);

    amt::storage<> s3("String");
    s1 = s3;
    REQUIRE(s1.empty() == false);
    REQUIRE(s1.is_string() == true);
    REQUIRE(s1.is_bool() == false);
    REQUIRE(s1.is_integer() == false);
    REQUIRE(s1.is_char() == false);
    REQUIRE(s1.is_float() == false);
    REQUIRE(s1.is_double() == false);
    REQUIRE(s1.index() != amt::storage<>::npos);
    REQUIRE(s1.cast<std::string>() == "String");
    REQUIRE_THROWS_AS(s1.cast<int>(), std::bad_cast);

    amt::storage<> s4(false);
    s1 = s4;
    REQUIRE(s1.empty() == false);
    REQUIRE(s1.is_string() == false);
    REQUIRE(s1.is_bool() == true);
    REQUIRE(s1.is_integer() == false);
    REQUIRE(s1.is_char() == false);
    REQUIRE(s1.is_float() == false);
    REQUIRE(s1.is_double() == false);
    REQUIRE(s1.index() != amt::storage<>::npos);
    REQUIRE(s1.cast<bool>() == false);
    REQUIRE_THROWS_AS(s1.cast<int>(), std::bad_cast);
}

TEST_CASE("Storage Move Construction", "[Move Construction]") {
    {
        amt::storage<> s2(10);
        amt::storage<> s1( std::move(s2) );
        REQUIRE(s1.empty() == false);
        REQUIRE(s1.is_string() == false);
        REQUIRE(s1.is_bool() == false);
        REQUIRE(s1.is_integer() == true);
        REQUIRE(s1.is_char() == false);
        REQUIRE(s1.is_float() == false);
        REQUIRE(s1.is_double() == false);
        REQUIRE(s1.index() != amt::storage<>::npos);
        REQUIRE(s1.cast<int>() == 10);
        REQUIRE_THROWS_AS(s1.cast<std::string>(), std::bad_cast);
    }
    {
        amt::storage<> s3("String");
        amt::storage<> s1( std::move(s3) );
        REQUIRE(s1.empty() == false);
        REQUIRE(s1.is_string() == true);
        REQUIRE(s1.is_bool() == false);
        REQUIRE(s1.is_integer() == false);
        REQUIRE(s1.is_char() == false);
        REQUIRE(s1.is_float() == false);
        REQUIRE(s1.is_double() == false);
        REQUIRE(s1.index() != amt::storage<>::npos);
        REQUIRE(s1.cast<std::string>() == "String");
        REQUIRE_THROWS_AS(s1.cast<int>(), std::bad_cast);
    }
    {
        amt::storage<> s4(false);
        amt::storage<> s1( std::move(s4) );
        REQUIRE(s1.empty() == false);
        REQUIRE(s1.is_string() == false);
        REQUIRE(s1.is_bool() == true);
        REQUIRE(s1.is_integer() == false);
        REQUIRE(s1.is_char() == false);
        REQUIRE(s1.is_float() == false);
        REQUIRE(s1.is_double() == false);
        REQUIRE(s1.index() != amt::storage<>::npos);
        REQUIRE(s1.cast<bool>() == false);
        REQUIRE_THROWS_AS(s1.cast<int>(), std::bad_cast);
    }
}

TEST_CASE("Storage Move Assignment", "[Move Assignment]") {
    amt::storage<> s1;
    REQUIRE(s1.empty());
    REQUIRE(s1.is_string() == false);
    REQUIRE(s1.is_bool() == false);
    REQUIRE(s1.is_integer() == false);
    REQUIRE(s1.is_char() == false);
    REQUIRE(s1.is_float() == false);
    REQUIRE(s1.is_double() == false);
    REQUIRE(s1.index() == amt::storage<>::npos);
    REQUIRE_THROWS_AS(s1.cast<int>(), std::bad_cast);

    amt::storage<> s2(10);
    s1 = std::move(s2);
    REQUIRE(s1.empty() == false);
    REQUIRE(s1.is_string() == false);
    REQUIRE(s1.is_bool() == false);
    REQUIRE(s1.is_integer() == true);
    REQUIRE(s1.is_char() == false);
    REQUIRE(s1.is_float() == false);
    REQUIRE(s1.is_double() == false);
    REQUIRE(s1.index() != amt::storage<>::npos);
    REQUIRE(s1.cast<int>() == 10);
    REQUIRE_THROWS_AS(s1.cast<std::string>(), std::bad_cast);

    amt::storage<> s3("String");
    s1 = std::move(s3);
    REQUIRE(s1.empty() == false);
    REQUIRE(s1.is_string() == true);
    REQUIRE(s1.is_bool() == false);
    REQUIRE(s1.is_integer() == false);
    REQUIRE(s1.is_char() == false);
    REQUIRE(s1.is_float() == false);
    REQUIRE(s1.is_double() == false);
    REQUIRE(s1.index() != amt::storage<>::npos);
    REQUIRE(s1.cast<std::string>() == "String");
    REQUIRE_THROWS_AS(s1.cast<int>(), std::bad_cast);

    amt::storage<> s4(false);
    s1 = std::move(s4);
    REQUIRE(s1.empty() == false);
    REQUIRE(s1.is_string() == false);
    REQUIRE(s1.is_bool() == true);
    REQUIRE(s1.is_integer() == false);
    REQUIRE(s1.is_char() == false);
    REQUIRE(s1.is_float() == false);
    REQUIRE(s1.is_double() == false);
    REQUIRE(s1.index() != amt::storage<>::npos);
    REQUIRE(s1.cast<bool>() == false);
    REQUIRE_THROWS_AS(s1.cast<int>(), std::bad_cast);
}


TEST_CASE("Storage Construction By Value", "[Value Construction]") {
    {
        amt::storage<> s1(10);
        REQUIRE(s1.empty() == false);
        REQUIRE(s1.is_string() == false);
        REQUIRE(s1.is_bool() == false);
        REQUIRE(s1.is_integer() == true);
        REQUIRE(s1.is_char() == false);
        REQUIRE(s1.is_float() == false);
        REQUIRE(s1.is_double() == false);
        REQUIRE(s1.index() != amt::storage<>::npos);
        REQUIRE(s1.cast<int>() == 10);
        REQUIRE_THROWS_AS(s1.cast<std::string>(), std::bad_cast);
    }
    {
        amt::storage<> s1("String");
        REQUIRE(s1.empty() == false);
        REQUIRE(s1.is_string() == true);
        REQUIRE(s1.is_bool() == false);
        REQUIRE(s1.is_integer() == false);
        REQUIRE(s1.is_char() == false);
        REQUIRE(s1.is_float() == false);
        REQUIRE(s1.is_double() == false);
        REQUIRE(s1.index() != amt::storage<>::npos);
        REQUIRE(s1.cast<std::string>() == "String");
        REQUIRE_THROWS_AS(s1.cast<int>(), std::bad_cast);
    }
    {
        amt::storage<> s1(false);
        REQUIRE(s1.empty() == false);
        REQUIRE(s1.is_string() == false);
        REQUIRE(s1.is_bool() == true);
        REQUIRE(s1.is_integer() == false);
        REQUIRE(s1.is_char() == false);
        REQUIRE(s1.is_float() == false);
        REQUIRE(s1.is_double() == false);
        REQUIRE(s1.index() != amt::storage<>::npos);
        REQUIRE(s1.cast<bool>() == false);
        REQUIRE_THROWS_AS(s1.cast<int>(), std::bad_cast);
    }
}

struct custom_type {
    float v1{};
    int v2{};
};

TEST_CASE("Storage Construction By Custom Type", "[Custom Type Construction]") {
    amt::storage<custom_type> s1(custom_type{1.f, 100});
    REQUIRE(s1.empty() == false);
    REQUIRE(s1.is_string() == false);
    REQUIRE(s1.is_bool() == false);
    REQUIRE(s1.is_integer() == false);
    REQUIRE(s1.is_char() == false);
    REQUIRE(s1.is_float() == false);
    REQUIRE(s1.is_double() == false);
    REQUIRE(s1.is_type<custom_type>() == true);
    REQUIRE(s1.type_index<custom_type>() == s1.index());
    REQUIRE(s1.index() != amt::storage<>::npos);
    REQUIRE(s1.cast<custom_type>().v1 == 1.f);
    REQUIRE(s1.cast<custom_type>().v2 == 100);
    REQUIRE_THROWS_AS(s1.cast<std::string>(), std::bad_cast);
}

TEST_CASE("Storage Visitor", "[Visitor]") {
    amt::storage<custom_type> s1(custom_type{1.f, 100});
    REQUIRE(s1.empty() == false);
    REQUIRE(s1.is_string() == false);
    REQUIRE(s1.is_bool() == false);
    REQUIRE(s1.is_integer() == false);
    REQUIRE(s1.is_char() == false);
    REQUIRE(s1.is_float() == false);
    REQUIRE(s1.is_double() == false);
    REQUIRE(s1.is_type<custom_type>() == true);
    REQUIRE(s1.type_index<custom_type>() == s1.index());
    REQUIRE(s1.index() != amt::storage<>::npos);
    REQUIRE(s1.cast<custom_type>().v1 == 1.f);
    REQUIRE(s1.cast<custom_type>().v2 == 100);
    REQUIRE_THROWS_AS(s1.cast<std::string>(), std::bad_cast);

    amt::visit(s1, [](custom_type const& val){
        REQUIRE(val.v1 == 1.f);
        REQUIRE(val.v2 == 100);
    });

    amt::visit(s1, [](auto&& val){
        using type = std::decay_t< decltype(val) >;
        REQUIRE(amt::core::type_to_string<type>() == "custom_type");
        if constexpr ( std::is_same_v< custom_type, type> ){
            REQUIRE(val.v1 == 1.f);
            REQUIRE(val.v2 == 100);
        }
    });

}