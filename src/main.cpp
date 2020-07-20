#include <algorithm>
#include <frame.hpp>
#include <functional>
#include <functions.hpp>
#include <iostream>
#include <numeric>
#include <dataframe.hpp>
#include <parser/csv.hpp>

int main(int, char **) {

    auto temp = amt::read_csv("/Users/amit/Desktop/code/dataframe/src/test.csv", true);
    std::cout<<temp<<'\n';
    std::cout<<amt::to<int>(temp,amt::in_place)<<'\n';
    return 0;
}