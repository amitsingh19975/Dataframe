#include <iostream>
#include <numeric>
#include <algorithm>
#include <dataframe.hpp>

int main(){
    amt::basic_series< amt::dynamic_unbounded_storage > s(10ul,100.f);
    for(auto el : amt::iter<float>(s)){
        std::cout<<el<<'\n';
    }
    return 0;
}

