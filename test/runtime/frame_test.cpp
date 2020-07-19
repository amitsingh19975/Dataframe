#include <catch2/catch.hpp>
#include <frame.hpp>
#include <operator.hpp>

TEST_CASE("Empty Construction of frame", "[frame]") {
    amt::frame<> f;
    REQUIRE(f.cols() == 0ul);
    REQUIRE(f.rows() == 0ul);
    REQUIRE(f.empty() == true);
}

TEST_CASE("Construction of frame with dimensions", "[frame]") {
    {
        amt::frame<> f(10,10);
        REQUIRE(f.cols() == 10ul);
        REQUIRE(f.rows() == 10ul);
        REQUIRE(f.empty() == false);

        std::vector<std::string> col_names = {
            "0", "1", "2", "3", "4", "5",
            "6", "7", "8", "9",
        };

        REQUIRE( (col_names == f.name_to_vector()) );
    }
    {
        amt::frame<> f(10);
        REQUIRE(f.cols() == 10ul);
        REQUIRE(f.rows() == 0ul);
        REQUIRE(f.empty() == false);
    }
}

TEST_CASE("Construction of frame using initializer_list", "[frame]") {
    {
        amt::frame<> f = {
            amt::series<>{1,2,3,4,5},
            amt::series<>{1,2,3,4,5},
            amt::series<>{1,2,3,4,5},
            amt::series<>{1,2,3,4,5}
        };
        REQUIRE(f.cols() == 4ul);
        REQUIRE(f.rows() == 5ul);
        REQUIRE(f.empty() == false);
        
        for(auto const& s : f){
            REQUIRE( (s == (amt::series<>{1,2,3,4,5})) );
        }
    }
    {
        amt::frame<> f = {
            { "A", amt::series<>{1,2,3,4,5} },
            { "B", amt::series<>{1,2,3,4,5} },
            { "C", amt::series<>{1,2,3,4,5} },
            { "D", amt::series<>{1,2,3,4,5} }
        };
        REQUIRE(f.cols() == 4ul);
        REQUIRE(f.rows() == 5ul);
        REQUIRE(f.empty() == false);
        REQUIRE(f.name(0) == "A");
        REQUIRE(f.name(1) == "B");
        REQUIRE(f.name(2) == "C");
        REQUIRE(f.name(3) == "D");
        
        for(auto const& s : f){
            REQUIRE( (s == (amt::series<>{1,2,3,4,5})) );
        }
    }
    {
        REQUIRE_THROWS_AS( (amt::frame<>{
            { "A", amt::series<>{1,2,3,4,5} },
            { "B", amt::series<>{1,2,3,4,5,6} },
            { "C", amt::series<>{1,2,3,4,5} },
            { "D", amt::series<>{1,2,3,4,5} }
        }), std::runtime_error );
    }
    {
        REQUIRE_THROWS_AS( (amt::frame<>{
            amt::series<>{1,2,3,4,5},
            amt::series<>{1,2,3,4,5,6},
            amt::series<>{1,2,3,4,5},
            amt::series<>{1,2,3,4,5}
        }), std::runtime_error );
    }
}

