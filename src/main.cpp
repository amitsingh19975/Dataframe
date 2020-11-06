#include <boost/core/demangle.hpp>
#include <iostream>
#include <series.hpp>
#include <functions.hpp>
#include <numeric>
#include <algorithm>
#include <traits/pipeline_traits.hpp>
#include <frame.hpp>

std::string get_name(auto&& t){
    return boost::core::demangle(typeid(t).name());
}

void print(std::unordered_set<amt::box> const& s){
    for(auto const& el : s){
        std::cout<<el<<'\n';
    }
}

void print(std::unordered_map<amt::box,std::size_t> const& s){
    for(auto const& [k,v] : s){
        std::cout<<"Key: "<<k<<", Value: "<<v<<'\n';
    }
}

int main(){
    amt::frame f = {
        {"Test1",{1.,2.,3.345,4.}, amt::dtype<double>{}},
        {"Test2",{"1","23333333333333333333333333333333333333333333333333333","3","4"}, amt::dtype<std::string>{}},
        {"Test3",{"1","2","3","4"}, amt::dtype<std::string>{}}
    };

    // std::cout<<amt::list_filter(f,amt::name_list{"Test1", "Test3"})<<'\n';

    // amt::series temp = {"names", {"A", "A", "B", "A", "C", "C"}, amt::dtype<std::string>()};
    std::cout<< amt::describe(f)<<'\n';

    return 0;
}