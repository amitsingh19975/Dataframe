#include <catch2/catch.hpp>
#include <series.hpp>
#include <slice.hpp>

SCENARIO("Series Construction", "[Series]") {
    GIVEN( "A empty series object" ){
        amt::series<> ser;

        REQUIRE(ser.size() == 0);
        REQUIRE(ser.is_string() == false);
        REQUIRE(ser.is_bool() == false);
        REQUIRE(ser.is_integer() == false);
        REQUIRE(ser.is_char() == false);
        REQUIRE(ser.is_float() == false);
        REQUIRE(ser.is_double() == false);
        REQUIRE(ser.empty() == true);
        REQUIRE(ser.check_types() == true);

        WHEN( "size is increased with no initialization of value" ){
            ser.resize(2);

            THEN( "the size changes" ){
                REQUIRE(ser.size() == 2);
                REQUIRE(ser.is_string() == false);
                REQUIRE(ser.is_bool() == false);
                REQUIRE(ser.is_integer() == false);
                REQUIRE(ser.is_char() == false);
                REQUIRE(ser.is_float() == false);
                REQUIRE(ser.is_double() == false);
                REQUIRE(ser.empty() == false);
                REQUIRE(ser.check_types() == false);
            }
        }

        WHEN( "size is increased with initialization of value" ){
            ser.resize(2, 10);

            THEN( "the size changes" ){
                REQUIRE(ser.size() == 2);
                REQUIRE(ser.is_string() == false);
                REQUIRE(ser.is_bool() == false);
                REQUIRE(ser.is_integer() == true);
                REQUIRE(ser.is_char() == false);
                REQUIRE(ser.is_float() == false);
                REQUIRE(ser.is_double() == false);
                REQUIRE(ser.empty() == false);
                REQUIRE(ser.check_types() == true);
            }
        }

        WHEN( "size is increased with initialization of value with different type" ){
            ser.resize(2, 10);
            REQUIRE_THROWS_AS( ser.resize(3, "Hello"), std::runtime_error );
        }

        WHEN( "the elements are pushed or emplaced" ){
            ser.push_back(10);

            THEN( "the one element is in the container" ){
                REQUIRE(ser.size() == 1ul);
                REQUIRE(ser.is_string() == false);
                REQUIRE(ser.is_bool() == false);
                REQUIRE(ser.is_integer() == true);
                REQUIRE(ser.is_char() == false);
                REQUIRE(ser.is_float() == false);
                REQUIRE(ser.is_double() == false);
                REQUIRE(ser.empty() == false);
                REQUIRE(ser.check_types() == true);
            }
            ser.emplace_back<int>(20);

            THEN( "the two element are in the container" ){
                REQUIRE(ser.size() == 2ul);
                REQUIRE(ser.is_string() == false);
                REQUIRE(ser.is_bool() == false);
                REQUIRE(ser.is_integer() == true);
                REQUIRE(ser.is_char() == false);
                REQUIRE(ser.is_float() == false);
                REQUIRE(ser.is_double() == false);
                REQUIRE(ser.empty() == false);
                REQUIRE(ser.check_types() == true);
            }
        }

        WHEN( "the elements are pushed or emplaced with different type" ){
            ser.push_back("Hello");
            REQUIRE_THROWS_AS( ser.push_back(3), std::runtime_error );
            REQUIRE_THROWS_AS( ser.emplace_back<int>(3), std::runtime_error );
        }
    }

    GIVEN( "A series object with initial size" ){
        amt::series<> ser(10);

        REQUIRE(ser.size() == 10);
        REQUIRE(ser.is_string() == false);
        REQUIRE(ser.is_bool() == false);
        REQUIRE(ser.is_integer() == false);
        REQUIRE(ser.is_char() == false);
        REQUIRE(ser.is_float() == false);
        REQUIRE(ser.is_double() == false);
        REQUIRE(ser.empty() == false);
        REQUIRE(ser.check_types() == false);
    }

    GIVEN( "A series object initialized with initializer_list" ){
        amt::series<> ser = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        REQUIRE(ser.size() == 10);
        REQUIRE(ser.is_string() == false);
        REQUIRE(ser.is_bool() == false);
        REQUIRE(ser.is_integer() == true);
        REQUIRE(ser.is_char() == false);
        REQUIRE(ser.is_float() == false);
        REQUIRE(ser.is_double() == false);
        REQUIRE(ser.empty() == false);
        REQUIRE(ser.check_types() == true);

        int i = 1;
        for(auto const& el : ser){
            REQUIRE(el.cast<int>() == i);
            ++i;
        }
    }

    GIVEN( "A series object initialized with std::vector" ){
        std::vector<std::string> v = {"A", "B", "C", "D"};
        amt::series<> ser(v);

        REQUIRE(ser.size() == v.size());
        REQUIRE(ser.is_string() == true);
        REQUIRE(ser.is_bool() == false);
        REQUIRE(ser.is_integer() == false);
        REQUIRE(ser.is_char() == false);
        REQUIRE(ser.is_float() == false);
        REQUIRE(ser.is_double() == false);
        REQUIRE(ser.empty() == false);
        REQUIRE(ser.check_types() == true);

        std::size_t i = 0ul;
        for(auto const& el : ser){
            REQUIRE(el.cast<std::string>() == v[i]);
            ++i;
        }
    }

    GIVEN( "A series object initialized with series view" ){
        amt::series<> org = {1,2,3,4,5,6,7,8,9,10};
        auto view = org(amt::slice(amt::slice_stride{2}));
        amt::series<> ser(view);

        REQUIRE(ser.size() == view.size());
        REQUIRE(ser.is_string() == false);
        REQUIRE(ser.is_bool() == false);
        REQUIRE(ser.is_integer() == true);
        REQUIRE(ser.is_char() == false);
        REQUIRE(ser.is_float() == false);
        REQUIRE(ser.is_double() == false);
        REQUIRE(ser.empty() == false);
        REQUIRE(ser.check_types() == true);

        std::size_t i = 0ul;
        for(auto const& el : view){
            REQUIRE(el.cast<int>() == ser[i].cast<int>());
            ++i;
        }
    }
}