TEST_CASE("Construction of frame using std::vector", "[frame]") {
    {
        std::vector< amt::series<> > temp = {
            amt::series<>{1,2,3,4,5},
            amt::series<>{1,2,3,4,5},
            amt::series<>{1,2,3,4,5},
            amt::series<>{1,2,3,4,5}
        };
        amt::frame<> f(std::move(temp));

        REQUIRE(f.cols() == 4ul);
        REQUIRE(f.rows() == 5ul);
        REQUIRE(f.empty() == false);
        
        for(auto const& s : f){
            REQUIRE( (s == (amt::series<>{1,2,3,4,5})) );
        }
    }
    {
        std::vector< std::pair<std::string_view,amt::series<>> > temp = {
            { "A", amt::series<>{1,2,3,4,5} },
            { "B", amt::series<>{1,2,3,4,5} },
            { "C", amt::series<>{1,2,3,4,5} },
            { "D", amt::series<>{1,2,3,4,5} }
        };
        amt::frame<> f(std::move(temp));
        REQUIRE(f.cols() == 4ul);
        REQUIRE(f.rows() == 5ul);
        REQUIRE(f.empty() == false);
        REQUIRE(f.name(0) == "A");
        REQUIRE(f.name(1) == "B");
        REQUIRE(f.name(2) == "C");
        REQUIRE(f.name(3) == "D");
        
        for(auto const& s : f){
            REQUIRE( (s == (amt::series<>{1,2,3,4,5})) );
        }
    }
    {
        std::vector< std::pair<std::string_view,amt::series<>> > temp = {
            { "A", amt::series<>{1,2,3,4,5} },
            { "B", amt::series<>{1,2,3,4,5,6} },
            { "C", amt::series<>{1,2,3,4,5} },
            { "D", amt::series<>{1,2,3,4,5} }
        };
        REQUIRE_THROWS_AS( (amt::frame<>(std::move(temp))), std::runtime_error );
    }
    {
        std::vector< amt::series<> > temp = {
            amt::series<>{1,2,3,4,5},
            amt::series<>{1,2,3,4,5,6},
            amt::series<>{1,2,3,4,5},
            amt::series<>{1,2,3,4,5}
        };
        REQUIRE_THROWS_AS( (amt::frame<>(std::move(temp))), std::runtime_error );
    }
}

TEST_CASE("Copy construction", "[frame]") {
    amt::frame<> forg = {
        amt::series<>{1,2,3,4,5},
        amt::series<>{1,2,3,4,5},
        amt::series<>{1,2,3,4,5},
        amt::series<>{1,2,3,4,5}
    };
    REQUIRE(forg.cols() == 4ul);
    REQUIRE(forg.rows() == 5ul);
    REQUIRE(forg.empty() == false);

    amt::frame<> fc(forg);
    REQUIRE(fc.cols() == 4ul);
    REQUIRE(fc.rows() == 5ul);
    REQUIRE(fc.empty() == false);

    REQUIRE(fc == forg);
}

TEST_CASE("Copy Assignment", "[frame]") {
    amt::frame<> forg = {
        amt::series<>{1,2,3,4,5},
        amt::series<>{1,2,3,4,5},
        amt::series<>{1,2,3,4,5},
        amt::series<>{1,2,3,4,5}
    };
    REQUIRE(forg.cols() == 4ul);
    REQUIRE(forg.rows() == 5ul);
    REQUIRE(forg.empty() == false);

    amt::frame<> fc;
    fc = forg;
    REQUIRE(fc.cols() == 4ul);
    REQUIRE(fc.rows() == 5ul);
    REQUIRE(fc.empty() == false);

    REQUIRE(fc == forg);
}

TEST_CASE("Move construction", "[frame]") {
    amt::frame<> forg = {
        amt::series<>{1,2,3,4,5},
        amt::series<>{1,2,3,4,5},
        amt::series<>{1,2,3,4,5},
        amt::series<>{1,2,3,4,5}
    };
    REQUIRE(forg.cols() == 4ul);
    REQUIRE(forg.rows() == 5ul);
    REQUIRE(forg.empty() == false);

    amt::frame<> fc(std::move(forg));
    REQUIRE(fc.cols() == 4ul);
    REQUIRE(fc.rows() == 5ul);
    REQUIRE(fc.empty() == false);

    for(auto const& s : fc){
        REQUIRE( (s == (amt::series<>{1,2,3,4,5})) );
    }
}

