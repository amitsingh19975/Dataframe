#include <iostream>
#include <numeric>
#include <algorithm>
#include <dataframe.hpp>

int main(){
    amt::basic_series< amt::dynamic_unbounded_storage > s(10ul,100.f);
    amt::basic_series< amt::dynamic_unbounded_storage > t;
    std::cout<<(s==t)<<'\n';
    return 0;
}

