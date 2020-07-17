#include <catch2/catch.hpp>

constexpr unsigned int factorial(unsigned int n){
    return n <= 1 ? n : factorial( n - 1 ) * n;
}

TEST_CASE("Factorial are computed with constexpr", "[factorial]"){
    STATIC_REQUIRE(factorial(1) == 1);
    STATIC_REQUIRE(factorial(2) == 2);
    STATIC_REQUIRE(factorial(3) == 6);
    STATIC_REQUIRE(factorial(10) == 3628800);
}