TEST_CASE("Move Assignment", "[frame]") {
    amt::frame<> forg = {
        amt::series<>{1,2,3,4,5},
        amt::series<>{1,2,3,4,5},
        amt::series<>{1,2,3,4,5},
        amt::series<>{1,2,3,4,5}
    };
    REQUIRE(forg.cols() == 4ul);
    REQUIRE(forg.rows() == 5ul);
    REQUIRE(forg.empty() == false);

    amt::frame<> fc;
    fc = std::move(forg);
    REQUIRE(fc.cols() == 4ul);
    REQUIRE(fc.rows() == 5ul);
    REQUIRE(fc.empty() == false);

    for(auto const& s : fc){
        REQUIRE( (s == (amt::series<>{1,2,3,4,5})) );
    }
}

TEST_CASE("Accessing elements using Operator[]", "[frame]") {
    amt::frame<> f = {
        amt::series<>{1,2,3,4,5},
        amt::series<>{11,21,31,41,51},
        amt::series<>{12,22,32,42,52},
        amt::series<>{13,23,33,43,53}
    };
    REQUIRE(f.cols() == 4ul);
    REQUIRE(f.rows() == 5ul);
    REQUIRE(f.empty() == false);
    REQUIRE(f[0] == (amt::series<>{1,2,3,4,5}) );
    REQUIRE(f[1] == (amt::series<>{11,21,31,41,51}) );
    REQUIRE(f[2] == (amt::series<>{12,22,32,42,52}) );
    REQUIRE(f[3] == (amt::series<>{13,23,33,43,53}) );
}

TEST_CASE("Accessing elements using at", "[frame]") {
    amt::frame<> f = {
        { "A", amt::series<>{1,2,3,4,5} },
        { "B", amt::series<>{11,21,31,41,51} },
        { "C", amt::series<>{12,22,32,42,52} },
        { "D", amt::series<>{13,23,33,43,53} }
    };
    REQUIRE(f.cols() == 4ul);
    REQUIRE(f.rows() == 5ul);
    REQUIRE(f.empty() == false);
    {
        REQUIRE(f.at(0) == (amt::series<>{1,2,3,4,5}) );
        REQUIRE(f.at(1) == (amt::series<>{11,21,31,41,51}) );
        REQUIRE(f.at(2) == (amt::series<>{12,22,32,42,52}) );
        REQUIRE(f.at(3) == (amt::series<>{13,23,33,43,53}) );
        REQUIRE_THROWS_AS( f.at(4), std::out_of_range );
    }
    {
        REQUIRE(f.at("A") == (amt::series<>{1,2,3,4,5}) );
        REQUIRE(f.at("B") == (amt::series<>{11,21,31,41,51}) );
        REQUIRE(f.at("C") == (amt::series<>{12,22,32,42,52}) );
        REQUIRE(f.at("D") == (amt::series<>{13,23,33,43,53}) );
    }
    {
        REQUIRE(f.at("A",0) == amt::storage<>(1) );
        REQUIRE(f.at("A",1) == amt::storage<>(2) );
        REQUIRE(f.at("A",2) == amt::storage<>(3) );
        REQUIRE(f.at("A",3) == amt::storage<>(4) );
        REQUIRE(f.at("A",4) == amt::storage<>(5) );
        
        REQUIRE(f.at(0,0) == amt::storage<>(1) );
        REQUIRE(f.at(0,1) == amt::storage<>(2) );
        REQUIRE(f.at(0,2) == amt::storage<>(3) );
        REQUIRE(f.at(0,3) == amt::storage<>(4) );
        REQUIRE(f.at(0,4) == amt::storage<>(5) );
    }
}

