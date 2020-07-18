#include <catch2/catch.hpp>
#include <slice.hpp>

SCENARIO( "basic_slice construction", "[Slice]" ){
    using slice_type = std::size_t;
    constexpr auto const npos = std::numeric_limits<slice_type>::max();

    GIVEN( "the construction with no arguments" ){
        amt::basic_slice<slice_type> sl;
        REQUIRE(sl.first() == 0ul);
        REQUIRE(sl.last() == npos);
        REQUIRE(sl.step() == 1ul);
    }

    GIVEN( "the construction with the one argument" ){
        WHEN( "the arument is of the type unsigned integral" ){
            amt::basic_slice<slice_type> sl(1ul);
            REQUIRE(sl.first() == 1ul);
            REQUIRE(sl.last() == npos);
            REQUIRE(sl.step() == 1ul);
        }
        
        WHEN( "the arument is of the type slice_first" ){
            amt::basic_slice<slice_type> sl(amt::slice_first(1ul));
            REQUIRE(sl.first() == 1ul);
            REQUIRE(sl.last() == npos);
            REQUIRE(sl.step() == 1ul);
        }
        
        WHEN( "the arument is of the type slice_last" ){
            amt::basic_slice<slice_type> sl(amt::slice_last(10ul));
            REQUIRE(sl.first() == 0ul);
            REQUIRE(sl.last() == 10ul);
            REQUIRE(sl.step() == 1ul);
        }
        
        WHEN( "the arument is of the type slice_stride" ){
            amt::basic_slice<slice_type> sl(amt::slice_stride(10ul));
            REQUIRE(sl.first() == 0ul);
            REQUIRE(sl.last() == npos);
            REQUIRE(sl.step() == 10ul);
        }
    }

    GIVEN( "the construction with the two arguments" ){
        WHEN( "the first arument is of the type unsigned integral and the second any valid type" ){
            {
                amt::basic_slice<slice_type> sl(1ul, 10ul);
                REQUIRE(sl.first() == 1ul);
                REQUIRE(sl.last() == 10ul);
                REQUIRE(sl.step() == 1ul);
            }
            {
                amt::basic_slice<slice_type> sl(1ul, amt::slice_first(10ul));
                REQUIRE(sl.first() == 10ul);
                REQUIRE(sl.last() == npos);
                REQUIRE(sl.step() == 1ul);
            }
            {
                amt::basic_slice<slice_type> sl(1ul, amt::slice_last(10ul));
                REQUIRE(sl.first() == 1ul);
                REQUIRE(sl.last() == 10ul);
                REQUIRE(sl.step() == 1ul);
            }
            {
                amt::basic_slice<slice_type> sl(1ul, amt::slice_stride(10ul));
                REQUIRE(sl.first() == 1ul);
                REQUIRE(sl.last() == npos);
                REQUIRE(sl.step() == 10ul);
            }
        }

        WHEN( "the first arument is of the type slice_first and the second any valid type" ){
            {
                amt::basic_slice<slice_type> sl(amt::slice_first(1ul), 10ul);
                REQUIRE(sl.first() == 1ul);
                REQUIRE(sl.last() == 10ul);
                REQUIRE(sl.step() == 1ul);
            }
            {
                amt::basic_slice<slice_type> sl(amt::slice_first(1ul), amt::slice_first(10ul));
                REQUIRE(sl.first() == 10ul);
                REQUIRE(sl.last() == npos);
                REQUIRE(sl.step() == 1ul);
            }
            {
                amt::basic_slice<slice_type> sl(amt::slice_first(1ul), amt::slice_last(10ul));
                REQUIRE(sl.first() == 1ul);
                REQUIRE(sl.last() == 10ul);
                REQUIRE(sl.step() == 1ul);
            }
            {
                amt::basic_slice<slice_type> sl(amt::slice_first(1ul), amt::slice_stride(10ul));
                REQUIRE(sl.first() == 1ul);
                REQUIRE(sl.last() == npos);
                REQUIRE(sl.step() == 10ul);
            }
        }

        WHEN( "the first arument is of the type slice_last and the second any valid type" ){
            {
                amt::basic_slice<slice_type> sl(amt::slice_last(1ul), 10ul);
                REQUIRE(sl.first() == 0ul);
                REQUIRE(sl.last() == 10ul);
                REQUIRE(sl.step() == 1ul);
            }
            {
                amt::basic_slice<slice_type> sl(amt::slice_last(1ul), amt::slice_first(10ul));
                REQUIRE(sl.first() == 10ul);
                REQUIRE(sl.last() == 1ul);
                REQUIRE(sl.step() == 1ul);
            }
            {
                amt::basic_slice<slice_type> sl(amt::slice_last(1ul), amt::slice_last(10ul));
                REQUIRE(sl.first() == 0ul);
                REQUIRE(sl.last() == 10ul);
                REQUIRE(sl.step() == 1ul);
            }
            {
                amt::basic_slice<slice_type> sl(amt::slice_last(1ul), amt::slice_stride(10ul));
                REQUIRE(sl.first() == 0ul);
                REQUIRE(sl.last() == 1ul);
                REQUIRE(sl.step() == 10ul);
            }
        }

        WHEN( "the first arument is of the type slice_stride and the second any valid type" ){
            {
                amt::basic_slice<slice_type> sl(amt::slice_stride(5ul), 10ul);
                REQUIRE(sl.first() == 0ul);
                REQUIRE(sl.last() == 10ul);
                REQUIRE(sl.step() == 5ul);
            }
            {
                amt::basic_slice<slice_type> sl(amt::slice_stride(5ul), amt::slice_first(10ul));
                REQUIRE(sl.first() == 10ul);
                REQUIRE(sl.last() == npos);
                REQUIRE(sl.step() == 5ul);
            }
            {
                amt::basic_slice<slice_type> sl(amt::slice_stride(5ul), amt::slice_last(10ul));
                REQUIRE(sl.first() == 0ul);
                REQUIRE(sl.last() == 10ul);
                REQUIRE(sl.step() == 5ul);
            }
            {
                amt::basic_slice<slice_type> sl(amt::slice_stride(5ul), amt::slice_stride(10ul));
                REQUIRE(sl.first() == 0ul);
                REQUIRE(sl.last() == npos);
                REQUIRE(sl.step() == 10ul);
            }
        }
    }

    GIVEN( "the construction with the three arguments with the permutaion of [unsigned integral, slice_first, slice_last, slice_stide]" ){

        {
            amt::basic_slice<slice_type> sl(amt::slice_first(1ul), amt::slice_stride(10ul), amt::slice_last(100ul));
            REQUIRE(sl.first() == 1ul);
            REQUIRE(sl.last() == 100ul);
            REQUIRE(sl.step() == 10ul);
        }
        {
            amt::basic_slice<slice_type> sl(1ul, amt::slice_stride(10ul), amt::slice_last(100ul));
            REQUIRE(sl.first() == 1ul);
            REQUIRE(sl.last() == 100ul);
            REQUIRE(sl.step() == 10ul);
        }
        {
            amt::basic_slice<slice_type> sl(amt::slice_first(1ul), 10ul, amt::slice_last(100ul));
            REQUIRE(sl.first() == 1ul);
            REQUIRE(sl.last() == 100ul);
            REQUIRE(sl.step() == 1ul);
        }

    }
}

