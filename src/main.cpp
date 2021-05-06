#include <iostream>
#include <numeric>
#include <algorithm>
#include <dataframe.hpp>

int main(){
    amt::basic_series< amt::dynamic_unbounded_storage > s(10ul,100.f);
    amt::basic_series< amt::dynamic_unbounded_storage > t(10ul, std::string_view("StringTest"));
    
    for(auto const& el : amt::iter<float>(s)){
        std::cout<<el<<' ';
    }
    std::cout<<"\n";
    
    for(auto const& el : amt::iter<std::string_view>(t)){
        std::cout<<el<<' ';
    }
    std::cout<<"\n";

    std::cout<<amt::is_any<float,int>(s)<<'\n';

    return 0;
}

