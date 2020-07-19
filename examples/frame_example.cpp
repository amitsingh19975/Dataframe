#undef NDEBUG
#include <algorithm>
#include <dataframe.hpp>
#include <frame.hpp>
#include <functional>
#include <functions.hpp>
#include <iostream>
#include <numeric>

struct test {};

int main(int, char **) {

    /// Standard supported types are
    /// int8_t, int16_t, int32_t, int64_t
    /// uint8_t, uint16_t, uint32_t, uint64_t
    /// float, double, bool, std::size_t, std::ptrdiff_t
    /// std::string

    /// To extend the types just need to add to the
    /// template arguments
    /// @code amt::frame<T1,T2,...,Tn> temp; @endcode

    {
        amt::frame<> fr = {{1, 2, 3, 4, 5}, {"A", "B", "C", "D", "E"}};

        std::cout << "------------ Out -------------" << std::endl;
        //[ Column: "0", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "1", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "Frame: { " << std::endl << fr << "}" << std::endl;
        std::cout << std::endl;
    }

    // With User Column Name
    {
        amt::frame<> fr = {{"Number", {1, 2, 3, 4, 5}},
                           {"Alphabet", {"A", "B", "C", "D", "E"}}};

        std::cout << "------------ Out -------------" << std::endl;
        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "Frame: { " << std::endl << fr << "}" << std::endl;
        std::cout << std::endl;
    }

    // Operator Support
    {
        /// Can not apply any operator other than operator(+)
        /// because in frame only those operators are applicable
        /// which is a subset of all the data types in the frame.
        /// Other than that all the operators are can be applied
        amt::frame<> fr = {{"Number", {1, 2, 3, 4, 5}},
                           {"Alphabet", {"A", "B", "C", "D", "E"}}};

        std::cout << "------------ Operator -------------" << std::endl;
        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "F1     : { " << std::endl << fr << "}" << std::endl;

        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "F2     : { " << std::endl << fr << "}" << std::endl;

        //[ Column: "0", Type: "std::int32_t" ]{ 2, 4, 6, 8, 10 }
        //[ Column: "1", Type: "std::string" ]{ "AA", "BB", "CC", "DD", "EE" }
        std::cout << "F1 + F2: { " << std::endl
                  << (fr + fr) << "}" << std::endl;
        std::cout << std::endl;
    }

    // Free Function Accumulate
    {
        /// Functions will applied to those series which can invoke it
        /// Here column "Alphabet" will not be able to invoke it only
        /// column "Number" will give the result
        amt::frame<> fr = {{"Number", {1.f, 2.f, 3.f, 4.f, 5.f}},
                           {"Alphabet", {"A", "B", "C", "D", "E"}}};

        std::cout << "------------ Accumulate -------------" << std::endl;
        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "Frame : { " << std::endl << fr << "}" << std::endl;
        std::cout << "Sum:    " << amt::accumulate(fr, 0.f, std::plus<float>{})
                  << std::endl; // 15
        std::cout << "Sub:    " << amt::accumulate(fr, 0.f, std::minus<float>{})
                  << std::endl; // -15
        std::cout << "Mul:    "
                  << amt::accumulate(fr, 1.f, std::multiplies<float>{})
                  << std::endl; // 120
        std::cout << "Div:    "
                  << amt::accumulate(fr, 1.f, std::divides<float>{})
                  << std::endl; // 0.00833333
        std::cout << std::endl;
    }

    // Free Function Drop Columns With Indices
    {
        amt::frame<> fr = {{"Number", {1.f, 2.f, 3.f, 4.f, 5.f}},
                           {"Alphabet", {"A", "B", "C", "D", "E"}}};

        std::cout << "------------ Drop Using Column Index -------------"
                  << std::endl;
        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "Before Frame          : { " << std::endl
                  << fr << "}" << std::endl;

        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "Out Of Place Drop Cols: { " << std::endl
                  << amt::drop_cols(fr, amt::out_place, amt::index_list{0})
                  << "}" << std::endl;

        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "After Frame           : { " << std::endl
                  << fr << "}" << std::endl;

        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "Before Frame      : { " << std::endl
                  << fr << "}" << std::endl;

        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "In Place Drop Cols: { " << std::endl
                  << amt::drop_cols(fr, amt::in_place, amt::index_list{0})
                  << "}" << std::endl;

        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "After Frame       : { " << std::endl
                  << fr << "}" << std::endl;

        std::cout << std::endl;
    }

    // Free Function Drop Columns With Column Name
    {
        amt::frame<> fr = {{"Number", {1.f, 2.f, 3.f, 4.f, 5.f}},
                           {"Alphabet", {"A", "B", "C", "D", "E"}}};

        std::cout << "------------ Drop Columns Using Column Name -------------"
                  << std::endl;
        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "Before Frame          : { " << std::endl
                  << fr << "}" << std::endl;

        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        std::cout << "Out Of Place Drop Cols: { " << std::endl
                  << amt::drop_cols(fr, amt::out_place,
                                    amt::name_list{"Alphabet"})
                  << "}" << std::endl;

        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "After Frame           : { " << std::endl
                  << fr << "}" << std::endl;

        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "Before Frame      : { " << std::endl
                  << fr << "}" << std::endl;

        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        std::cout << "In Place Drop Cols: { " << std::endl
                  << amt::drop_cols(fr, amt::in_place,
                                    amt::name_list{"Alphabet"})
                  << "}" << std::endl;

        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        std::cout << "After Frame       : { " << std::endl
                  << fr << "}" << std::endl;

        std::cout << std::endl;
    }

    // Free Function Drop Rows
    {
        amt::frame<> fr = {{"Number", {1.f, 2.f, 3.f, 4.f, 5.f}},
                           {"Alphabet", {"A", "B", "C", "D", "E"}}};

        std::cout << "------------ Drop Rows -------------" << std::endl;
        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "Before Frame          : { " << std::endl
                  << fr << "}" << std::endl;

        //[ Column: "Number", Type: "float" ]{ 2, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "B", "E" }
        std::cout << "Out Of Place Drop Rows: { " << std::endl
                  << amt::drop_rows(fr, amt::out_place,
                                    amt::index_list{0, 3, 2})
                  << "}" << std::endl;

        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "After Frame           : { " << std::endl
                  << fr << "}" << std::endl;

        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "Before Frame      : { " << std::endl
                  << fr << "}" << std::endl;

        //[ Column: "Number", Type: "float" ]{ 2, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "B", "E" }
        std::cout << "In Place Drop Rows: { " << std::endl
                  << amt::drop_rows(fr, amt::in_place, amt::index_list{0, 3, 2})
                  << "}" << std::endl;

        //[ Column: "Number", Type: "float" ]{ 2, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "B", "E" }
        std::cout << "After Frame       : { " << std::endl
                  << fr << "}" << std::endl;

        std::cout << std::endl;
    }

    // Free Function Filter
    {
        /// Functions will applied to those series which can invoke it
        /// Here column "Alphabet" will not be able to invoke it only
        /// column "Number" will give the result
        amt::frame<> fr = {{"Number", {1, 2, 3, 4, 5}},
                           {"Alphabet", {"A", "B", "C", "D", "E"}}};

        auto even = [](int val) { return val & 1; };

        std::cout << "------------ Filter -------------" << std::endl;
        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "Before Frame       : { " << std::endl
                  << fr << "}" << std::endl;

        //[ Column: "Number", Type: "std::int32_t" ]{ 2, 4 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "B", "D" }
        std::cout << "Out Of Place Filter: { " << std::endl
                  << amt::filter(fr, amt::out_place, even) << "}" << std::endl;

        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "After Frame        : { " << std::endl
                  << fr << "}" << std::endl;

        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "Before Frame   : { " << std::endl
                  << fr << "}" << std::endl;

        //[ Column: "Number", Type: "std::int32_t" ]{ 2, 4 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "B", "D" }
        std::cout << "In Place Filter: { " << std::endl
                  << amt::filter(fr, amt::in_place, even) << "}" << std::endl;

        //[ Column: "Number", Type: "std::int32_t" ]{ 2, 4 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "B", "D" }
        std::cout << "After Frame    : { " << std::endl
                  << fr << "}" << std::endl;

        std::cout << std::endl;
    }

    // Free Function Transform
    {
        /// Functions will applied to those series which can invoke it
        /// Here column "Alphabet" will not be able to invoke it only
        /// column "Number" will give the result
        amt::frame<> fr = {{"Number", {1, 2, 3, 4, 5}},
                           {"Alphabet", {"A", "B", "C", "D", "E"}}};

        auto sq = [](int val) { return val * val; };

        std::cout << "------------ Transform -------------" << std::endl;
        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "Before Frame          : { " << std::endl
                  << fr << "}" << std::endl;

        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 4, 9, 16, 25 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "Out Of Place Transform: { " << std::endl
                  << amt::transform(fr, amt::out_place, sq) << "}" << std::endl;

        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "After Frame           : { " << std::endl
                  << fr << "}" << std::endl;

        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "Before Frame      : { " << std::endl
                  << fr << "}" << std::endl;

        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 4, 9, 16, 25 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "In Place Transform: { " << std::endl
                  << amt::transform(fr, amt::in_place, sq) << "}" << std::endl;

        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 4, 9, 16, 25 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E" }
        std::cout << "After Frame       : { " << std::endl
                  << fr << "}" << std::endl;

        std::cout << std::endl;
    }

    // Free Function To
    {
        /// Functions will applied to those series which can invoke it
        /// Here column "Alphabet" will not be able to invoke it only
        /// column "Number" will give the result
        amt::frame<> fr = {{"Number", {1, 2, 3, 4, 5}},
                           {"Alphabet", {"1", "2", "3", "4", "5"}}};

        std::cout << "------------ To -------------" << std::endl;
        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "1", "2", "3", "4", "5" }
        std::cout << "Before Frame   : { " << std::endl
                  << fr << "}" << std::endl;

        //[ Column: "Number", Type: "float" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "float" ]{ 1, 2, 3, 4, 5 }
        std::cout << "Out Of Place To: { " << std::endl
                  << amt::to<float>(fr, amt::out_place) << "}" << std::endl;

        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "1", "2", "3", "4", "5" }
        std::cout << "After Frame    : { " << std::endl
                  << fr << "}" << std::endl;

        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "std::string" ]{ "1", "2", "3", "4", "5" }
        std::cout << "Before Frame: { " << std::endl << fr << "}" << std::endl;

        //[ Column: "Number", Type: "float" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "float" ]{ 1, 2, 3, 4, 5 }
        std::cout << "In Place To : { " << std::endl
                  << amt::to<float>(fr, amt::in_place) << "}" << std::endl;

        //[ Column: "Number", Type: "float" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Alphabet", Type: "float" ]{ 1, 2, 3, 4, 5 }
        std::cout << "After Frame : { " << std::endl << fr << "}" << std::endl;

        std::cout << std::endl;
    }

    // Frame View
    {
        amt::frame<> fr = {{"Number", {1, 2, 3, 4, 5}},
                           {"Volume", {1.3, 25.4, 3.12, 423.2, 5.33}},
                           {"Char Label", {'a', 'b', 'c', 'd', 'e'}},
                           {"Alphabet", {"A", "B", "C", "D", "E"}}};

        std::cout << "------------ Frame View -------------" << std::endl;
        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Volume", Type: "double" ]{ 1.3, 25.4, 3.12, 423.2, 5.33 }
        //[ Column: "Char Label", Type: "std::int8_t" ]{ 'a', 'b', 'c', 'd', 'e'
        //} [ Column: "Alphabet", Type: "std::string" ]{ "A", "B", "C", "D", "E"
        //}
        std::cout << "F1: { " << std::endl << fr << "}" << std::endl;

        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 3, 5 }
        //[ Column: "Char Label", Type: "std::int8_t" ]{ a, c, e }
        std::cout << "V1: { " << std::endl
                  << fr(amt::slice(amt::slice_stride(2)),
                        amt::slice(0ul, 5ul, 2ul))
                  << "}" << std::endl;

        //[ Column: "Number", Type: "std::int32_t" ]{ 1, 2, 3, 4, 5 }
        //[ Column: "Char Label", Type: "std::int8_t" ]{ a, b, c, d, e }
        std::cout << "V2: { " << std::endl
                  << fr(amt::slice(amt::slice_stride(2))) << "}" << std::endl;

        std::cout << std::endl;
    }

    return 0;
}