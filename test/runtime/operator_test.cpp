#include <catch2/catch.hpp>
#include <dataframe.hpp>

TEST_CASE( "Operator (==) for box object", "[Operator==]" ){
    {
        amt::box<> lhs(10);
        amt::box<> rhs(10);

        REQUIRE( lhs == rhs );
    }
    {
        amt::box<> lhs("Hello");
        amt::box<> rhs("Hello");

        REQUIRE( lhs == rhs );
    }
    {
        amt::box<> lhs('A');
        amt::box<> rhs('A');

        REQUIRE( lhs == rhs );
    }
}

TEST_CASE( "Operator (!=) for box object", "[Operator!=]" ){
    {
        amt::box<> lhs(10);
        amt::box<> rhs(20);

        REQUIRE( lhs != rhs );
    }
    {
        amt::box<> lhs("Hello");
        amt::box<> rhs("World");

        REQUIRE( lhs != rhs );
    }
    {
        amt::box<> lhs('A');
        amt::box<> rhs('B');

        REQUIRE( lhs != rhs );
    }
}


TEST_CASE( "Operator (<) for box object", "[Operator<]" ){
    {
        amt::box<> lhs(10);
        amt::box<> rhs(20);

        REQUIRE( lhs < rhs );
    }
    {
        amt::box<> lhs("Hello");
        amt::box<> rhs("World");

        REQUIRE( lhs < rhs );
    }
    {
        amt::box<> lhs('A');
        amt::box<> rhs('B');

        REQUIRE( lhs < rhs );
    }
}

TEST_CASE( "Operator (<=) for box object", "[Operator<=]" ){
    {
        amt::box<> lhs(10);
        amt::box<> rhs(10);

        REQUIRE( lhs <= rhs );
    }
    {
        amt::box<> lhs("Hello");
        amt::box<> rhs("Hello");

        REQUIRE( lhs <= rhs );
    }
    {
        amt::box<> lhs('A');
        amt::box<> rhs('A');

        REQUIRE( lhs <= rhs );
    }
}

TEST_CASE( "Operator (>) for box object", "[Operator>]" ){
    {
        amt::box<> lhs(20);
        amt::box<> rhs(10);

        REQUIRE( lhs > rhs );
    }
    {
        amt::box<> lhs("World");
        amt::box<> rhs("Hello");

        REQUIRE( lhs > rhs );
    }
    {
        amt::box<> lhs('B');
        amt::box<> rhs('A');

        REQUIRE( lhs > rhs );
    }
}

TEST_CASE( "Operator (>=) for box object", "[Operator>=]" ){
    {
        amt::box<> lhs(10);
        amt::box<> rhs(10);

        REQUIRE( lhs >= rhs );
    }
    {
        amt::box<> lhs("Hello");
        amt::box<> rhs("Hello");

        REQUIRE( lhs >= rhs );
    }
    {
        amt::box<> lhs('A');
        amt::box<> rhs('A');

        REQUIRE( lhs >= rhs );
    }
}

TEST_CASE( "Operator (+) for box object", "[Operator+]" ){
    {
        amt::box<> lhs(10);
        amt::box<> rhs(10);
        amt::box<> res(20);

        REQUIRE( (lhs + rhs) == res );
    }
    {
        amt::box<> lhs("Hello");
        amt::box<> rhs("Hello");
        amt::box<> res("HelloHello");

        REQUIRE( (lhs + rhs) == res );
    }
    {
        amt::box<> lhs('A');
        amt::box<> rhs('A');
        amt::box<> res('A' + 'A');

        REQUIRE( (lhs + rhs) == res );
    }
}

TEST_CASE( "Operator (-) for box object", "[Operator-]" ){
    {
        amt::box<> lhs(10);
        amt::box<> rhs(10);
        amt::box<> res(0);

        REQUIRE( (lhs - rhs) == res );
    }
    {
        amt::box<> lhs('A');
        amt::box<> rhs('A');
        amt::box<> res(0);

        REQUIRE( (lhs - rhs) == res );
    }
}