TEST_CASE("Accessing elements using Operator()", "[frame]") {
    amt::frame<> f = {
        { "A", amt::series<>{1,2,3,4,5} },
        { "B", amt::series<>{11,21,31,41,51} },
        { "C", amt::series<>{12,22,32,42,52} },
        { "D", amt::series<>{13,23,33,43,53} }
    };
    REQUIRE(f.cols() == 4ul);
    REQUIRE(f.rows() == 5ul);
    REQUIRE(f.empty() == false);
    {
        REQUIRE(f(0ul) == (amt::series<>{1,2,3,4,5}) );
        REQUIRE(f(1ul) == (amt::series<>{11,21,31,41,51}) );
        REQUIRE(f(2ul) == (amt::series<>{12,22,32,42,52}) );
        REQUIRE(f(3ul) == (amt::series<>{13,23,33,43,53}) );
        REQUIRE_THROWS_AS( f(4ul), std::out_of_range );
    }
    {
        REQUIRE(f("A") == (amt::series<>{1,2,3,4,5}) );
        REQUIRE(f("B") == (amt::series<>{11,21,31,41,51}) );
        REQUIRE(f("C") == (amt::series<>{12,22,32,42,52}) );
        REQUIRE(f("D") == (amt::series<>{13,23,33,43,53}) );
    }
    {
        REQUIRE(f("A",0ul) == amt::storage<>(1) );
        REQUIRE(f("A",1ul) == amt::storage<>(2) );
        REQUIRE(f("A",2ul) == amt::storage<>(3) );
        REQUIRE(f("A",3ul) == amt::storage<>(4) );
        REQUIRE(f("A",4ul) == amt::storage<>(5) );
        
        REQUIRE(f(0ul,0ul) == amt::storage<>(1) );
        REQUIRE(f(0ul,1ul) == amt::storage<>(2) );
        REQUIRE(f(0ul,2ul) == amt::storage<>(3) );
        REQUIRE(f(0ul,3ul) == amt::storage<>(4) );
        REQUIRE(f(0ul,4ul) == amt::storage<>(5) );
    }
}

TEST_CASE("Accessing column names", "[frame]") {
    amt::frame<> f = {
        { "A", amt::series<>{1,2,3,4,5} },
        { "B", amt::series<>{11,21,31,41,51} },
        { "C", amt::series<>{12,22,32,42,52} },
        { "D", amt::series<>{13,23,33,43,53} }
    };
    REQUIRE(f.cols() == 4ul);
    REQUIRE(f.rows() == 5ul);
    REQUIRE(f.empty() == false);
    REQUIRE(f.name(0) == "A" );
    REQUIRE(f.name(1) == "B" );
    REQUIRE(f.name(2) == "C" );
    REQUIRE(f.name(3) == "D" );
}

TEST_CASE("Getting column names as vector", "[frame]") {
    amt::frame<> f = {
        { "A", amt::series<>{1,2,3,4,5} },
        { "B", amt::series<>{11,21,31,41,51} },
        { "C", amt::series<>{12,22,32,42,52} },
        { "D", amt::series<>{13,23,33,43,53} }
    };
    REQUIRE(f.cols() == 4ul);
    REQUIRE(f.rows() == 5ul);
    REQUIRE(f.empty() == false);
    
    std::vector<std::string> col_names = {
        "A", "B", "C", "D"
    };

    REQUIRE( (col_names == f.name_to_vector()) );
}

TEST_CASE("Replacing column name", "[frame]") {
    amt::frame<> f = {
        { "A", amt::series<>{1,2,3,4,5} },
        { "B", amt::series<>{11,21,31,41,51} },
        { "C", amt::series<>{12,22,32,42,52} },
        { "D", amt::series<>{13,23,33,43,53} }
    };
    REQUIRE(f.cols() == 4ul);
    REQUIRE(f.rows() == 5ul);
    REQUIRE(f.empty() == false);
    REQUIRE( ((std::vector<std::string>{"A", "B", "C", "D"}) == f.name_to_vector()) );

    f.replace(0,"F");
    REQUIRE( ((std::vector<std::string>{"F", "B", "C", "D"}) == f.name_to_vector()) );

}

