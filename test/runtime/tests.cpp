#include <catch2/catch.hpp>

constexpr unsigned int factorial(unsigned int n){
    return n <= 1 ? n : factorial( n - 1 ) * n;
}

TEST_CASE("Factorial are computed", "[factorial]"){
    REQUIRE(factorial(1) == 1);
    REQUIRE(factorial(2) == 2);
    REQUIRE(factorial(3) == 6);
    REQUIRE(factorial(10) == 3628800);
}