TEST_CASE( "Operator (*) for box object", "[Operator*]" ){
    {
        amt::box<> lhs(10);
        amt::box<> rhs(10);
        amt::box<> res(100);

        REQUIRE( (lhs * rhs) == res );
    }
    {
        amt::box<> lhs(20.f);
        amt::box<> rhs(20.f);
        amt::box<> res(400.f);

        REQUIRE( (lhs * rhs) == res );
    }
}

TEST_CASE( "Operator (/) for box object", "[Operator*]" ){
    {
        amt::box<> lhs(10);
        amt::box<> rhs(10);
        amt::box<> res(1);

        REQUIRE( (lhs / rhs) == res );
    }
    {
        amt::box<> lhs(30.f);
        amt::box<> rhs(20.f);
        amt::box<> res((30.f / 20.f));

        REQUIRE( (lhs / rhs) == res );
    }
}

TEST_CASE( "Operator (==) for series object", "[Operator==]" ){
    {
        amt::series<> lhs(100,10);
        amt::series<> rhs(100,10);

        REQUIRE( lhs == rhs );
    }
    {
        amt::series<> lhs(100,"Hello");
        amt::series<> rhs(100,"Hello");

        REQUIRE( lhs == rhs );
    }
    {
        amt::series<> lhs(100,'A');
        amt::series<> rhs(100,'A');

        REQUIRE( lhs == rhs );
    }
}

TEST_CASE( "Operator (!=) for series object", "[Operator!=]" ){
    {
        amt::series<> lhs(100,10);
        amt::series<> rhs(100,20);

        REQUIRE( lhs != rhs );
    }
    {
        amt::series<> lhs(100,"Hello");
        amt::series<> rhs(100,"World");

        REQUIRE( lhs != rhs );
    }
    {
        amt::series<> lhs(100,'A');
        amt::series<> rhs(100,'B');

        REQUIRE( lhs != rhs );
    }
}

TEST_CASE( "Operator (<) for series object", "[Operator<]" ){
    {
        amt::series<> lhs(100,10);
        amt::series<> rhs(100,20);

        REQUIRE( lhs < rhs );
    }
    {
        amt::series<> lhs(100,"Hello");
        amt::series<> rhs(100,"World");

        REQUIRE( lhs < rhs );
    }
    {
        amt::series<> lhs(100,'A');
        amt::series<> rhs(100,'B');

        REQUIRE( lhs < rhs );
    }
}

TEST_CASE( "Operator (<=) for series object", "[Operator<=]" ){
    {
        amt::series<> lhs(100,10);
        amt::series<> rhs(100,10);

        REQUIRE( lhs <= rhs );
    }
    {
        amt::series<> lhs(100,"Hello");
        amt::series<> rhs(100,"Hello");

        REQUIRE( lhs <= rhs );
    }
    {
        amt::series<> lhs(100,'A');
        amt::series<> rhs(100,'A');

        REQUIRE( lhs <= rhs );
    }
}

TEST_CASE( "Operator (>) for series object", "[Operator>]" ){
    {
        amt::series<> lhs(100,20);
        amt::series<> rhs(100,10);

        REQUIRE( lhs > rhs );
    }
    {
        amt::series<> lhs(100,"World");
        amt::series<> rhs(100,"Hello");

        REQUIRE( lhs > rhs );
    }
    {
        amt::series<> lhs(100,'B');
        amt::series<> rhs(100,'A');

        REQUIRE( lhs > rhs );
    }
}

TEST_CASE( "Operator (>=) for series object", "[Operator>=]" ){
    {
        amt::series<> lhs(100,10);
        amt::series<> rhs(100,10);

        REQUIRE( lhs >= rhs );
    }
    {
        amt::series<> lhs(100,"Hello");
        amt::series<> rhs(100,"Hello");

        REQUIRE( lhs >= rhs );
    }
    {
        amt::series<> lhs(100,'A');
        amt::series<> rhs(100,'A');

        REQUIRE( lhs >= rhs );
    }
}