TEST_CASE("Setting column names using vector", "[frame]") {
    {
        amt::frame<> f = {
            amt::series<>{1,2,3,4,5},
            amt::series<>{11,21,31,41,51},
            amt::series<>{12,22,32,42,52},
            amt::series<>{13,23,33,43,53}
        };
        REQUIRE(f.cols() == 4ul);
        REQUIRE(f.rows() == 5ul);
        REQUIRE(f.empty() == false);
        REQUIRE( ((std::vector<std::string>{"0", "1", "2", "3"}) == f.name_to_vector()) );
        std::vector<std::string> new_col_name = {
            "A","B","C","D"
        };
        f.set_name(new_col_name);
        REQUIRE( (new_col_name == f.name_to_vector()) );
    }
    {
        amt::frame<> f = {
            amt::series<>{1,2,3,4,5},
            amt::series<>{11,21,31,41,51},
            amt::series<>{12,22,32,42,52},
            amt::series<>{13,23,33,43,53}
        };

        REQUIRE(f.cols() == 4ul);
        REQUIRE(f.rows() == 5ul);
        REQUIRE(f.empty() == false);
        REQUIRE( ((std::vector<std::string>{"0", "1", "2", "3"}) == f.name_to_vector()) );

        amt::frame<> otherf = {
            { "A", amt::series<>{1,2,3,4,5} },
            { "B", amt::series<>{11,21,31,41,51} },
            { "C", amt::series<>{12,22,32,42,52} },
            { "D", amt::series<>{13,23,33,43,53} }
        };

        REQUIRE(otherf.cols() == 4ul);
        REQUIRE(otherf.rows() == 5ul);
        REQUIRE(otherf.empty() == false);

        f.set_name(otherf);
        REQUIRE( (otherf.name_to_vector() == f.name_to_vector()) );
    }
    {
        amt::frame<> f = {
            amt::series<>{1,2,3,4,5},
            amt::series<>{11,21,31,41,51},
            amt::series<>{12,22,32,42,52},
            amt::series<>{13,23,33,43,53}
        };

        REQUIRE(f.cols() == 4ul);
        REQUIRE(f.rows() == 5ul);
        REQUIRE(f.empty() == false);
        REQUIRE( ((std::vector<std::string>{"0", "1", "2", "3"}) == f.name_to_vector()) );

        amt::frame<> otherf = {
            { "A", amt::series<>{1,2,3,4,5} },
            { "B", amt::series<>{11,21,31,41,51} },
            { "C", amt::series<>{12,22,32,42,52} },
            { "D", amt::series<>{13,23,33,43,53} },
            { "E", amt::series<>{13,23,33,43,53} },
            { "F", amt::series<>{13,23,33,43,53} }
        };

        REQUIRE(otherf.cols() == 6ul);
        REQUIRE(otherf.rows() == 5ul);
        REQUIRE(otherf.empty() == false);

        auto fv = otherf(amt::slice( amt::slice_last(3) ));
        REQUIRE(fv.cols() == 4ul);
        REQUIRE(fv.rows() == 5ul);
        REQUIRE(fv.empty() == false);

        f.set_name(fv);
        REQUIRE( (fv.name_to_vector() == f.name_to_vector()) );
    }
}

TEST_CASE("Reseting column name to default", "[frame]") {
    amt::frame<> f = {
        { "A", amt::series<>{1,2,3,4,5} },
        { "B", amt::series<>{11,21,31,41,51} },
        { "C", amt::series<>{12,22,32,42,52} },
        { "D", amt::series<>{13,23,33,43,53} }
    };
    REQUIRE(f.cols() == 4ul);
    REQUIRE(f.rows() == 5ul);
    REQUIRE(f.empty() == false);
    REQUIRE( ((std::vector<std::string>{"A", "B", "C", "D"}) == f.name_to_vector()) );

    f.reset_name();
    REQUIRE( ((std::vector<std::string>{"0", "1", "2", "3"}) == f.name_to_vector()) );

}

