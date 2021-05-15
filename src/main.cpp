#include <algorithm>
#include <iostream>
#include <numeric>
#include <dataframe.hpp>

int main() {
    amt::basic_series< amt::dynamic_unbounded_storage > s(10ul,4.f);
    
    // for ( auto const& el : amt::iter< float >( s ) ) {
    //     std::cout << el << ' ';
    // }
    // std::cout << "\n" << amt::is<float>(s)<<'\n';

    // amt::push_back(s,"Hello");
    // amt::insert(s, 0, {"Word", "Ret", "try"});
    
    amt::transform<float,std::string>(s,[](float l){
        return std::powf(l,3.f);
    });

    std::cout<<amt::reduce(s, 0.f, std::plus<float>{})<<std::endl;

    auto f = amt::for_each<float,std::string>([](auto&& el){ std::cout<<el<<", "; });
    f(s);
    std::cout << "\n";

    return 0;
}
