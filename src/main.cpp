#include <iostream>
#include <numeric>
#include <algorithm>
#include <dataframe.hpp>
#include <dataframe/parser/csv.hpp>

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
    std::string_view filename = "/Users/amit/Downloads/house-prices-advanced-regression-techniques/test.csv";
    auto data = amt::read_csv(filename, true);
    amt::replace(data["Alley"], amt::tag::inplace, { {"NA", "NAA"} });

    amt::infer<>(data,amt::tag::inplace);
    // // amt::imputer.mean(data["LotFrontage"],amt::tag::inplace, amt::tag::nan);
    // // amt::drop_nan(data,amt::tag::inplace);
    
    amt::pretty_string << std::setprecision(5) << std::fixed;
    amt::frame h = amt::head(data);
    std::cout<<amt::pretty_string(h, 4u, true, 3u)<<'\n';
    std::cout<<amt::describe(h)<<'\n';

    // amt::frame f = {
    //     {"Test1",{1.,2.,3.345,4.}, amt::dtype<double>{}},
    //     {"Test2",{"1","23333333333333333333333333333333333333333333333333333","3","4"}, amt::dtype<std::string>{}},
    //     {"Test3",{"1","2","3","4"}, amt::dtype<std::string>{}}
    // };

    // amt::series t = {"Test1",{44., 27., 30., 38., 40., 35., std::nan("1"), 48., 50., 37.}, amt::dtype<double>{}};
    return 0;
}