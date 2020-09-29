#include <algorithm>
#include <frame.hpp>
#include <functional>
#include <functions.hpp>
#include <iostream>
#include <numeric>
#include <dataframe.hpp>
#include <parser/csv.hpp>
#include <linear_regression.hpp>
#include <plot/matplotlibcpp.h>

void clean_data(amt::frame<>& f){
    amt::name_list n = {
        "id",
        "date",
        // "price",
        "bedrooms",
        "bathrooms",
        "sqft_living",
        "sqft_lot",
        "floors",
        "waterfront",
        "view",
        "condition",
        "grade",
        "sqft_above",
        "sqft_basement",
        "yr_built",
        "yr_renovated",
        "zipcode",
        "lat",
        "long",
        "sqft_living15",
        // "sqft_lot15" 
    };
    amt::drop_cols(f,amt::in_place, std::move(n));
    amt::filter(f,amt::in_place,[](std::string_view val){
        std::string temp(val);
        std::transform(temp.begin(),temp.end(), temp.begin(), [](auto c){return std::tolower(c);});
        if( temp.empty() ||  (temp == "nan") ) return true;
        return false;
    });

    amt::to<double>(f,amt::in_place);
}

inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}


inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}


inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}
std::string norm_str(std::string name){
    trim(name);
    name[0] = static_cast<char>(std::toupper(name[0]));
    std::transform(name.begin(),name.end(),name.begin(),[](auto const& c){
        if( c == '_' ) return ' ';
        return c;
    });
    for(auto i = 2ul; i < name.size(); ++i){
        auto& p = name[i - 1];
        auto& c = name[i];
        if(p == ' ') c = static_cast<char>(std::toupper(c));
    }
    return name;
}

void plot(amt::frame<> const& x, amt::frame<> const& y){
    namespace plt = matplotlibcpp;
    std::vector<double> temp_x(x.rows()), temp_y(y.rows());

    auto i = 0ul;
    for(auto const& el : y[0]){
        auto val = el.template as<double>();
        temp_y[i++] = val;
    }

    i = 0ul;
    for(auto const& el : x[0]){
        auto val = el.template as<double>();
        temp_x[i++] = val;
    }
    // plt::figure_size(1200, 780);
    plt::title("House Pricing");
    plt::scatter(temp_x,temp_y);
    plt::xlabel( norm_str(x.name(0)) );
    plt::ylabel( norm_str(y.name(0)) );
    plt::show();
}

void plot_pred(double s, double c, amt::frame<> const& x, amt::frame<> const& y){
    namespace plt = matplotlibcpp;
    std::vector<double> temp_x(x.rows()), temp_y(y.rows());

    auto i = 0ul;
    auto max = 0.0;
    for(auto const& el : y[0]){
        auto val = el.template as<double>();
        temp_y[i++] = val;
    }

    i = 0ul;
    for(auto const& el : x[0]){
        auto val = el.template as<double>();
        temp_x[i++] = val;
        max = std::max(val,max);
    }

    auto sz = static_cast<std::size_t>(max);
    std::vector<double> range_x(sz), temp_p(sz);
    std::iota(range_x.begin(), range_x.end(), 0);

    for(auto j = 0ul; j < sz; ++j) 
        temp_p[j] = c + s * static_cast<double>(j);
    // plt::figure_size(1200, 780);
    plt::title("House Pricing");
    plt::scatter(temp_x,temp_y);
    plt::plot(range_x,temp_p,"r");
    plt::xlabel( norm_str(x.name(0)) );
    plt::ylabel( norm_str(y.name(0)) );
    plt::show();
}

int main(int, char **) {

    auto temp = amt::read_csv("/Users/amit/Desktop/code/dataframe/src/test.csv", true);
    amt::to<float>(temp,amt::in_place);
    std::cout<<std::fixed<<amt::svar(temp["price"])<<'\n';
    // clean_data(temp);
    
    // auto i = 0ul;
    // auto y = amt::drop_cols(temp,amt::out_place,[&i](auto const&){
    //     if(i++ > 0) return true;
    //     return false;
    // });
    // auto x = amt::drop_cols(temp,amt::out_place,amt::index_list{0});

    // auto split_ratio = 0.25;
    // auto rsz = y.rows();
    // auto training_sz = static_cast<std::size_t>(split_ratio * static_cast<double>(rsz));
    // // auto testing_sz = rsz - training_sz;

    // auto x_train = amt::drop_rows(x,amt::out_place,training_sz);
    // auto y_train = amt::drop_rows(y,amt::out_place,training_sz);

    // auto x_test = amt::drop_rows(x,amt::out_place,0, training_sz);
    // auto y_test = amt::drop_rows(y,amt::out_place,0, training_sz);
    // // plot(x,y);

    // auto l_model = amt::linear_regression(x_train,y_train);
    // plot_pred(l_model.slope(), l_model.intercept(), x_test, y_test);
    // auto pred = l_model.predict(x_test);
    // y_test.push_back(std::move(pred));

    return 0;
}