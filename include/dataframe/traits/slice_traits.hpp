#if !defined(AMT_DATAFRAME_TRAITS_SLICE_TRAITS_HPP)
#define AMT_DATAFRAME_TRAITS_SLICE_TRAITS_HPP

#include <type_traits>

namespace amt {

template <typename V> struct basic_slice;

} // namespace amt

namespace amt {

template <typename T> struct is_slice : std::false_type {};

template <typename V> struct is_slice<basic_slice<V>> : std::true_type {};

template <typename T>
inline static constexpr bool is_slice_v = is_slice<std::decay_t<T>>::value;

template <typename T> concept Slice = is_slice_v<T>;

} // namespace amt

#endif // AMT_DATAFRAME_TRAITS_SLICE_TRAITS_HPP
