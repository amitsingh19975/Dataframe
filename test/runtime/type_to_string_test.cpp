#include <catch2/catch.hpp>
#include <core/type_to_string.hpp>

struct custom_type{};

TEST_CASE("Types are converted to string", "[TypeToString]") {
    REQUIRE(amt::core::type_to_string<std::int8_t>() == "std::int8_t");
    REQUIRE(amt::core::type_to_string<std::int16_t>() == "std::int16_t");
    REQUIRE(amt::core::type_to_string<std::int32_t>() == "std::int32_t");
    REQUIRE(amt::core::type_to_string<std::int64_t>() == "std::int64_t");
    REQUIRE(amt::core::type_to_string<std::uint8_t>() == "std::uint8_t");
    REQUIRE(amt::core::type_to_string<std::uint16_t>() == "std::uint16_t");
    REQUIRE(amt::core::type_to_string<std::uint32_t>() == "std::uint32_t");
    REQUIRE(amt::core::type_to_string<std::uint64_t>() == "std::uint64_t");
    REQUIRE(amt::core::type_to_string<std::string>() == "std::string");
    REQUIRE(amt::core::type_to_string<float>() == "float");
    REQUIRE(amt::core::type_to_string<double>() == "double");
    REQUIRE(amt::core::type_to_string<custom_type>() == "custom_type");
    
    REQUIRE(amt::core::type_to_string(std::int8_t{}) == "std::int8_t");
    REQUIRE(amt::core::type_to_string(std::int16_t{}) == "std::int16_t");
    REQUIRE(amt::core::type_to_string(std::int32_t{}) == "std::int32_t");
    REQUIRE(amt::core::type_to_string(std::int64_t{}) == "std::int64_t");
    REQUIRE(amt::core::type_to_string(std::uint8_t{}) == "std::uint8_t");
    REQUIRE(amt::core::type_to_string(std::uint16_t{}) == "std::uint16_t");
    REQUIRE(amt::core::type_to_string(std::uint32_t{}) == "std::uint32_t");
    REQUIRE(amt::core::type_to_string(std::uint64_t{}) == "std::uint64_t");
    REQUIRE(amt::core::type_to_string(std::string{}) == "std::string");
    REQUIRE(amt::core::type_to_string(float{}) == "float");
    REQUIRE(amt::core::type_to_string(double{}) == "double");
    REQUIRE(amt::core::type_to_string(custom_type{}) == "custom_type");
}