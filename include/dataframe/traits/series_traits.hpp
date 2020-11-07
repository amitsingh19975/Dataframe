#if !defined(AMT_DATAFRAME_TRAITS_SERIES_TRAITS_HPP)
#define AMT_DATAFRAME_TRAITS_SERIES_TRAITS_HPP

#include <type_traits>

namespace amt {

template <typename V> struct basic_series;

template <typename V, typename Axis> struct basic_series_view;

} // namespace amt

namespace amt {

template <typename T> struct is_series : std::false_type {};

template <typename V> struct is_series<basic_series<V>> : std::true_type {};

template <typename V, typename A>
struct is_series<basic_series_view<V, A>> : std::true_type {};

template <typename T>
inline static constexpr bool is_series_v = is_series<std::decay_t<T>>::value;

template <typename T> struct is_series_view : std::false_type {};

template <typename V, typename A>
struct is_series_view<basic_series_view<V, A>> : std::true_type {};

template <typename T>
inline static constexpr bool is_series_view_v =
    is_series_view<std::decay_t<T>>::value;

template <typename T> concept Series = is_series_v<T>;

template <typename T> concept SeriesView = is_series_view_v<T>;

template <typename T>
inline static constexpr bool is_pure_series_v =
    is_series_v<T> && !is_series_view_v<T>;

template <typename T> concept PureSeries = is_pure_series_v<T>;

template <typename T> struct series_type;

template <PureSeries T> struct series_type<T> { using type = T; };

template <SeriesView T> struct series_type<T> {
    using type = typename T::view_of;
};

template <typename T> using series_type_t = typename series_type<T>::type;

template <typename... Ts> struct series_result;

template <Series T, typename U>
requires(!Series<U>) struct series_result<T, U> {
    using type = series_type_t<T>;
};

template <typename T, Series U>
requires(!Series<T>) struct series_result<T, U> {
    using type = series_type_t<U>;
};

template <Series T, Series U> struct series_result<T, U> {
    using type = series_type_t<T>;
};

template <Series T> struct series_result<T> { using type = series_type_t<T>; };

template <typename... Ts>
using series_result_t = typename series_result<std::decay_t<Ts>...>::type;

template <Series L, Series R>
inline static constexpr bool is_series_same_v =
    std::is_same_v<std::decay_t<typename std::decay_t<L>::value_type>,
                   std::decay_t<typename std::decay_t<R>::value_type>>;

} // namespace amt

#endif // AMT_DATAFRAME_TRAITS_SERIES_TRAITS_HPP
