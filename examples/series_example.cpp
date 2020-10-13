#undef NDEBUG
#include <algorithm>
#include <dataframe.hpp>
#include <frame.hpp>
#include <functional>
#include <functions.hpp>
#include <iostream>
#include <numeric>

int main(int, char **) {

    /// Standard supported types are
    /// int8_t, int16_t, int32_t, int64_t
    /// uint8_t, uint16_t, uint32_t, uint64_t
    /// float, double, bool, std::size_t, std::ptrdiff_t
    /// std::string

    /// To extend the types just need to add to the
    /// template arguments
    /// @code amt::series<T1,T2,...,Tn> temp; @endcode

    {
        amt::series<> ser(10);
        std::iota(ser.begin(), ser.end(), 1);
        std::cout << "------------ Out -------------" << std::endl;
        std::cout << "Series: " << ser
                  << std::endl; // [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        std::cout << std::endl;
    }

    {
        amt::series<> ser = {"Hello", "World"};
        std::cout << "------------ Out -------------" << std::endl;
        std::cout << "Series: " << ser << std::endl; // ["Hello", "Word"]
        std::cout << std::endl;
    }

    // Operators support if types support them
    {
        amt::series<> s1(10);
        amt::series<> s2(10);
        std::iota(s1.begin(), s1.end(), 1.f);
        std::iota(s2.begin(), s2.end(), 101.f);
        std::cout << "------------ Operators -------------" << std::endl;
        std::cout << "S1:      " << s1
                  << std::endl; // [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ]
        std::cout << "S2:      " << s2
                  << std::endl; // [ 101, 102, 103, 104, 105, 106, 107, 108,
                                // 109, 110 ]
        std::cout << "S1 + S2: " << amt::series<>(s1 + s2)
                  << std::endl; // [ 102, 104, 106, 108, 110, 112, 114, 116,
                                // 118, 120 ]
        std::cout << "S1 - S2: " << amt::series<>(s1 - s2)
                  << std::endl; // [ -100, -100, -100, -100, -100, -100, -100,
                                // -100, -100, -100 ]
        std::cout << "S1 * S2: " << amt::series<>(s1 * s2)
                  << std::endl; // [ 101, 204, 309, 416, 525, 636, 749, 864,
                                // 981, 1100 ]
        std::cout << "S1 / S2: " << amt::series<>(s1 / s2)
                  << std::endl; // [ 0.00990099, 0.0196078, 0.0291262,
                                // 0.0384615, 0.047619, 0.0566038, 0.0654206,
                                // 0.0740741, 0.0825688, 0.0909091 ]
        std::cout << std::endl;
    }

    // Operator+ for String
    {
        amt::series<> s1 = {"Hello "};
        amt::series<> s2 = {"World!"};
        std::cout << "------------ Operator(+) on String -------------"
                  << std::endl;
        std::cout << "S1:      " << s1 << std::endl;      // [ "Hello " ]
        std::cout << "S2:      " << s2 << std::endl;      // [ "World" ]
        std::cout << "S1 + S2: " << s1 + s2 << std::endl; // [ "Hello World!" ]
        std::cout << std::endl;
    }

    // Series View
    {
        amt::series<> ser(10);
        std::iota(ser.begin(), ser.end(), 1);
        std::cout << "------------ Out -------------" << std::endl;
        std::cout << "Slice Rep: [ First: Last: Stride]; Size = ( (Last - "
                     "First) / Stride ) + 1"
                  << std::endl;
        std::cout << "Series:             " << ser
                  << std::endl; // [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        std::cout << "View With [0:10:2]: "
                  << ser(amt::slice(amt::slice_stride(2))) << std::endl;
        std::cout << "View With [0:5:1]:  "
                  << ser(amt::slice(amt::slice_last(5))) << std::endl;
        std::cout << std::endl;
    }

    // Operators With Series View of Slice [0 : 10 : 2] and Slice [1 : 10 : 2]
    {
        amt::series<> s1(10);
        amt::series<> s2(10);
        std::iota(s1.begin(), s1.end(), 1.f);
        std::iota(s2.begin(), s2.end(), 101.f);
        auto V1 = s1(amt::slice(amt::slice_stride(2)));
        auto V2 = s2(amt::slice(1, amt::slice_stride(2)));
        std::cout << "------------ Operators -------------" << std::endl;
        std::cout << "V1:      " << V1 << std::endl; // [ 1, 3, 5, 7, 9 ]
        std::cout << "V2:      " << V2
                  << std::endl; // [ 102, 104, 106, 108, 110 ]
        std::cout << "V1 + V2: " << V1 + V2
                  << std::endl; // [ 103, 107, 111, 115, 119 ]
        std::cout << "V1 - V2: " << V1 - V2
                  << std::endl; // [ -101, -101, -101, -101, -101 ]
        std::cout << "V1 * V2: " << V1 * V2
                  << std::endl; // [ 102, 312, 530, 756, 990 ]
        std::cout << "V1 / V2: " << V1 / V2
                  << std::endl; // [ 0.00980392, 0.0288462, 0.0471698,
                                // 0.0648148, 0.0818182 ]
        std::cout << std::endl;
    }

    // Free Functions Accumulate
    {
        amt::series<> ser(10);
        std::iota(ser.begin(), ser.end(), 1.f);
        std::cout << "------------ Accumulate -------------" << std::endl;
        std::cout << "Series: " << ser
                  << std::endl; // [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        std::cout << "Sum:    " << amt::accumulate(ser, 0.f, std::plus<float>{})
                  << std::endl; // 55
        std::cout << "Sub:    "
                  << amt::accumulate(ser, 0.f, std::minus<float>{})
                  << std::endl; // -55
        std::cout << "Mul:    "
                  << amt::accumulate(ser, 1.f, std::multiplies<float>{})
                  << std::endl; // 3.6288e+06
        std::cout << "Div:    "
                  << amt::accumulate(ser, 1.f, std::divides<float>{})
                  << std::endl; // 2.75573e-07
        std::cout << std::endl;
    }

    // Free Functions Transform
    {
        amt::series<> ser(10);
        std::iota(ser.begin(), ser.end(), 1);

        auto sq = [](int val) { return val * val; };
        std::cout << "------------ Transform -------------" << std::endl;
        std::cout << "Before Series:               " << ser
                  << std::endl; // [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        std::cout << "Out Of Place Transformation: "
                  << amt::transform(ser, amt::out_place, sq)
                  << std::endl; // [ 2, 4, 6, 8, 10 ]
        std::cout << "After Series:                " << ser
                  << std::endl; // [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        std::cout << std::endl;
        std::cout << "Before Series:           " << ser
                  << std::endl; // [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        std::cout << "In Place Transformation: "
                  << amt::transform(ser, amt::in_place, sq)
                  << std::endl; // [ 2, 4, 6, 8, 10 ]
        std::cout << "After Series:            " << ser
                  << std::endl; // [ 2, 4, 6, 8, 10 ]
        std::cout << std::endl;
    }

    // Free Functions Filter
    {
        amt::series<> ser(10);
        std::iota(ser.begin(), ser.end(), 1);

        auto even = [](int val) { return val & 1; };
        std::cout << "------------ Filter -------------" << std::endl;
        std::cout << "Before Series:         " << ser
                  << std::endl; // [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        std::cout << "Out Of Place Filtered: "
                  << amt::filter(ser, amt::out_place, even)
                  << std::endl; // [ 2, 4, 6, 8, 10 ]
        std::cout << "After Series:          " << ser
                  << std::endl; // [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        std::cout << std::endl;
        std::cout << "Before Series:     " << ser
                  << std::endl; // [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        std::cout << "In Place Filtered: "
                  << amt::filter(ser, amt::in_place, even)
                  << std::endl; // [ 2, 4, 6, 8, 10 ]
        std::cout << "After Series:      " << ser
                  << std::endl; // [ 2, 4, 6, 8, 10 ]
        std::cout << std::endl;
    }

    // Free Functions To
    {
        amt::series<> ser(10);
        std::iota(ser.begin(), ser.end(), 1);

        std::cout << "------------ To -------------" << std::endl;
        std::cout << "Before Series:   " << ser
                  << std::endl; // [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        std::cout << "With Type:       " << amt::type_to_string(ser)
                  << std::endl; // std::int32_t
        std::cout
            << "Out Of Place To: " << amt::to<std::string>(ser, amt::out_place)
            << std::endl; // ["1", "2", "3", "4", "5", "6", "7", "8", "9", "10"]
        std::cout << "With Type:       " << amt::type_to_string(ser)
                  << std::endl; // std::int32_t
        std::cout << "After Series:    " << ser
                  << std::endl; // [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        std::cout << std::endl;

        std::cout << "Before Series: " << ser
                  << std::endl; // [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        std::cout << "With Type:     " << amt::type_to_string(ser)
                  << std::endl; // std::int32_t
        std::cout
            << "In Place To:   " << amt::to<std::string>(ser, amt::in_place)
            << std::endl; // ["1", "2", "3", "4", "5", "6", "7", "8", "9", "10"]
        std::cout
            << "After Series:  " << ser
            << std::endl; // ["1", "2", "3", "4", "5", "6", "7", "8", "9", "10"]
        std::cout << "With Type:     " << amt::type_to_string(ser)
                  << std::endl; // std::string
        std::cout << std::endl;
    }

    return 0;
}