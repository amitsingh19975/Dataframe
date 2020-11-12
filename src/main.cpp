#include <iostream>
#include <numeric>
#include <algorithm>
#include <dataframe.hpp>
#include <dataframe/core/argument_list.hpp>

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
    // std::string_view filename = "/Users/amit/Downloads/house-prices-advanced-regression-techniques/test.csv";
    // auto data = amt::read_csv(filename, true);
    // amt::replace(data["Alley"], amt::tag::inplace, { {"NA", "NAA"} });

    // amt::infer<>(data,amt::tag::inplace);
    // // amt::imputer.mean(data["LotFrontage"],amt::tag::inplace, amt::tag::nan);
    // // // amt::drop_nan(data,amt::tag::inplace);
    
    // amt::pretty_string << std::setprecision(5) << std::fixed;
    // std::cout<<amt::pretty_string(data)<<'\n';
    // std::cout<<amt::describe(data)<<'\n';

    amt::frame f = {
        { "Day", {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}, amt::dtype<std::int64_t>{} },
        { "Outlook", {"Sunny", "Sunny", "Overcast", "Rain", "Rain", "Rain", "Overcast", "Sunny", "Sunny", "Rain", "Sunny", "Overcast", "Overcast", "Rain"}, amt::dtype<std::string>{} },
        { "Temperature", {"Hot", "Hot", "Hot", "Mild", "Cool", "Cool", "Cool", "Mild", "Cool", "Mild", "Mild", "Mild", "Hot", "Mild"}, amt::dtype<std::string>{} },
        { "Humidity", {"High", "High", "High", "High", "Normal", "Normal", "Normal", "High", "Normal", "Normal", "Normal", "High", "Normal", "High"}, amt::dtype<std::string>{} },
        { "Wind", {"Weak", "Strong", "Weak", "Weak", "Weak", "Strong", "Strong", "Weak", "Weak", "Weak", "Strong", "Strong", "Weak", "Strong"}, amt::dtype<std::string>{} },
        { "Plays", {"No", "No", "Yes", "Yes", "Yes", "No", "Yes", "No", "Yes", "Yes", "Yes", "Yes", "Yes", "No"}, amt::dtype<std::string>{} }
    };
    std::cout<<amt::pretty_string(f)<<'\n';
    // auto sunny = amt::filter(f, f["Outlook"] == "Sunny");
    // auto rain = amt::filter(f, f["Outlook"] == "Rain");
    // auto over = amt::filter(f, f["Outlook"] == "Overcast");
    // auto sunny = amt::filter["Outlook"](f, amt::equal("Sunny") || amt::equal("Rain") );
    // auto rain = amt::filter["Outlook"](f, amt::equal("Rain"));
    // auto over = amt::filter["Outlook"](f, amt::equal("Overcast"));
    // auto sunny_plays = amt::freq(sunny["Plays"]);
    // auto rain_plays = amt::freq(rain["Plays"]);
    // auto over_plays = amt::freq(over["Plays"]);
    // std::cout<<amt::pretty_string(sunny)<<'\n';
    // print(sunny_plays);
    // std::cout<<amt::pretty_string(rain)<<'\n';
    // print(rain_plays);
    // std::cout<<amt::pretty_string(over)<<'\n';
    // print(over_plays);
    auto temp = amt::get_dummies<>["Temperature"](f, false);
    std::cout<<amt::pretty_string(temp)<<'\n';

    return 0;
}