TEST_CASE( "Operator (+) for series object", "[Operator+]" ){
    {
        amt::series<> lhs(100,10);
        amt::series<> rhs(100,10);
        amt::series<> res(100,20);

        REQUIRE( (lhs + rhs) == res );
    }
    {
        amt::series<> lhs(100,"Hello");
        amt::series<> rhs(100,"Hello");
        amt::series<> res(100,"HelloHello");

        REQUIRE( (lhs + rhs) == res );
    }
    {
        amt::series<> lhs(100,'A');
        amt::series<> rhs(100,'A');
        amt::series<> res(100,'A' + 'A');

        REQUIRE( (lhs + rhs) == res );
    }
}

TEST_CASE( "Operator (-) for series object", "[Operator-]" ){
    {
        amt::series<> lhs(100,10);
        amt::series<> rhs(100,10);
        amt::series<> res(100,0);

        REQUIRE( (lhs - rhs) == res );
    }
    {
        amt::series<> lhs(100,'A');
        amt::series<> rhs(100,'A');
        amt::series<> res(100,'A' - 'A');

        REQUIRE( (lhs - rhs) == res );
    }
}

TEST_CASE( "Operator (*) for series object", "[Operator*]" ){
    {
        amt::series<> lhs(100,10.f);
        amt::series<> rhs(100,10.f);
        amt::series<> res(100,100.f);

        REQUIRE( (lhs * rhs) == res );
    }
}

TEST_CASE( "Operator (/) for series object", "[Operator/]" ){
    {
        amt::series<> lhs(100,10.f);
        amt::series<> rhs(100,10.f);
        amt::series<> res(100,1.f);

        REQUIRE( (lhs / rhs) == res );
    }
}

TEST_CASE( "Operator (==) for frame object", "[Operator==]" ){
    {
        amt::frame<> lhs = {
            amt::series<>(100,10)
        };
        amt::frame<> rhs = {
            amt::series<>(100,10)
        };

        REQUIRE( lhs == rhs );
    }
    {
        amt::frame<> lhs = {
            amt::series<>(100,"Hello")
        };
        amt::frame<> rhs = {
            amt::series<>(100,"Hello")
        };

        REQUIRE( lhs == rhs );
    }
    {
        amt::frame<> lhs = {
            amt::series<>(100,'A')
        };
        amt::frame<> rhs = {
            amt::series<>(100,'A')
        };

        REQUIRE( lhs == rhs );
    }
}

TEST_CASE( "Operator (!=) for frame object", "[Operator!=]" ){
    {
        amt::frame<> lhs = {
            amt::series<>(100,10)
        };
        amt::frame<> rhs = {
            amt::series<>(100,20)
        };

        REQUIRE( lhs != rhs );
    }
    {
        amt::frame<> lhs = {
            amt::series<>(100,"Hello")
        };
        amt::frame<> rhs = {
            amt::series<>(100,"World")
        };

        REQUIRE( lhs != rhs );
    }
    {
        amt::frame<> lhs = {
            amt::series<>(100,'A')
        };
        amt::frame<> rhs = {
            amt::series<>(100,'B')
        };

        REQUIRE( lhs != rhs );
    }
}

TEST_CASE( "Operator (<=) for frame object", "[Operator<=]" ){
    {
        amt::frame<> lhs = {
            amt::series<>(100,10)
        };
        amt::frame<> rhs = {
            amt::series<>(100,10)
        };

        REQUIRE( lhs <= rhs );
    }
    {
        amt::frame<> lhs = {
            amt::series<>(100,"Hello")
        };
        amt::frame<> rhs = {
            amt::series<>(100,"Hello")
        };

        REQUIRE( lhs <= rhs );
    }
    {
        amt::frame<> lhs = {
            amt::series<>(100,'A')
        };
        amt::frame<> rhs = {
            amt::series<>(100,'A')
        };

        REQUIRE( lhs <= rhs );
    }
}

TEST_CASE( "Operator (<) for frame object", "[Operator<]" ){
    {
        amt::frame<> lhs = {
            amt::series<>(100,10)
        };
        amt::frame<> rhs = {
            amt::series<>(100,20)
        };

        REQUIRE( lhs < rhs );
    }
    {
        amt::frame<> lhs = {
            amt::series<>(100,"Hello")
        };
        amt::frame<> rhs = {
            amt::series<>(100,"World")
        };

        REQUIRE( lhs < rhs );
    }
    {
        amt::frame<> lhs = {
            amt::series<>(100,'A')
        };
        amt::frame<> rhs = {
            amt::series<>(100,'B')
        };

        REQUIRE( lhs < rhs );
    }
}

