#undef NDEBUG
#include <algorithm>
#include <frame.hpp>
#include <functional>
#include <functions.hpp>
#include <iostream>
#include <numeric>
#include <operator.hpp>
#include <series.hpp>
#include <vector>

struct test {};

int main(int, char **) {

    amt::frame<> s = {{"A", {"1", "2", "3", "4", "5"}},
                      {"B", {11, 12, 13, 14, 15}},
                      {"C", {21, 22, 23, 24, 25}}};
    std::cout << s << '\n';
    auto temp = s;
    std::cout << amt::to<float>(temp, amt::in_place) << '\n';
    std::cout << temp << '\n';

    return 0;
}