TEST_CASE("Getting View from frame", "[frame]") {
    amt::frame<> f = {
        { "A", amt::series<>{1,2,3,4,5} },
        { "B", amt::series<>{11,21,31,41,51} },
        { "C", amt::series<>{12,22,32,42,52} },
        { "D", amt::series<>{13,23,33,43,53} },
        { "E", amt::series<>{13,23,33,43,53} },
        { "F", amt::series<>{13,23,33,43,53} }
    };
    REQUIRE(f.cols() == 6ul);
    REQUIRE(f.rows() == 5ul);
    REQUIRE(f.empty() == false);
    
    {
        auto temp = amt::frame<> {
            { "A", amt::series<>{1,2,3,4,5} },
            { "C", amt::series<>{12,22,32,42,52} },
            { "E", amt::series<>{13,23,33,43,53} }
        };
        auto fv = f( amt::slice(amt::slice_stride(2)) );
        REQUIRE(temp == fv);
    }

}

TEST_CASE("Erasing elements from frame", "[frame]") {
    
    {
        amt::frame<> f = {
            { "A", amt::series<>{1,2,3,4,5} },
            { "B", amt::series<>{11,21,31,41,51} },
            { "C", amt::series<>{12,22,32,42,52} },
            { "D", amt::series<>{13,23,33,43,53} },
            { "E", amt::series<>{13,23,33,43,53} },
            { "F", amt::series<>{13,23,33,43,53} }
        };
        
        amt::frame<> fres = {
            { "B", amt::series<>{11,21,31,41,51} },
            { "C", amt::series<>{12,22,32,42,52} },
            { "D", amt::series<>{13,23,33,43,53} },
            { "E", amt::series<>{13,23,33,43,53} },
            { "F", amt::series<>{13,23,33,43,53} }
        };
        REQUIRE(f.cols() == 6ul);
        REQUIRE(f.rows() == 5ul);
        REQUIRE(f.empty() == false);
        
        (void)f.erase(f.begin() + 0);
        f.normalize_index();

        REQUIRE(f.cols() == 5ul);
        REQUIRE(f.rows() == 5ul);
        REQUIRE(f.empty() == false);
        REQUIRE(f.index("B") == 0ul);
        REQUIRE(f.index("C") == 1ul);
        REQUIRE(f.index("D") == 2ul);
        REQUIRE(f.index("E") == 3ul);
        REQUIRE(f.index("F") == 4ul);

        REQUIRE(f == fres);
    }
    {
        amt::frame<> f = {
            { "A", amt::series<>{1,2,3,4,5} },
            { "B", amt::series<>{11,21,31,41,51} },
            { "C", amt::series<>{12,22,32,42,52} },
            { "D", amt::series<>{13,23,33,43,53} },
            { "E", amt::series<>{13,23,33,43,53} },
            { "F", amt::series<>{13,23,33,43,53} }
        };
        
        REQUIRE(f.cols() == 6ul);
        REQUIRE(f.rows() == 5ul);
        REQUIRE(f.empty() == false);
        
        amt::frame<> fres = {};
        (void)f.erase(f.begin(), f.end());
        REQUIRE(f.cols() == 0ul);
        REQUIRE(f.rows() == 0ul);
        REQUIRE(f.empty() == true);

        REQUIRE(f == fres);
    }
}

