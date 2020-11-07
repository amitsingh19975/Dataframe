#if !defined(AMT_DATAFRAME_TRAITS_BASIC_TRAITS_HPP)
#define AMT_DATAFRAME_TRAITS_BASIC_TRAITS_HPP

#include <type_traits>
#include <ostream>

namespace amt {

template <typename T, typename U> concept HasLess = requires(T t, U u) {
    {t < u};
};

template <typename T, typename U> concept HasLessEqual = requires(T t, U u) {
    {t <= u};
};

template <typename T, typename U> concept HasGreater = requires(T t, U u) {
    {t > u};
};

template <typename T, typename U> concept HasGreaterEqual = requires(T t, U u) {
    {t >= u};
};

template <typename T, typename U> concept HasEqualEqual = requires(T t, U u) {
    {t == u};
};

template <typename T, typename U> concept HasNotEqual = requires(T t, U u) {
    {t != u};
};

template <typename T, typename U> concept HasPlus = requires(T t, U u) {
    {t + u};
};

template <typename T, typename U> concept HasMinus = requires(T t, U u) {
    {t - u};
};

template <typename T, typename U> concept HasMul = requires(T t, U u) {
    {t * u};
};

template <typename T, typename U> concept HasDiv = requires(T t, U u) {
    {t / u};
};

template <typename T, typename U> concept HasMod = requires(T t, U u) {
    {t % u};
};

template <typename T, typename U> concept HasBitwiseAND = requires(T t, U u) {
    {t & u};
};

template <typename T, typename U> concept HasBitwiseOR = requires(T t, U u) {
    {t | u};
};

template <typename T, typename U> concept HasBitwiseXOR = requires(T t, U u) {
    {t ^ u};
};

template <typename T, typename U> concept HasLogicalAND = requires(T t, U u) {
    {t && u};
};

template <typename T, typename U> concept HasLogicalOR = requires(T t, U u) {
    {t || u};
};

template <typename T, typename U> concept HasLeftShift = requires(T t, U u) {
    {t << u};
};

template <typename T, typename U> concept HasRightShift = requires(T t, U u) {
    {t >> u};
};

template <typename T> concept HasUnaryNot = requires(T t) { {!t}; };

template <typename T> concept HasUnaryPlus = requires(T t) { {+t}; };

template <typename T> concept HasUnaryMinus = requires(T t) { {-t}; };

template <typename T> concept HasUnaryNeg = requires(T t) { {~t}; };

template <typename T> concept HasPostInc = requires(T t) { {t++}; };

template <typename T> concept HasPreInc = requires(T t) { {++t}; };

template <typename T> concept HasPostDec = requires(T t) { {t--}; };

template <typename T> concept HasPreDec = requires(T t) { {--t}; };

template <typename T> concept HasStdToString = requires(T t) { {std::to_string(t)}; };

template <typename T> struct is_tuple : std::false_type {};

template <typename T>
inline static constexpr bool is_tuple_v = is_tuple<std::decay_t<T>>::value;

template <typename... Ts>
struct is_tuple<std::tuple<Ts...>> : std::true_type {};

template<typename T>
struct is_std_string : std::false_type{};

template<>
struct is_std_string< std::string > : std::true_type{};

template<typename T>
inline static constexpr bool is_std_string_v = is_std_string<T>::value;

} // namespace amt

#endif // AMT_DATAFRAME_TRAITS_BASIC_TRAITS_HPP