TEST_CASE("Normalizing Slice","[Slice]"){
    using slice_type = std::size_t;
    constexpr auto const npos = std::numeric_limits<slice_type>::max();
    {
        amt::basic_slice<slice_type> sl(amt::slice_first(1ul), amt::slice_stride(10ul), amt::slice_last(100ul));
        REQUIRE(sl.first() == 1ul);
        REQUIRE(sl.last() == 100ul);
        REQUIRE(sl.step() == 10ul);

        sl = amt::detail::norm_slice(std::move(sl), 50ul);
        REQUIRE(sl.first() == 1ul);
        REQUIRE(sl.last() == 49ul);
        REQUIRE(sl.step() == 10ul);
        REQUIRE(sl.size() == ( ( (49ul - 1ul) / 10ul ) + 1ul ) );
    }
    {
        amt::basic_slice<slice_type> sl(amt::slice_first(npos), amt::slice_stride(10ul), amt::slice_last(npos));
        REQUIRE(sl.first() == npos);
        REQUIRE(sl.last() == npos);
        REQUIRE(sl.step() == 10ul);

        sl = amt::detail::norm_slice(std::move(sl), 50ul);
        REQUIRE(sl.first() == 49ul);
        REQUIRE(sl.last() == 49ul);
        REQUIRE(sl.step() == 10ul);
        REQUIRE(sl.size() == 1ul );
    }
}