TEST_CASE("Erasing column name from frame", "[frame]") {
    
    {
        amt::frame<> f = {
            { "A", amt::series<>{1,2,3,4,5} },
            { "B", amt::series<>{11,21,31,41,51} },
            { "C", amt::series<>{12,22,32,42,52} },
            { "D", amt::series<>{13,23,33,43,53} },
            { "E", amt::series<>{13,23,33,43,53} },
            { "F", amt::series<>{13,23,33,43,53} }
        };
        
        REQUIRE(f.cols() == 6ul);
        REQUIRE(f.rows() == 5ul);
        REQUIRE(f.empty() == false);
        f.erase_name(0ul);
        REQUIRE(f.name(0ul) == "0");
    }
    
    {
        amt::frame<> f = {
            { "A", amt::series<>{1,2,3,4,5} },
            { "B", amt::series<>{11,21,31,41,51} },
            { "C", amt::series<>{12,22,32,42,52} },
            { "D", amt::series<>{13,23,33,43,53} },
            { "E", amt::series<>{13,23,33,43,53} },
            { "F", amt::series<>{13,23,33,43,53} }
        };
        
        REQUIRE(f.cols() == 6ul);
        REQUIRE(f.rows() == 5ul);
        REQUIRE(f.empty() == false);
        (void)f.erase_name("A");
        REQUIRE(f.name(0ul) == "0");
    }

}

TEST_CASE("Erasing row", "[frame]") {
    
    {
        amt::frame<> f = {
            { "A", amt::series<>{1,2,3,4,5} },
            { "B", amt::series<>{11,21,31,41,51} },
            { "C", amt::series<>{12,22,32,42,52} },
            { "D", amt::series<>{13,23,33,43,53} },
            { "E", amt::series<>{13,23,33,43,53} },
            { "F", amt::series<>{13,23,33,43,53} }
        };
        
        REQUIRE(f.cols() == 6ul);
        REQUIRE(f.rows() == 5ul);
        REQUIRE(f.empty() == false);
        
        amt::frame<> fres = {
            { "A", amt::series<>{2,3,4,5} },
            { "B", amt::series<>{21,31,41,51} },
            { "C", amt::series<>{22,32,42,52} },
            { "D", amt::series<>{23,33,43,53} },
            { "E", amt::series<>{23,33,43,53} },
            { "F", amt::series<>{23,33,43,53} }
        };
        
        REQUIRE(fres.cols() == 6ul);
        REQUIRE(fres.rows() == 4ul);
        REQUIRE(fres.empty() == false);
        f.erase_row(0ul);
        REQUIRE(f == fres);
    }

}

TEST_CASE("Resizing frame", "[frame]") {
    
    {
        amt::frame<> f;
        REQUIRE(f.cols() == 0ul);
        REQUIRE(f.rows() == 0ul);
        REQUIRE(f.empty() == true);

        f.resize(4ul);
        REQUIRE(f.cols() == 4ul);
        REQUIRE(f.rows() == 0ul);
        REQUIRE(f.empty() == false);

        REQUIRE(f.name(0ul) == "0");
        REQUIRE(f.name(1ul) == "1");
        REQUIRE(f.name(2ul) == "2");
        REQUIRE(f.name(3ul) == "3");
        
    }
    
    {
        amt::frame<> f;
        REQUIRE(f.cols() == 0ul);
        REQUIRE(f.rows() == 0ul);
        REQUIRE(f.empty() == true);

        f.resize(4ul,4ul);
        REQUIRE(f.cols() == 4ul);
        REQUIRE(f.rows() == 4ul);
        REQUIRE(f.empty() == false);

        REQUIRE(f.name(0ul) == "0");
        REQUIRE(f.name(1ul) == "1");
        REQUIRE(f.name(2ul) == "2");
        REQUIRE(f.name(3ul) == "3");
        
    }
    
    {
        amt::frame<> f;
        REQUIRE(f.cols() == 0ul);
        REQUIRE(f.rows() == 0ul);
        REQUIRE(f.empty() == true);

        amt::series<> s = {1,2,3,4};

        f.resize(4ul,s);
        REQUIRE(f.cols() == 4ul);
        REQUIRE(f.rows() == 4ul);
        REQUIRE(f.empty() == false);

        REQUIRE(f.name(0ul) == "0");
        REQUIRE(f.name(1ul) == "1");
        REQUIRE(f.name(2ul) == "2");
        REQUIRE(f.name(3ul) == "3");

        for(auto const& el : f){
            REQUIRE(el == s);
        }
        
    }

}
