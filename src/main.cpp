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

int main(){
    amt::frame f = {
        {"Test1",{1,2,3,4}, amt::dtype<double>{}},
        {"Test2",{"1","2","3","4"}, amt::dtype<std::string>{}},
        {"Test3",{"1","2","3","4"}, amt::dtype<std::string>{}}
    };
    amt::frame tf = {
        {"Test4",{1,2,3,4,5}, amt::dtype<double>{}},
        {"Test5",{"1","2","3","4","5"}, amt::dtype<std::string>{}},
    };
    std::cout<<f<<'\n';
    std::cout<<amt::concat_col(f[0], tf)<<'\n';
    return 0;
}