TEST_CASE("Series copy construction", "[Series]"){
    amt::series<> ser = {1, 2, 3, 4, 5, 6};
    amt::series<> test(ser);

    REQUIRE(test.size() == ser.size());
    REQUIRE(test.is_string() == false);
    REQUIRE(test.is_bool() == false);
    REQUIRE(test.is_integer() == true);
    REQUIRE(test.is_char() == false);
    REQUIRE(test.is_float() == false);
    REQUIRE(test.is_double() == false);
    REQUIRE(test.empty() == false);
    REQUIRE(test.check_types() == true);

    std::size_t i = 0ul;
    for(auto const& el : test){
        REQUIRE(el.cast<int>() == ser[i].cast<int>());
        ++i;
    }
}

TEST_CASE("Series copy Assignment", "[Series]"){
    amt::series<> ser = {1, 2, 3, 4, 5, 6};
    amt::series<> test;
    test = ser;

    REQUIRE(test.size() == ser.size());
    REQUIRE(test.is_string() == false);
    REQUIRE(test.is_bool() == false);
    REQUIRE(test.is_integer() == true);
    REQUIRE(test.is_char() == false);
    REQUIRE(test.is_float() == false);
    REQUIRE(test.is_double() == false);
    REQUIRE(test.empty() == false);
    REQUIRE(test.check_types() == true);

    std::size_t i = 0ul;
    for(auto const& el : test){
        REQUIRE(el.cast<int>() == ser[i].cast<int>());
        ++i;
    }
}

TEST_CASE("Series move construction", "[Series]"){
    amt::series<> ser = {1, 2, 3, 4, 5, 6};
    amt::series<> test(std::move(ser));

    REQUIRE(test.size() == 6);
    REQUIRE(test.is_string() == false);
    REQUIRE(test.is_bool() == false);
    REQUIRE(test.is_integer() == true);
    REQUIRE(test.is_char() == false);
    REQUIRE(test.is_float() == false);
    REQUIRE(test.is_double() == false);
    REQUIRE(test.empty() == false);
    REQUIRE(test.check_types() == true);

    int i = 1;
    for(auto const& el : test){
        REQUIRE(el.cast<int>() == i);
        ++i;
    }
}

