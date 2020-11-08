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
    amt::transform(data["Alley"], amt::tag::inplace, [](auto&& val){
        if(get<std::string>(val) == "NA"){
            return std::string("NAA");
        }
        return get<std::string>(val);
    });

    amt::infer<true>(data,amt::tag::inplace);
    // amt::imputer.mean(data["LotFrontage"],amt::tag::inplace, amt::tag::nan);
    // amt::drop_nan(data,amt::tag::inplace);
    amt::pretty_string << std::setprecision(5) << std::fixed;
    std::cout<<amt::pretty_string(data)<<'\n';

    // amt::frame f = {
    //     {"Test1",{1.,2.,3.345,4.}, amt::dtype<double>{}},
    //     {"Test2",{"1","23333333333333333333333333333333333333333333333333333","3","4"}, amt::dtype<std::string>{}},
    //     {"Test3",{"1","2","3","4"}, amt::dtype<std::string>{}}
    // };

    // amt::series t = {"Test1",{44., 27., 30., 38., 40., 35., std::nan("1"), 48., 50., 37.}, amt::dtype<double>{}};
    // amt::series t = {"Test1",{"A", "B", "NaN", "nan", ""}};
    // std::cout<<amt::cast<char>(t)<<'\n';
    return 0;
}