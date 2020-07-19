#include <algorithm>
#include <frame.hpp>
#include <functional>
#include <functions.hpp>
#include <iostream>
#include <numeric>
#include <dataframe.hpp>

struct test {};int main(int, char **) {

    // amt::frame<> s = {{"A", {"1", "2", "3", "4", "5"}},
    //                   {"B", {11, 12, 13, 14, 15}},
    //                   {"C", {21, 22, 23, 24, 25}}};
    // std::cout << s << '\n';
    // auto temp = s;
    // std::cout << amt::to<float>(temp, amt::in_place) << '\n';
    // std::cout << temp << '\n';

    amt::series<> ser(10000,10);
    auto temp = ser;
    temp[566] = 20;

    std::cout<<std::boolalpha<<(temp == ser)<<'\n';

    return 0;
}