TEST_CASE( "Operator (>=) for frame object", "[Operator>=]" ){
    {
        amt::frame<> lhs = {
            amt::series<>(100,10)
        };
        amt::frame<> rhs = {
            amt::series<>(100,10)
        };

        REQUIRE( lhs >= rhs );
    }
    {
        amt::frame<> lhs = {
            amt::series<>(100,"Hello")
        };
        amt::frame<> rhs = {
            amt::series<>(100,"Hello")
        };

        REQUIRE( lhs >= rhs );
    }
    {
        amt::frame<> lhs = {
            amt::series<>(100,'A')
        };
        amt::frame<> rhs = {
            amt::series<>(100,'A')
        };

        REQUIRE( lhs >= rhs );
    }
}


TEST_CASE( "Operator (>) for frame object", "[Operator>]" ){
    {
        amt::frame<> lhs = {
            amt::series<>(100,20)
        };
        amt::frame<> rhs = {
            amt::series<>(100,10)
        };

        REQUIRE( lhs > rhs );
    }
    {
        amt::frame<> lhs = {
            amt::series<>(100,"World")
        };
        amt::frame<> rhs = {
            amt::series<>(100,"Hello")
        };

        REQUIRE( lhs > rhs );
    }
    {
        amt::frame<> lhs = {
            amt::series<>(100,'B')
        };
        amt::frame<> rhs = {
            amt::series<>(100,'A')
        };

        REQUIRE( lhs > rhs );
    }
}

TEST_CASE( "Operator (+) for frame object", "[Operator+]" ){
    {
        amt::frame<> lhs = {
            amt::series<>(100,20)
        };
        amt::frame<> rhs = {
            amt::series<>(100,10)
        };
        amt::frame<> res = {
            amt::series<>(100,30)
        };

        REQUIRE( (lhs + rhs) == res );
    }
    {
        amt::frame<> lhs = {
            amt::series<>(100,"World")
        };
        amt::frame<> rhs = {
            amt::series<>(100,"Hello")
        };
        amt::frame<> res = {
            amt::series<>(100,"WorldHello")
        };

        REQUIRE( (lhs + rhs) == res );
    }
    {
        amt::frame<> lhs = {
            amt::series<>(100,'B')
        };
        amt::frame<> rhs = {
            amt::series<>(100,'A')
        };

        amt::frame<> res = {
            amt::series<>(100,'A' + 'B')
        };
        REQUIRE( (lhs + rhs) == res );
    }
}

TEST_CASE( "Operator (-) for frame object", "[Operator-]" ){
    {
        amt::frame<> lhs = {
            amt::series<>(100,20)
        };
        amt::frame<> rhs = {
            amt::series<>(100,10)
        };
        amt::frame<> res = {
            amt::series<>(100,10)
        };

        REQUIRE( (lhs - rhs) == res );
    }
    {
        amt::frame<> lhs = {
            amt::series<>(100,'B')
        };
        amt::frame<> rhs = {
            amt::series<>(100,'A')
        };

        amt::frame<> res = {
            amt::series<>(100,'B' - 'A')
        };
        REQUIRE( (lhs - rhs) == res );
    }
}

TEST_CASE( "Operator (*) for frame object", "[Operator*]" ){
    {
        amt::frame<> lhs = {
            amt::series<>(100,20)
        };
        amt::frame<> rhs = {
            amt::series<>(100,10)
        };
        amt::frame<> res = {
            amt::series<>(100,200)
        };

        REQUIRE( (lhs * rhs) == res );
    }
}

TEST_CASE( "Operator (/) for frame object", "[Operator/]" ){
    {
        amt::frame<> lhs = {
            amt::series<>(100,20.f)
        };
        amt::frame<> rhs = {
            amt::series<>(100,10.f)
        };
        amt::frame<> res = {
            amt::series<>(100,2.f)
        };

        REQUIRE( (lhs / rhs) == res );
    }
}