TEST_CASE("Series move Assignment", "[Series]"){
    amt::series<> ser = {1, 2, 3, 4, 5, 6};
    amt::series<> test;
    test = std::move(ser);

    REQUIRE(test.size() == 6);
    REQUIRE(test.is_string() == false);
    REQUIRE(test.is_bool() == false);
    REQUIRE(test.is_integer() == true);
    REQUIRE(test.is_char() == false);
    REQUIRE(test.is_float() == false);
    REQUIRE(test.is_double() == false);
    REQUIRE(test.empty() == false);
    REQUIRE(test.check_types() == true);

    int i = 1;
    for(auto const& el : test){
        REQUIRE(el.cast<int>() == i);
        ++i;
    }
}


TEST_CASE("Series constructing view", "[Series]"){
    amt::series<> ser = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto test = ser( amt::slice(0ul, 10ul, 2ul) );

    REQUIRE(test.size() == 5 );

    std::size_t j = 0ul;
    for(auto i = 0ul; i < ser.size(); i += 2ul){
        REQUIRE( test[j++].cast<int>() == ser[i].cast<int>() );
    }
}

SCENARIO("Erasing elements from series", "[Series]"){
    amt::series<> ser = {1, 2, 3, 4, 5, 6};

    REQUIRE(ser.size() == 6);
    REQUIRE(ser.is_string() == false);
    REQUIRE(ser.is_bool() == false);
    REQUIRE(ser.is_integer() == true);
    REQUIRE(ser.is_char() == false);
    REQUIRE(ser.is_float() == false);
    REQUIRE(ser.is_double() == false);
    REQUIRE(ser.empty() == false);
    REQUIRE(ser.check_types() == true);

    for(auto i = 0ul; i < ser.size(); ++i){
        REQUIRE( ser[i].cast<int>() == (i + 1) );
    }

    GIVEN( "if the position is a iterator" ){
        auto it = ser.begin();
        (void)ser.erase(it);
        REQUIRE(ser.size() == 5);
        
        for(auto i = 0ul; i < ser.size(); ++i){
            REQUIRE( ser[i].cast<int>() == (i + 2) );
        }
    }

    GIVEN( "if the position is a size" ){
        (void)ser.erase(0);
        REQUIRE(ser.size() == 5);
        
        for(auto i = 0ul; i < ser.size(); ++i){
            REQUIRE( ser[i].cast<int>() == (i + 2) );
        }
    }

    GIVEN( "if the position is a range" ){
        (void)ser.erase(ser.begin(), ser.end());
        REQUIRE(ser.size() == 0);
    }
    
}


SCENARIO("Series type to string", "[Series]"){
    
    GIVEN( "the container is empty" ){
        amt::series<> ser;

        REQUIRE(ser.size() == 0);
        REQUIRE(ser.is_string() == false);
        REQUIRE(ser.is_bool() == false);
        REQUIRE(ser.is_integer() == false);
        REQUIRE(ser.is_char() == false);
        REQUIRE(ser.is_float() == false);
        REQUIRE(ser.is_double() == false);
        REQUIRE(ser.empty() == true);
        REQUIRE(ser.check_types() == true);

        REQUIRE(amt::type_to_string(ser) == "Empty");
    }
    
    GIVEN( "the container is not empty" ){
        amt::series<> ser = {1,2,3,4,5};

        REQUIRE(ser.size() == 5);
        REQUIRE(ser.is_string() == false);
        REQUIRE(ser.is_bool() == false);
        REQUIRE(ser.is_integer() == true);
        REQUIRE(ser.is_char() == false);
        REQUIRE(ser.is_float() == false);
        REQUIRE(ser.is_double() == false);
        REQUIRE(ser.empty() == false);
        REQUIRE(ser.check_types() == true);

        REQUIRE(amt::type_to_string(ser) == "std::int32_t");
    }
    
}
